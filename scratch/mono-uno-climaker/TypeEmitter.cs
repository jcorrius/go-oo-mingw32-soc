/*************************************************************************
 *
 *  $RCSfile: TypeEmitter.cs,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: martink $ $Date: 2005/09/29 13:01:33 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): Martin Kretzschmar_____________________
 *
 *
 ************************************************************************/

using System;
using System.Collections;
using System.Diagnostics;
using System.Reflection;
using System.Reflection.Emit;

using unoidl.com.sun.star.reflection;
using unoidl.com.sun.star.uno;

public class TypeEmitter : IDisposable
{
    ModuleBuilder m_module_builder;
    Assembly[] m_extra_assemblies;

    MethodInfo m_method_info_Type_GetTypeFromHandle;

    Type m_type_Exception;
    Type m_type_RuntimeException;

    Hashtable m_incomplete_ifaces;
    Hashtable m_incomplete_services;
    Hashtable m_incomplete_singletons;
    Hashtable m_generated_structs;

    static MethodAttributes c_ctor_method_attr =
        MethodAttributes.Public |
        MethodAttributes.HideBySig |
        MethodAttributes.SpecialName |
        MethodAttributes.RTSpecialName
        /* | FIXME ??? compiler does not know Instance ===
           MethodAttributes.Instance */;

    class iface_entry
    {
        public XInterfaceTypeDescription2 m_xType;
        public TypeBuilder m_type_builder;
    };

    class struct_entry
    {
        public string[] m_member_names;
        public Type[] m_param_types;
        public ConstructorInfo m_default_ctor;
        public ConstructorInfo m_ctor;
    }

    class service_entry
    {
        public TypeBuilder m_type_builder;
        public XServiceTypeDescription2 m_xType;
    }

    class singleton_entry
    {
        public TypeBuilder m_type_builder;
        public XSingletonTypeDescription2 m_xType;
    }

    static String to_cts_name(string uno_name)
    {
        return "unoidl." + uno_name;
    }

    static object to_cli_constant(uno.Any value) // FIXME redundant
    {
        return value.Value;
    }

    static void emit_ldarg(ILGenerator code, int index)
    {
        switch (index)
        {
        case 0:
            code.Emit(OpCodes.Ldarg_0);
            break;
        case 1:
            code.Emit(OpCodes.Ldarg_1);
            break;
        case 2:
            code.Emit(OpCodes.Ldarg_2);
            break;
        case 3:
            code.Emit(OpCodes.Ldarg_3);
            break;
        default:
            if (index < 0x100)
                code.Emit(OpCodes.Ldarg_S, (byte) index);
            else if (index < 0x8000)
                code.Emit(OpCodes.Ldarg_S, (short) index);
            else
                code.Emit(OpCodes.Ldarg, index);
            break;
        }
    }

    static void polymorphicStructNameToStructName(ref string sPolyName)
    {
        if (!sPolyName.EndsWith(">"))
            return;

        int index = sPolyName.LastIndexOf('<');
        Trace.Assert(index != -1);
        sPolyName = sPolyName.Substring(0, index);
    }

    public Assembly type_resolve(object sender, ResolveEventArgs args)
    {
        string cts_name = args.Name;
        Type ret_type = m_module_builder.GetType(cts_name, false /* no exc */);

        if (ret_type == null)
        {
            iface_entry entry = (iface_entry) m_incomplete_ifaces[cts_name];
            if (entry == null)
                ret_type = entry.m_type_builder;
        }
        if (ret_type == null)
        {
            foreach (Assembly assembly in m_extra_assemblies)
            {
                ret_type = assembly.GetType(cts_name, false /* no exc */);
                if (ret_type != null)
                {
                    if (Climaker.g_verbose)
                    {
                        Console.WriteLine("> resolving type {0} from {1}.",
                                          cts_name,
                                          ret_type.Assembly.FullName);
                    }
                    break;
                }
            }
        }

        if (ret_type != null)
            return ret_type.Assembly;

        return null;
    }

    Type get_type(string cts_name, bool throw_exc)
    {
        Type ret_type = m_module_builder.GetType(cts_name, false);

        if (ret_type == null)
        {
            iface_entry entry = (iface_entry) m_incomplete_ifaces[cts_name];
            if (entry != null)
                ret_type = entry.m_type_builder;
        }

        // try the cli_basetypes assembly
        if (ret_type == null)
        {
            ret_type = Type.GetType(cts_name + ",cli_basetypes");
        }

        if (ret_type == null)
        {
            // may call on type_resolve()
            return Type.GetType(cts_name, throw_exc);
        }
        else
        {
            return ret_type;
        }
    }

    Type get_type_Exception()
    {
        if (m_type_Exception == null)
        {
            m_type_Exception = get_type("unoidl.com.sun.star.uno.Exception",
                                        false /* no exc */);
            if (m_type_Exception == null)
            {
                // define hardcoded type unoidl.com.sun.star.uno.Exception
                TypeBuilder type_builder =
                    m_module_builder.DefineType(
                        "unoidl.com.sun.star.uno.Exception",
                        (TypeAttributes.Public |
                         TypeAttributes.BeforeFieldInit |
                         TypeAttributes.AnsiClass),
                        typeof(System.Exception));
                FieldBuilder field_Context = type_builder.DefineField(
                    "Context", typeof(System.Object), FieldAttributes.Public);
                // default .ctor
                type_builder.DefineDefaultConstructor(c_ctor_method_attr);
                // .ctor
                Type[] param_types = new System.Type[] {typeof(System.String),
                                                        typeof(System.Object)};
                ConstructorBuilder ctor_builder =
                    type_builder.DefineConstructor(
                        c_ctor_method_attr, CallingConventions.Standard,
                        param_types);
                ctor_builder.DefineParameter(
                    1, ParameterAttributes.In, "Message");
                ctor_builder.DefineParameter(
                    2, ParameterAttributes.In, "Context");
                ILGenerator code = ctor_builder.GetILGenerator();
                code.Emit(OpCodes.Ldarg_0);
                code.Emit(OpCodes.Ldarg_1);
                param_types = new System.Type[] {typeof(System.String)};
                code.Emit(OpCodes.Call,
                          typeof(System.Exception).GetConstructor(param_types));
                code.Emit(OpCodes.Ldarg_0);
                code.Emit(OpCodes.Ldarg_2);
                code.Emit(OpCodes.Stfld, field_Context);
                code.Emit(OpCodes.Ret);

                if (Climaker.g_verbose)
                {
                    Console.WriteLine(
                        "> emitting exception type " +
                        "unoidl.com.sun.star.uno.Exception");
                }
                m_type_Exception = type_builder.CreateType();
            }
        }
        return m_type_Exception;
    }

    Type get_type_RuntimeException()
    {
        if (m_type_RuntimeException == null)
        {
            m_type_RuntimeException = get_type(
                "unoidl.com.sun.star.uno.RuntimeException", false /* no exc */);
            if (m_type_RuntimeException == null)
            {
                // define hardcoded type unoidl.com.sun.star.uno.RuntimeException
                Type type_Exception = get_type_Exception();
                TypeBuilder type_builder =
                    m_module_builder.DefineType(
                        "unoidl.com.sun.star.uno.RuntimeException",
                        (TypeAttributes.Public |
                         TypeAttributes.BeforeFieldInit |
                         TypeAttributes.AnsiClass),
                        type_Exception);
                // default .ctor
                type_builder.DefineDefaultConstructor(c_ctor_method_attr);
                // .ctor
                Type[] param_types = new Type[] {typeof(System.String),
                                                 typeof(System.Object)};
                ConstructorBuilder ctor_builder =
                    type_builder.DefineConstructor(
                        c_ctor_method_attr, CallingConventions.Standard,
                        param_types);
                ctor_builder.DefineParameter(
                    1, ParameterAttributes.In, "Message");
                ctor_builder.DefineParameter(
                    2, ParameterAttributes.In, "Context");
                ILGenerator code = ctor_builder.GetILGenerator();
                code.Emit(OpCodes.Ldarg_0);
                code.Emit(OpCodes.Ldarg_1);
                code.Emit(OpCodes.Ldarg_2);
                code.Emit(OpCodes.Call,
                          type_Exception.GetConstructor(param_types));
                code.Emit(OpCodes.Ret);

                if (Climaker.g_verbose)
                {
                    Console.WriteLine(
                        "> emitting exception type " +
                        "unoidl.com.sun.star.uno.RuntimeException");
                }
                m_type_RuntimeException = type_builder.CreateType();
            }
        }
        return m_type_RuntimeException;
    }

    Type get_type(XConstantTypeDescription xType)
    {
        string cts_name = to_cts_name(xType.getName());
        Type ret_type = get_type(cts_name, false /* no exc */);
        if (ret_type == null)
        {
            XConstantTypeDescription xConstant = 
                (XConstantTypeDescription) xType;
            object constant =
                to_cli_constant(xConstant.getConstantValue());
            TypeBuilder type_builder = m_module_builder.DefineType(
                cts_name,
                (TypeAttributes.Public |
                 TypeAttributes.Sealed |
                 TypeAttributes.BeforeFieldInit |
                 TypeAttributes.AnsiClass));
            FieldBuilder field_builder = type_builder.DefineField(
                cts_name.Substring(cts_name.LastIndexOf('.') + 1),
                constant.GetType(),
                (FieldAttributes.Public |
                 FieldAttributes.Static |
                 FieldAttributes.Literal));
            field_builder.SetConstant(constant);

            if (Climaker.g_verbose)
            {
                Console.WriteLine("> emitting constant type {0}", cts_name);
            }
            ret_type = type_builder.CreateType();
        }
        return ret_type;
    }

    Type get_type(XConstantsTypeDescription xType)
    {
        string cts_name = to_cts_name(xType.getName());
        Type ret_type = get_type(cts_name, false /* no exc */);
        if (ret_type == null)
        {
            TypeBuilder type_builder = m_module_builder.DefineType(
                cts_name,
                (TypeAttributes.Public |
                 TypeAttributes.Sealed |
                 TypeAttributes.BeforeFieldInit |
                 TypeAttributes.AnsiClass));
            XConstantTypeDescription[] seq_constants = xType.getConstants();

            foreach (XConstantTypeDescription xConstant in seq_constants)
            {
                object constant = to_cli_constant(xConstant.getConstantValue());
                string uno_name = xConstant.getName();
                FieldBuilder field_builder = type_builder.DefineField(
                    uno_name.Substring(uno_name.LastIndexOf('.') + 1),
                    constant.GetType(),
                    (FieldAttributes.Public |
                     FieldAttributes.Static |
                     FieldAttributes.Literal));
                field_builder.SetConstant(constant);
            }

            if (Climaker.g_verbose)
            {
                Console.WriteLine("> emitting constants group type {0}", cts_name);
            }
            ret_type = type_builder.CreateType();
        }
        return ret_type;
    }

    Type get_type(XEnumTypeDescription xType)
    {
        string cts_name = to_cts_name(xType.getName());
        Type ret_type = get_type(cts_name, false /* no exc */);
        if (ret_type == null)
        {
//             EnumBuilder enum_builder = m_module_builder.DefineEnum(
//                 cts_name,
//                 (TypeAttributes.Public |
//                  TypeAttributes.Sealed |
//                  TypeAttributes.AnsiClass),
//                 typeof(Int32));
            // workaround enum builder bug
            TypeBuilder enum_builder = m_module_builder.DefineType(
                cts_name,
                (TypeAttributes.Public | TypeAttributes.Sealed),
                typeof(Enum));
            enum_builder.DefineField(
                "value__", typeof(Int32),
                (FieldAttributes.Private | // FIXME mcs uses public here IIRC
                 FieldAttributes.SpecialName |
                 FieldAttributes.RTSpecialName));
            string[] enum_names = xType.getEnumNames();
            int[] enum_values = xType.getEnumValues();
            for (int enum_pos = 0; enum_pos < enum_names.Length; ++ enum_pos)
            {
//                 enum_builder.DefineLiteral(enum_name[enum_pos],
//                                            enum_values[enum_pos]);
                FieldBuilder field_builder =
                    enum_builder.DefineField(enum_names[enum_pos],
                                             enum_builder,
                                             (FieldAttributes.Public |
                                              FieldAttributes.Static |
                                              FieldAttributes.Literal));
                field_builder.SetConstant(enum_values[enum_pos]);
            }

            if (Climaker.g_verbose)
            {
                Console.WriteLine("> emitting enum type {0}", cts_name);
            }
            ret_type = enum_builder.CreateType();
        }
        return ret_type;
    }

    Type get_type(XCompoundTypeDescription xType)
    {
        string uno_name = xType.getName();
        if (xType.getTypeClass() == TypeClass.EXCEPTION)
        {
            switch (uno_name)
            {
            case "com.sun.star.uno.Exception":
                return get_type_Exception();
            case "com.sun.star.uno.RuntimeException":
                return get_type_RuntimeException();
            default:
                break;
            }
        }

        string cts_name = to_cts_name(uno_name);

        // if the struct is an instantiated polymorphic struct then we create
        // the simple struct name
        //
        // For example:
        // void func ([in] PolyStruct<boolean> arg9,
        // PolyStruct<boolean> will be converted to PolyStruct
        polymorphicStructNameToStructName(ref cts_name);

        Type ret_type = get_type(cts_name, false /* no exc */);
        if (ret_type == null)
        {
            XCompoundTypeDescription xBaseType =
                xType.getBaseType() as XCompoundTypeDescription;
            Type base_type = (xBaseType != null
                              ? get_type(xBaseType)
                              : typeof(Object));
            TypeBuilder type_builder = m_module_builder.DefineType(
                cts_name,
                (TypeAttributes.Public |
                 TypeAttributes.BeforeFieldInit |
                 TypeAttributes.AnsiClass),
                base_type);

            // Polymorphic struct, define uno.TypeParametersAttribute
            string[] type_parameters = null;
            XStructTypeDescription xStructTypeDesc =
                xType as XStructTypeDescription;
            if (xStructTypeDesc != null)
            {
                type_parameters = xStructTypeDesc.getTypeParameters();
                if (type_parameters.Length != 0)
                {
                    Type[] typesCtor =
                        new Type[] { Type.GetType("System.String[]") };
                    CustomAttributeBuilder attrBuilder =
                        new CustomAttributeBuilder(
                            typeof(uno.TypeParametersAttribute)
                            .GetConstructor(typesCtor),
                            type_parameters);
                    type_builder.SetCustomAttribute(attrBuilder);
                }
            }

            // FIXME huh? what does this  mean:
            // optional: lookup base type whether generated entry of this session
            struct_entry base_type_entry = null;
            if (base_type != null)
            {
                base_type_entry =
                    m_generated_structs[base_type.FullName] as struct_entry;
            }

            // members
            XTypeDescription[] seq_members = xType.getMemberTypes();
            string[] member_names = xType.getMemberNames();
            Trace.Assert(seq_members.Length == member_names.Length);

            int all_members_length = 0;
            int member_pos;
            int type_param_pos = 0;

            // collect base type; wrong order
            ArrayList base_types_list = new ArrayList(3 /* initial capacity */);
            for (Type base_type_pos = base_type_pos = base_type;
                 ! base_type_pos.Equals(typeof(System.Object));
                 base_type_pos = base_type_pos.BaseType)
            {
                base_types_list.Add(base_type_pos);
                if (base_type_pos.Equals(typeof(System.Exception)))
                {
                    // special Message member
                    all_members_length += 1;
                    break; // don't include System.Exception base classes
                }
                else
                {
                    all_members_length +=
                        base_type_pos.GetFields(BindingFlags.Instance |
                                                BindingFlags.Public |
                                                BindingFlags.DeclaredOnly).Length;
                }
            }

            // create all_members_arrays; right order
            string[] all_member_names =
                new string[all_members_length + seq_members.Length];
            Type[] all_param_types =
                new Type[all_members_length + seq_members.Length];
            member_pos =0;
            for (int pos = base_types_list.Count; pos-- != 0; )
            {
                Type ancestor = (Type) base_types_list[pos];
                if (ancestor.Equals(typeof(System.Exception)))
                {
                    all_member_names[member_pos] = "Message";
                    all_param_types[member_pos] = typeof (System.String);
                    ++member_pos;
                }
                else
                {
                    struct_entry existing_entry =
                        m_generated_structs[ancestor.FullName] as struct_entry;
                    if (existing_entry == null)
                    {
                        // complete type
                        FieldInfo[] fields = ancestor.GetFields(
                            BindingFlags.Instance |
                            BindingFlags.Public |
                            BindingFlags.DeclaredOnly);
                        foreach (FieldInfo field in fields)
                        {
                            all_member_names[member_pos] = field.Name;
                            all_param_types[member_pos] = field.FieldType;
                            ++member_pos;
                        }
                    }
                    else // generated during this session:
                         // members may be incomplete ifaces
                    {
                        for (int i = 0;
                             i < existing_entry.m_member_names.Length; ++i)
                        {
                            all_member_names[member_pos] =
                                existing_entry.m_member_names[i];
                            all_param_types[member_pos] =
                                existing_entry.m_param_types[i];
                            ++member_pos;
                        }
                    }
                }
            }
            Trace.Assert(all_members_length == member_pos);

            // build up entry
            struct_entry entry = new struct_entry();
            entry.m_member_names = new string[seq_members.Length];
            entry.m_param_types = new Type[seq_members.Length];

            // add members
            FieldBuilder[] members = new FieldBuilder[seq_members.Length];

            int curParamIndex = 0; // count the fields which have
                                   // parameterized types
            for (member_pos = 0; member_pos < seq_members.Length; ++member_pos)
            {
                string field_name = member_names[member_pos];
                Type field_type;
                // Special handling of struct parameter types
                bool bParameterizedType = false;
                if (seq_members[member_pos].getTypeClass() == TypeClass.UNKNOWN)
                {
                    bParameterizedType = true;
                    if (type_param_pos < type_parameters.Length)
                    {
                        field_type = typeof(System.Object);
                        type_param_pos++;
                    }
                    else
                    {
                        throw new System.Exception("unexpected member type in " +
                                                   xType.getName());
                    }
                }
                else
                {
                    field_type = get_type(seq_members[member_pos]);
                }
                members[member_pos] = type_builder.DefineField(
                    field_name, field_type, FieldAttributes.Public);

                // parameterized type (polymorphic struct) ?
                if (bParameterizedType && xStructTypeDesc != null)
                {
                    // get the name
                    Trace.Assert(type_parameters.Length > curParamIndex);
                    string sTypeName = type_parameters[curParamIndex++];
                    object[] args = new object[] { sTypeName };
                    // set ParameterizedTypeAttribute
                    Type[] ctorTypes = new Type[] { typeof(System.String) };

                    CustomAttributeBuilder attrBuilder =
                        new CustomAttributeBuilder(
                            typeof(uno.ParameterizedTypeAttribute)
                            .GetConstructor(ctorTypes),
                            args);

                    members[member_pos].SetCustomAttribute(attrBuilder);
                }
                // add to all_members
                all_member_names[all_members_length + member_pos] = field_name;
                all_param_types[all_members_length + member_pos] = field_type;
                // add to entry
                entry.m_member_names[member_pos] = field_name;
                entry.m_param_types[member_pos] = field_type;
            }
            all_members_length += members.Length;

            // default .ctor
            ConstructorBuilder ctor_builder =
                type_builder.DefineConstructor(
                    c_ctor_method_attr, CallingConventions.Standard,
                    new Type[0]);
            ILGenerator code = ctor_builder.GetILGenerator();
            code.Emit(OpCodes.Ldarg_0);
            code.Emit(OpCodes.Call,
                      base_type_entry == null
                      ? base_type.GetConstructor(new Type[0])
                      : base_type_entry.m_default_ctor);
            // default initialize members
            for (member_pos = 0; member_pos < seq_members.Length; ++member_pos)
            {
                FieldInfo field = members[member_pos];
                Type field_type = field.FieldType;
                // default initialize:
                // string, type, enum, sequence, struct, exception, any
                if (field_type.Equals(typeof(System.String)))
                {
                    code.Emit(OpCodes.Ldarg_0);
                    code.Emit(OpCodes.Ldstr, "");
                    code.Emit(OpCodes.Stfld, field);
                }
                else if (field_type.Equals(typeof(System.Type)))
                {
                    code.Emit(OpCodes.Ldarg_0);
                    code.Emit(OpCodes.Ldtoken, typeof(void));
                    code.Emit(OpCodes.Call, m_method_info_Type_GetTypeFromHandle);
                    code.Emit(OpCodes.Stfld, field);
                }
                else if (field_type.IsArray)
                {
                    code.Emit(OpCodes.Ldarg_0);
                    code.Emit(OpCodes.Ldc_I4_0);
                    code.Emit(OpCodes.Newarr, field_type.GetElementType());
                    code.Emit(OpCodes.Stfld, field);
                }
                else if (field_type.IsValueType)
                {
                    if (field_type.FullName.Equals("uno.Any"))
                    {
                        code.Emit(OpCodes.Ldarg_0);
                        code.Emit(OpCodes.Ldsfld, typeof(uno.Any).GetField("VOID"));
                        code.Emit(OpCodes.Stfld, field);
                    }
                }
                else if (field_type.IsClass)
                {
                    /* may be XInterface */
                    if (! field_type.Equals(typeof(System.Object)))
                    {
                        // struct, exception
                        code.Emit(OpCodes.Ldarg_0);
                        code.Emit(OpCodes.Newobj,
                                  field_type.GetConstructor(new Type[0]));
                        code.Emit(OpCodes.Stfld, field);
                    }
                }
            }
            code.Emit(OpCodes.Ret);
            entry.m_default_ctor = ctor_builder;

            // parameterized .ctor including all base members
            ctor_builder = type_builder.DefineConstructor(
                c_ctor_method_attr, CallingConventions.Standard, all_param_types);
            for (member_pos = 0; member_pos < all_members_length; ++member_pos)
            {
                ctor_builder.DefineParameter(
                    member_pos + 1 /* starts with 1 */, ParameterAttributes.In,
                    all_member_names[member_pos]);
            }
            code = ctor_builder.GetILGenerator();
            // call base .ctor
            code.Emit(OpCodes.Ldarg_0); // push this
            int base_members_length = all_members_length - seq_members.Length;
            Type[] param_types = new Type[base_members_length];
            for (member_pos = 0; member_pos < base_members_length; ++member_pos)
            {
                emit_ldarg(code, member_pos + 1);
                param_types[member_pos] = all_param_types[member_pos];
            }
            code.Emit(OpCodes.Call,
                      base_type_entry == null
                      ? base_type.GetConstructor(param_types)
                      : base_type_entry.m_ctor);
            // initialize members
            for (member_pos = 0; member_pos < seq_members.Length; ++member_pos)
            {
                code.Emit(OpCodes.Ldarg_0); // push this
                emit_ldarg(code, member_pos + base_members_length + 1);
                code.Emit(OpCodes.Stfld, members[member_pos]);
            }
            code.Emit(OpCodes.Ret);
            entry.m_ctor = ctor_builder;

            if (Climaker.g_verbose)
            {
                Console.WriteLine("> emitting {0} type {1}",
                                  xType.getTypeClass() == TypeClass.STRUCT
                                  ? "struct"
                                  : "exception",
                                  cts_name);
            }
            // new entry
            m_generated_structs.Add(cts_name, entry);
            ret_type = type_builder.CreateType();
        }
        return ret_type;
    }

    Type get_type(XInterfaceTypeDescription2 xType)
    {
        if (xType.getName() == "com.sun.star.uno.XInterface")
        {
            return typeof(System.Object);
        }

        string cts_name = to_cts_name(xType.getName());
        Type ret_type = get_type(cts_name, false /* no exc */);
        if (ret_type == null)
        {
            TypeBuilder type_builder;

            TypeAttributes attr = (TypeAttributes.Public |
                                   TypeAttributes.Interface |
                                   TypeAttributes.Abstract |
                                   TypeAttributes.AnsiClass);

            ArrayList baseTypes = new ArrayList();
            if (xType.getBaseTypes().Length > 0)
            {
                foreach (XInterfaceTypeDescription2 xIfaceTd
                         in xType.getBaseTypes())
                {
                    if (!(xIfaceTd.getName() == "com.sun.star.uno.XInterface"))
                    {
                        baseTypes.Add(xIfaceTd);
                    }
                }

                Type[] base_interfaces = new Type[baseTypes.Count];

                int index = 0;
                for (IEnumerator iter = baseTypes.GetEnumerator(); iter.MoveNext();
                     index++)
                {
                    base_interfaces[index] =
                        get_type((XInterfaceTypeDescription2) iter.Current);
                }
                type_builder = m_module_builder.DefineType(
                    cts_name, attr, null, base_interfaces);
            }
            else
            {
                Console.WriteLine(
                    "warning: IDL interface {0} is not derived from " +
                    "com.sun.star.uno.XInterface!", xType.getName());
                type_builder = m_module_builder.DefineType(cts_name, attr);
            }

            // insert to be completed
            iface_entry entry = new iface_entry();
            entry.m_xType = xType;
            entry.m_type_builder = type_builder;
            m_incomplete_ifaces.Add(cts_name, entry);

            // type is incomplete
            ret_type = type_builder;
        }
        return ret_type;
    }

    Type get_type(XServiceTypeDescription2 xType)
    {
        if (!xType.isSingleInterfaceBased())
            return null;

        string cts_name = to_cts_name(xType.getName());
        Type ret_type = get_type(cts_name, false /* no exc */);
        if (ret_type != null)
            return ret_type;

        TypeAttributes attr = (TypeAttributes.Public |
                               TypeAttributes.Sealed |
                               TypeAttributes.BeforeFieldInit |
                               TypeAttributes.AnsiClass);

        TypeBuilder type_builder = m_module_builder.DefineType(cts_name, attr);

        // insert to be completed
        service_entry entry = new service_entry();
        entry.m_xType = xType;
        entry.m_type_builder = type_builder;
        m_incomplete_services.Add(cts_name, entry);

        return type_builder;
    }

    Type get_type(XSingletonTypeDescription2 xType)
    {
        if (!xType.isInterfaceBased())
            return null;

        string cts_name = to_cts_name(xType.getName());
        Type ret_type = get_type(cts_name, false /* no exc */);
        if (ret_type != null)
            return ret_type;

        TypeAttributes attr = (TypeAttributes.Public |
                               TypeAttributes.Sealed |
                               TypeAttributes.BeforeFieldInit |
                               TypeAttributes.AnsiClass);

        TypeBuilder type_builder = m_module_builder.DefineType(cts_name, attr);

        // insert to be completed
        singleton_entry entry = new singleton_entry();
        entry.m_xType = xType;
        entry.m_type_builder = type_builder;
        m_incomplete_singletons.Add(cts_name, entry);

        return type_builder;
    }

    Type complete_iface_type(iface_entry entry)
    {
        TypeBuilder type_builder = entry.m_type_builder;
        XInterfaceTypeDescription2 xType = entry.m_xType;

        XTypeDescription[] baseTypes = xType.getBaseTypes();
        if (baseTypes.Length != 0)
        {
            foreach (XTypeDescription td in baseTypes)
            {
                XInterfaceTypeDescription baseType = resolveInterfaceTypedef(td);

                if (!(baseType.getName() == "com.sun.star.uno.XInterface"))
                {
                    string basetype_name = to_cts_name(baseType.getName());
                    iface_entry base_entry =
                        m_incomplete_ifaces[basetype_name] as iface_entry;
                    if (base_entry != null)
                    {
                        // complete uncomplete base type first
                        complete_iface_type(base_entry);
                    }
                }
            }
        }

        foreach (XInterfaceMemberTypeDescription xMember in xType.getMembers())
        {
            MethodBuilder method_builder;

            const MethodAttributes c_method_attr = (MethodAttributes.Public |
                                                    MethodAttributes.Abstract |
                                                    MethodAttributes.Virtual |
                                                    MethodAttributes.NewSlot |
                                                    MethodAttributes.HideBySig /* |
                                                    MethodAttributes.Instance */ );

            if (xMember.getTypeClass() == TypeClass.INTERFACE_METHOD)
            {
                XInterfaceMethodTypeDescription xMethod =
                    (XInterfaceMethodTypeDescription) xMember;

                XMethodParameter[] parameters = xMethod.getParameters();
                Type[] param_types = new Type[parameters.Length];
                // first determine all types
                // Make the first param type the return type
                foreach (XMethodParameter xParam in parameters)
                {
                    Type param_type = get_type(xParam.getType());
                    if (xParam.isOut())
                    {
                        param_type = get_type(param_type.FullName + "&", true);
                    }
                    param_types[xParam.getPosition()] = param_type;
                }

                // create method
//                 if (tb != null)
//                     method_builder = type_builder.DefineMethod(
//                         xMethod.getMemberName(), c_method_attr, tb, param_types);
//                 else
                method_builder = type_builder.DefineMethod(
                    xMethod.getMemberName(), c_method_attr,
                    get_type(xMethod.getReturnType()), param_types);

                // then define parameter infos
                foreach (XMethodParameter xParam in parameters)
                {
                    ParameterAttributes param_flags = 0;
                    if (xParam.isIn())
                        param_flags |= ParameterAttributes.In;
                    if (xParam.isOut())
                        param_flags |= ParameterAttributes.Out;
                    Trace.Assert(param_flags != 0);
                    method_builder.DefineParameter(
                        xParam.getPosition() + 1, /* starts with 1 */
                        param_flags,
                        xParam.getName());
                }
                // Apply attribute TypeParametersAttribute to return
                // value if it is a parameterized Type. Currently only
                // structs can have parameters.
                CustomAttributeBuilder attrBuilder;
                XStructTypeDescription xReturnStruct =
                    xMethod.getReturnType() as XStructTypeDescription;

                if (xReturnStruct != null)
                {
                    XTypeDescription[] type_args =
                        xReturnStruct.getTypeArguments();
                    if (type_args.Length != 0)
                    {
                        // get the ctor of the attribute
                        Type[] ctor = new Type[] { Type.GetType("System.Type[]") };
                        ConstructorInfo info =
                            typeof(uno.TypeParametersAttribute)
                            .GetConstructor(ctor);
                        // Get the arguments for the attribute's ctor
                        Type[] ctsTypes = new Type[type_args.Length];
                        for (int i = 0; i < type_args.Length; ++i)
                            ctsTypes[i] = get_type(type_args[i]);

                        attrBuilder = new CustomAttributeBuilder(
                            info, ctsTypes);

                        method_builder.SetCustomAttribute(attrBuilder);
                    }
                }

                // define UNO exception attribute (exceptions)
                attrBuilder = get_iface_method_exception_attribute(xMethod);
                if (attrBuilder != null)
                    method_builder.SetCustomAttribute(attrBuilder);

                // oneway attribute
                if (xMethod.isOneway())
                {
                    Type[] ctorOneway = new Type[0];
                    object[] args = new object[0];
                    attrBuilder = new CustomAttributeBuilder(
                        typeof(uno.OnewayAttribute).GetConstructor(ctorOneway),
                        args);
                    method_builder.SetCustomAttribute(attrBuilder);
                }
            }
            else // attribute
            {
                Trace.Assert(xMember.getTypeClass() ==
                             TypeClass.INTERFACE_ATTRIBUTE);
                XInterfaceAttributeTypeDescription2 xAttribute =
                    (XInterfaceAttributeTypeDescription2) xMember;

                MethodAttributes c_property_method_attr =
                    c_method_attr | MethodAttributes.SpecialName;

                Type attribute_type = get_type(xAttribute.getType());
                Type[] parameters = new Type[0];

                PropertyBuilder property_builder =
                    type_builder.DefineProperty(
                        xAttribute.getMemberName(), PropertyAttributes.None,
                        attribute_type, parameters);

                // set BoundAttribute, if necessary
                if (xAttribute.isBound())
                {
                    ConstructorInfo ctorBoundAttr =
                        typeof(uno.BoundAttribute).GetConstructor(new Type[0]);
                    CustomAttributeBuilder attrBuilderBound =
                        new CustomAttributeBuilder(ctorBoundAttr, new object[0]);
                    property_builder.SetCustomAttribute(attrBuilderBound);
                }

                // getter
                method_builder = type_builder.DefineMethod(
                    "get_" + xAttribute.getMemberName(),
                    c_property_method_attr, attribute_type, parameters);

                // define UNO exception attribute (exceptions)
                CustomAttributeBuilder attrBuilder =
                    get_exception_attribute(xAttribute.getGetExceptions());
                if (attrBuilder != null)
                    method_builder.SetCustomAttribute(attrBuilder);

                property_builder.SetGetMethod(method_builder);

                if (!xAttribute.isReadOnly())
                {
                    // setter
                    parameters = new Type[] { attribute_type };
                    method_builder =
                        type_builder.DefineMethod(
                            "set_" + xAttribute.getMemberName(),
                            c_property_method_attr, null, parameters);
                    // define parameter info
                    method_builder.DefineParameter(
                        1 /* starts with 1 */, ParameterAttributes.In, "value");
                    // define UNO exception attribute (exceptions)
                    attrBuilder =
                        get_exception_attribute(xAttribute.getSetExceptions());
                    if (attrBuilder != null)
                        method_builder.SetCustomAttribute(attrBuilder);

                    property_builder.SetSetMethod(method_builder);
                }
            }
        }

        // remove from incomplete types map
        string cts_name = type_builder.FullName;
        m_incomplete_ifaces.Remove(cts_name);
        
        if (Climaker.g_verbose)
        {
            Console.WriteLine("> emitting interface type {0}", cts_name);
        }

        return type_builder.CreateType();
    }

    //Examples of generated code
    //     public static XWeak constructor1(XComponentContext ctx)
    //     {
    //         XMultiComponentFactory factory = ctx.getServiceManager();
    //         if (factory == null)
    //             throw new com.sun.star.uno.DeploymentException("bla", null);
    //         return (XWeak) factory.createInstanceWithContext("service_specifier", ctx);
    //     }
    //     public static XWeak constructor2(XComponentContext ctx, int a, int b)
    //     {
    //         XMultiComponentFactory factory = ctx.getServiceManager();
    //         if (factory == null)
    //             throw new com.sun.star.uno.DeploymentException("bla", null);
    //         Any a1;
    //         a1 = new Any( typeof(int), a);
    //         Any a2;
    //         a2 = new Any( typeof(int), b);
    //         Any[] arAny = new Any[2];
    //         arAny[0] = a1;
    //         arAny[1] = a2;
    //         return (XWeak) factory.createInstanceWithArgumentsAndContext("service_specifier", arAny, ctx);
    //     }
    //     public static XWeak constructor3(XComponentContext ctx, params Any[] c)
    //     {
    //         XMultiComponentFactory factory = ctx.getServiceManager();
    //         if (factory == null)
    //             throw new com.sun.star.uno.DeploymentException("bla", null);
    //         return (XWeak) factory.createInstanceWithArgumentsAndContext("service_specifier", c, ctx);
    //     }

    Type complete_service_type(service_entry entry)
    {
        TypeBuilder type_builder = entry.m_type_builder;
        XServiceTypeDescription2 xServiceType = entry.m_xType;

        // Create the private default constructor
        ConstructorBuilder ctor_builder =
            type_builder.DefineConstructor(
                (MethodAttributes.Private |
                 MethodAttributes.HideBySig |
                 MethodAttributes.SpecialName |
                 MethodAttributes.RTSpecialName),
                CallingConventions.Standard, null);

        ILGenerator code = ctor_builder.GetILGenerator();
        code.Emit(OpCodes.Ldarg_0); // push this
        code.Emit(OpCodes.Call,
                  type_builder.BaseType.GetConstructor(new Type[0]));
        code.Emit(OpCodes.Ret);

        // Create the service constructors.
        // optain the interface which makes up this service, it is the
        // return type of the constructor functions
        XInterfaceTypeDescription2 xIfaceType =
            xServiceType.getInterface() as XInterfaceTypeDescription2;
        if (xIfaceType == null)
            xIfaceType = resolveInterfaceTypedef(xServiceType.getInterface());
        Type retType = get_type(xIfaceType);

        // Create the ConstructorInfo for a DeploymentException
        Type typeDeploymentExc =
            get_type("unoidl.com.sun.star.uno.DeploymentException", true);

        Type[] typeCtor = new Type[] { typeof(System.String),
                                       typeof(System.Object) };
        ConstructorInfo ctorDeploymentException =
            typeDeploymentExc.GetConstructor(typeCtor);

        XServiceConstructorDescription[] ctors =
            xServiceType.getConstructors();

        Type type_uno_exception =
            get_type("unoidl.com.sun.star.uno.Exception", true);

        for (int i = ctors.Length - 1; i >= 0; --i)
        {
            bool bParameterArray = false;
            Type typeAny = typeof(uno.Any);
            XServiceConstructorDescription ctorDes = ctors[i];
            // obtain the parameter types
            XParameter[] xParams = ctorDes.getParameters();
            Type[] typeParameters = new Type[xParams.Length + 1];
            typeParameters[0] =
                get_type("unoidl.com.sun.star.uno.XComponentContext", true);
            for (int iparam = 0; iparam != xParams.Length; ++iparam)
            {
                if (xParams[iparam].isRestParameter())
                    typeParameters[iparam + 1] = typeof(uno.Any[]);
                else
                    typeParameters[iparam + 1] =
                        get_type(xParams[iparam].getType());
            }

            // define method
            string ctorName;
            if (ctorDes.isDefaultConstructor())
                ctorName = "create";
            else
                ctorName = ctorDes.getName();
            MethodBuilder method_builder = type_builder.DefineMethod(
                ctorName,
                (MethodAttributes.Public |
                 MethodAttributes.HideBySig |
                 MethodAttributes.Static),
                retType,
                typeParameters);

            // define UNO exception attribute (exceptions)
            CustomAttributeBuilder attrBuilder =
                get_service_exception_attribute(ctorDes);
            if (attrBuilder != null)
                method_builder.SetCustomAttribute(attrBuilder);

            // define parameter attributes (paramarray), names etc.
            // The first parameter is the XComponentContext, which cannot be
            // obtained from reflection.
            // The context is not part of the idl description
            /* ParameterBuilder param_builder = */
            method_builder.DefineParameter(
                1, ParameterAttributes.In, "the_context");

            ParameterBuilder[] parameterBuilders =
                new ParameterBuilder[xParams.Length];
            for (int iparam = 0; iparam != xParams.Length + 1; ++iparam)
            {
                parameterBuilders[iparam] = method_builder.DefineParameter(
                    iparam + 2, ParameterAttributes.In,
                    xParams[iparam].getName());

                if (xParams[iparam].isRestParameter())
                {
                    bParameterArray = true;
                    // set the ParameterArrayAttribute
                    ConstructorInfo ctor_info =
                        typeof(System.ParamArrayAttribute).GetConstructor(
                            new Type[0]);
                    CustomAttributeBuilder attr_builder =
                        new CustomAttributeBuilder(ctor_info, new object[0]);
                    parameterBuilders[iparam].SetCustomAttribute(attr_builder);
                    break;
                }
            }

            ILGenerator ilGen = method_builder.GetILGenerator();

            // Define locals
            // XMultiComponentFactory
            LocalBuilder local_factory = ilGen.DeclareLocal(
                get_type("unoidl.com.sun.star.lang.XMultiComponentFactory",
                         true));

            // The return type
            LocalBuilder local_return_val = ilGen.DeclareLocal(retType);

            // Obtain the XMultiComponentFactory and throw an exception if we
            // do not get one
            ilGen.Emit(OpCodes.Ldarg_0);

            MethodInfo methodGetServiceManager = get_type(
                "unoidl.com.sun.star.uno.XComponentContext", true)
                .GetMethod("getServiceManager");
            ilGen.Emit(OpCodes.Callvirt, methodGetServiceManager);
            ilGen.Emit(OpCodes.Stloc, local_factory);
            ilGen.Emit(OpCodes.Ldloc, local_factory);
            Label label1 = ilGen.DefineLabel();
            ilGen.Emit(OpCodes.Brtrue, label1);
            ilGen.Emit(OpCodes.Ldstr,
                       String.Format("The service unoidl.{0} could not be created. The context failed to supply the service manager.",
                                     xServiceType.getName()));
            ilGen.Emit(OpCodes.Ldarg_0);
            ilGen.Emit(OpCodes.Newobj, ctorDeploymentException);
            ilGen.Emit(OpCodes.Throw);
            ilGen.MarkLabel(label1);
            
            // We create a try/catch around the createInstanceWithContext,
            // etc. functions
            
            // There are 3 cases

            // 1. function do not specify exceptions. Then
            //    RuntimeExceptions are retrhown and other exceptions
            //    produce a DeploymentException.

            // 2. function specify Exception. Then all exceptions fly
            //    through

            // 3. function specifies exceptions but no Exception. Then
            //    these are rethrown and other exceptions, except
            //    RuntimeException, produce a deployment exception.

            // In case there are no parameters we call
            // XMultiComponentFactory.createInstanceWithContext

            ArrayList exceptionTypes =
                get_service_ctor_method_exceptions_reduced(
                    ctorDes.getExceptions());
            if (! exceptionTypes.Contains(type_uno_exception))
            {
                ilGen.BeginExceptionBlock();
            }
            if (xParams.Length == 0)
            {
                ilGen.Emit(OpCodes.Ldloc, local_factory);
                ilGen.Emit(OpCodes.Ldstr, to_cts_name(xServiceType.getName()));
                ilGen.Emit(OpCodes.Ldarg_0);

                MethodInfo methodCIWAAC = 
                    local_factory.LocalType.GetMethod(
                        "createInstanceWithArgumentsAndContext");
                ilGen.Emit(OpCodes.Callvirt, methodCIWAAC);
            }
            else if (bParameterArray)
            {
                // Service constructor with parameter array
                ilGen.Emit(OpCodes.Ldloc, local_factory);
                ilGen.Emit(OpCodes.Ldstr, to_cts_name(xServiceType.getName()));
                ilGen.Emit(OpCodes.Ldarg_1);
                ilGen.Emit(OpCodes.Ldarg_0);
                MethodInfo methodCIWAAC =
                    local_factory.LocalType.GetMethod(
                        "createInstanceWithArgumentsAndContext");
                ilGen.Emit(OpCodes.Callvirt, methodCIWAAC);
            }
            else
            {
                // Any param1, Any param2, etc.
                // For each parameter, except the component context, and
                // the parameter array an Any is created.
                LocalBuilder[] localAnys = new LocalBuilder[xParams.Length];

                for (int iparam = 0; iparam < xParams.Length; ++iparam)
                {
                    localAnys[iparam] = ilGen.DeclareLocal(typeAny);
                }

                // Any[]. This array is filled with the created Anys
                // which contain the parameters and the values
                // contained in the parameter array
                LocalBuilder local_anyParams =
                    ilGen.DeclareLocal(typeof(uno.Any[]));

                // Create the Any for every argument, except for the
                // parameter array. localAnys contains the LocalBuilder for
                // all these parameters. We call the ctor Any(Type, Object)
                Type[] typesCtorAny = { typeof(System.Type),
                                        typeof(System.Object) };
                ConstructorInfo ctorAny =
                    typeAny.GetConstructor(typesCtorAny);
                for (int j = 0; j < localAnys.Length; ++j)
                {
                    ilGen.Emit(OpCodes.Ldloca, localAnys[j]);
                    ilGen.Emit(OpCodes.Ldtoken, typeParameters[j+1]);

                    Type[] typeParams = { typeof(System.RuntimeTypeHandle) };

                    ilGen.Emit(OpCodes.Call,
                               typeof(System.Type).GetMethod(
                                   "GetTypeFromHandle", typeParams));
                    ilGen.Emit(OpCodes.Ldarg, j + 1);
                    // if the parameter is a value type then we need
                    // to box it, because the Any ctor takes an Object
                    if (typeParameters[j+1].IsValueType)
                        ilGen.Emit(OpCodes.Box, typeParameters[j+1]);
                    ilGen.Emit(OpCodes.Call, ctorAny);
                }

                // Create the Any[] that is passed to the
                // createInstanceWithContext[AndArguments] function
                ilGen.Emit(OpCodes.Ldc_I4, localAnys.Length);
                ilGen.Emit(OpCodes.Newarr, typeAny);
                ilGen.Emit(OpCodes.Stloc, local_anyParams);

                // Assign all anys created from the parameters array
                // to the Any[]
                for (int j = 0; j < localAnys.Length; ++j)
                {
                    ilGen.Emit(OpCodes.Ldloc, local_anyParams);
                    ilGen.Emit(OpCodes.Ldc_I4, j);
                    ilGen.Emit(OpCodes.Ldelema, typeAny);
                    ilGen.Emit(OpCodes.Ldloc, localAnys[j]);
                    ilGen.Emit(OpCodes.Stobj, typeAny);
                }

                // call createInstanceWithContextAndArguments
                ilGen.Emit(OpCodes.Ldloc, local_factory);
                ilGen.Emit(OpCodes.Ldstr, to_cts_name(xServiceType.getName()));
                ilGen.Emit(OpCodes.Ldloc, local_anyParams);
                ilGen.Emit(OpCodes.Ldarg_0);
                MethodInfo methodCIWAAC =
                    local_factory.LocalType.GetMethod(
                        "createInstanceWithArgumentsAndContext");
                ilGen.Emit(OpCodes.Callvirt, methodCIWAAC);
            }

            // cast the object returned by the functions
            // createInstanceWithContext or
            // createInstanceWithArgumentsAndContext to the interface type
            ilGen.Emit(OpCodes.Castclass, retType);
            ilGen.Emit(OpCodes.Stloc, local_return_val);

            // catch exceptions thrown by createInstanceWithArgumentsAndContext
            // and createInstanceWithContext
            if (!exceptionTypes.Contains(type_uno_exception))
            {
                // catch (unoidl.com.sun.star.uno.RuntimeException) {throw;}
                ilGen.BeginCatchBlock(
                    get_type(
                        "unoidl.com.sun.star.uno.RuntimeException", true));
                ilGen.Emit(OpCodes.Pop);
                ilGen.Emit(OpCodes.Rethrow);

                // catch and rethrow all other defined Exceptions
                foreach (Type excType in exceptionTypes)
                {
                    if (excType.IsInstanceOfType(
                            get_type(
                                "unoidl.com.sun.star.uno.RuntimeException",
                                true)))
                    {
                        // we have a catch for RuntimeException already defined
                        continue;
                    }

                    // catch Exception and rethrow
                    ilGen.BeginCatchBlock(excType);
                    ilGen.Emit(OpCodes.Pop);
                    ilGen.Emit(OpCodes.Rethrow);
                }

                // catch (unoidl.com.sun.star.uno.Exception)
                // { throw DeploymentException... }
                ilGen.BeginCatchBlock(type_uno_exception);

                // define the local variable that keeps the exception
                LocalBuilder local_exception = ilGen.DeclareLocal(
                    type_uno_exception);

                // Store the exception
                ilGen.Emit(OpCodes.Stloc, local_exception);

                // prepare the construction of the exception
                ilGen.Emit(OpCodes.Ldstr,
                           "The context (com.sun.star.uno.XComponentContext)" +
                           "failed to supply the service " +
                           to_cts_name(xServiceType.getName()) +
                           ": ");

                // add to the string the Exception.Message
                ilGen.Emit(OpCodes.Ldloc, local_exception);
                ilGen.Emit(OpCodes.Callvirt,
                           type_uno_exception.GetProperty("Message")
                           .GetGetMethod());
                Type[] concatParams = new Type[] { typeof(System.String),
                                                   typeof(System.String) };
                ilGen.Emit(OpCodes.Call,
                           typeof(System.String).GetMethod("Concat",
                                                           concatParams));
                // load context argument
                ilGen.Emit(OpCodes.Ldarg_0);
                ilGen.Emit(OpCodes.Newobj, ctorDeploymentException);
                ilGen.Emit(OpCodes.Throw); // Exception(typeDeploymentExc);

                ilGen.EndExceptionBlock();
            }

            // Check if the service instance was create and throw an
            // exception if not.
            Label label_service_created = ilGen.DefineLabel();
            ilGen.Emit(OpCodes.Ldloc, local_return_val);
            ilGen.Emit(OpCodes.Brtrue_S, label_service_created);

            ilGen.Emit(OpCodes.Ldstr,
                       "The context (com.sun.star.uno.XComponentContext)" +
                       "failed to supply the service unoidl." +
                       to_cts_name(xServiceType.getName()) +
                       ".");
            ilGen.Emit(OpCodes.Ldarg_0);
            ilGen.Emit(OpCodes.Newobj, ctorDeploymentException);
            ilGen.Emit(OpCodes.Throw); // Exception(typeDeploymentExc);

            ilGen.MarkLabel(label_service_created);
            ilGen.Emit(OpCodes.Ldloc, local_return_val);
            ilGen.Emit(OpCodes.Ret);
        }

        // remove from incomplete types map
        m_incomplete_services.Remove(type_builder.FullName);
        
        if (Climaker.g_verbose)
        {
            Console.WriteLine("> emitting service type {0}",
                              type_builder.FullName);
        }

        return type_builder.CreateType();
    }

    CustomAttributeBuilder get_service_exception_attribute(
        XServiceConstructorDescription ctorDes)
    {
        return get_exception_attribute(ctorDes.getExceptions());
    }

    CustomAttributeBuilder get_iface_method_exception_attribute(
        XInterfaceMethodTypeDescription xMethod)
    {
        XTypeDescription[] TDs = xMethod.getExceptions();
        XCompoundTypeDescription[] CTDs =
            new XCompoundTypeDescription[TDs.Length];
        
        for (int i = 0; i < TDs.Length; ++i)
            CTDs[i] = (XCompoundTypeDescription) TDs[i];

        return get_exception_attribute(CTDs);
    }

    CustomAttributeBuilder get_exception_attribute(
        XCompoundTypeDescription[] exceptionsTd)
    {
        CustomAttributeBuilder attr_builder = null;

        Type[] typesCtor = new Type[] { Type.GetType("System.Type[]") };
        ConstructorInfo ctor_ExceptionAttribute =
            typeof(uno.ExceptionAttribute).GetConstructor(typesCtor);

        if (exceptionsTd.Length != 0) // opt
        {
            Type[] exception_types = new Type[exceptionsTd.Length];
            for (int exc_pos = 0; exc_pos < exceptionsTd.Length; ++exc_pos)
            {
                XCompoundTypeDescription xExc = exceptionsTd[exc_pos];
                exception_types[exc_pos] = get_type(xExc);
            }

            object[] args = new object[] { exception_types };
            attr_builder = new CustomAttributeBuilder(
                ctor_ExceptionAttribute, args);
        }
        return attr_builder;
    }
        
    Type complete_singleton_type(singleton_entry entry)
    {
        TypeBuilder type_builder = entry.m_type_builder;
        XSingletonTypeDescription2 xSingletonType = entry.m_xType;
        string sSingletonName = to_cts_name(xSingletonType.getName());

        // Create the private default constructor
        ConstructorBuilder ctor_builder =
            type_builder.DefineConstructor((MethodAttributes.Private |
                                            MethodAttributes.HideBySig |
                                            MethodAttributes.SpecialName |
                                            MethodAttributes.RTSpecialName),
                                           CallingConventions.Standard, null);

        ILGenerator ilGen = ctor_builder.GetILGenerator();
        ilGen.Emit(OpCodes.Ldarg_0); // push this
        ilGen.Emit(OpCodes.Call,
                   type_builder.BaseType.GetConstructor(new Type[0]));
        ilGen.Emit(OpCodes.Ret);

        // obtain the interface which makes up this service, it is the return
        // type of the constructor functions
        XInterfaceTypeDescription2 xIfaceType =
            xSingletonType.getInterface() as XInterfaceTypeDescription2;

        if (xIfaceType == null)
            xIfaceType = resolveInterfaceTypedef(
                xSingletonType.getInterface());
        Type retType = get_type(xIfaceType);

        // Create the ConstructorInfo for a DeploymentException
        Type typeDeploymentExc =
            get_type("unoidl.com.sun.star.uno.DeploymentException", true);

        Type[] typeCtor = new Type[] { typeof(System.String),
                                       typeof(System.Object) };
        ConstructorInfo ctorDeploymentException =
            typeDeploymentExc.GetConstructor(typeCtor);

        // define method
        Type[] typeParameters =
            new Type[] { get_type(
                             "unoidl.com.sun.star.uno.XComponentContext",
                             true) };
        MethodBuilder method_builder = type_builder.DefineMethod(
            "get",
            (MethodAttributes.Public |
             MethodAttributes.HideBySig |
             MethodAttributes.Static),
            retType,
            typeParameters);

//    method_builder.SetCustomAttribute(get_service_ctor_method_attribute(ctorDes));

        // The first parameter is the XComponentContext, which cannot be
        // obtained using reflection.
        // The context is not part of the idl description
        /* ParameterBuilder param_builder = */ method_builder.DefineParameter(
            1, ParameterAttributes.In, "the_context");

        ilGen = method_builder.GetILGenerator();
        // Define locals
        // Any, returned by XComponentContext.getValueByName
        LocalBuilder local_any = ilGen.DeclareLocal(typeof(uno.Any));

        // Call XContext.getValueByName
        ilGen.Emit(OpCodes.Ldarg_0);
        // build the singleton name: /singleton/unoidl.com.sun.star.XXX
        ilGen.Emit(OpCodes.Ldstr, "/singletons/" + sSingletonName);

        MethodInfo methodGetValueByName =
            get_type("unoidl.com.sun.star.uno.XComponentContext",
                     true).GetMethod("getValueByName");
        ilGen.Emit(OpCodes.Callvirt, methodGetValueByName);
        ilGen.Emit(OpCodes.Stloc_0);

        // Does the returned Any contain a value?
        ilGen.Emit(OpCodes.Ldloca_S, local_any);
        MethodInfo methodHasValue = typeof(uno.Any).GetMethod("hasValue");
        ilGen.Emit(OpCodes.Call, methodHasValue);

        // If not, then throw a DeploymentException
        Label label_singleton_exists = ilGen.DefineLabel();
        ilGen.Emit(OpCodes.Brtrue_S, label_singleton_exists);
        ilGen.Emit(OpCodes.Ldstr,
                   "Component context fails to supply singleton " +
                   sSingletonName + " of type " + retType.FullName + ".");
        ilGen.Emit(OpCodes.Ldarg_0);
        ilGen.Emit(OpCodes.Newobj, ctorDeploymentException);
        ilGen.Emit(OpCodes.Throw);
        ilGen.MarkLabel(label_singleton_exists);

        // Cast the singleton contained in the Any to the expected
        // interface and return it.
        ilGen.Emit(OpCodes.Ldloca_S, local_any);
        ilGen.Emit(OpCodes.Call,
                   typeof(uno.Any).GetProperty("Value").GetGetMethod());
        ilGen.Emit(OpCodes.Castclass, retType);
        ilGen.Emit(OpCodes.Ret);

        // remove from incomplete singletons map
        m_incomplete_singletons.Remove(type_builder.FullName);
    
        if (Climaker.g_verbose)
        {
            Console.WriteLine("> emitting singleton type {0}",
                              type_builder.FullName);
        }

        return type_builder.CreateType();
    }

    public Type get_type(XTypeDescription xType)
    {
        switch (xType.getTypeClass())
        {
        case TypeClass.VOID:
            return typeof(void);
        case TypeClass.CHAR:
            return typeof(System.Char);
        case TypeClass.BOOLEAN:
            return typeof(System.Boolean);
        case TypeClass.BYTE:
            return typeof(System.Byte);
        case TypeClass.SHORT:
            return typeof(System.Int16);
        case TypeClass.UNSIGNED_SHORT:
            return typeof(System.UInt16);
        case TypeClass.LONG:
            return typeof(System.Int32);
        case TypeClass.UNSIGNED_LONG:
            return typeof(System.UInt32);
        case TypeClass.HYPER:
            return typeof(System.Int64);
        case TypeClass.UNSIGNED_HYPER:
            return typeof(System.UInt64);
        case TypeClass.FLOAT:
            return typeof(System.Single);
        case TypeClass.DOUBLE:
            return typeof(System.Double);
        case TypeClass.STRING:
            return typeof(System.String);
        case TypeClass.TYPE:
            return typeof(System.Type);
        case TypeClass.ANY:
            return typeof(uno.Any);
        case TypeClass.ENUM:
            return get_type((XEnumTypeDescription) xType);
        case TypeClass.TYPEDEF:
            // unwind typedefs
            return get_type(((XIndirectTypeDescription)
                             xType).getReferencedType());
        case TypeClass.STRUCT:
        case TypeClass.EXCEPTION:
            return get_type((XCompoundTypeDescription) xType);
        case TypeClass.SEQUENCE:
        {
            Type element_type = get_type(((XIndirectTypeDescription)
                                          xType).getReferencedType());
            return get_type(element_type.FullName + "[]", true);
        }
        case TypeClass.INTERFACE:
            return get_type((XInterfaceTypeDescription2) xType);
        case TypeClass.CONSTANT:
            return get_type((XConstantTypeDescription) xType);
        case TypeClass.CONSTANTS:
            return get_type((XConstantsTypeDescription) xType);
        case TypeClass.SERVICE:
            return get_type((XServiceTypeDescription2) xType);
        case TypeClass.SINGLETON:
            return get_type((XSingletonTypeDescription2) xType);
        case TypeClass.MODULE:
            // ignore these
            return null;
        default:
            throw new System.Exception("unexpected type " + xType.getName());
        }
    }

    // must be called to finish up uncompleted types
    public void Dispose()
    {
        for (IDictionaryEnumerator enumerator =
                 m_incomplete_ifaces.GetEnumerator();
             enumerator.MoveNext(); )
        {
            complete_iface_type(enumerator.Value as iface_entry);
        }

        for (IDictionaryEnumerator enumerator =
                 m_incomplete_services.GetEnumerator();
             enumerator.MoveNext(); )
        {
            complete_service_type(enumerator.Value as service_entry);
        }

        for (IDictionaryEnumerator enumerator =
                 m_incomplete_singletons.GetEnumerator();
             enumerator.MoveNext(); )
        {
            complete_singleton_type(enumerator.Value as singleton_entry);
        }             
    }

    public TypeEmitter(ModuleBuilder module_builder, Assembly[] extra_assemblies)
    {
        m_module_builder = module_builder;
        m_extra_assemblies = extra_assemblies;
        m_method_info_Type_GetTypeFromHandle = null;
        m_type_Exception = null;
        m_type_RuntimeException = null;
        m_incomplete_ifaces = new Hashtable();
        m_incomplete_services = new Hashtable();
        m_incomplete_singletons = new Hashtable();
        m_generated_structs = new Hashtable();

        Type[] param_types = new Type[] { typeof(System.RuntimeTypeHandle) };
        m_method_info_Type_GetTypeFromHandle =
            typeof(System.Type).GetMethod("GetTypeFromHandle", param_types);
    }

    /* Creates .System.Type object for UNO exceptions. The UNO
       exceptions are obtained by
       com.sun.star.reflection.XServiceConstructorDescription.getExceptions
       In a first step the respective CLI types are created. Then it
       is examined if a Type represents a super class of another
       class. If so the Type of the derived class is discarded. For
       example there are a uno RuntimeException and a
       DeploymentException which inherits RuntimeException. Then only
       the cli Type of the RuntimeException is returned.  The purpose
       of this function is to provide exceptions for which catch
       blocks are generated in the service constructor code.

       The result is always an instance of ArrayList, even if the
       sequence argument does not contain any elements.
    */
    ArrayList get_service_ctor_method_exceptions_reduced(
        XCompoundTypeDescription[] exceptionsTd)
    {
        if (exceptionsTd.Length == 0)
            return new ArrayList();

        ArrayList types = new ArrayList();
        foreach (XCompoundTypeDescription xCompTd in exceptionsTd)
        {
            types.Add(get_type(to_cts_name(xCompTd.getName()), true));
        }

        int start = 0;
        while (true)
        {
            bool bRemove = false;
            for (int i = start; i < types.Count; ++i)
            {
                Type t = (Type) types[i];
                for (int j = 0; j < types.Count; ++j)
                {
                    if (t.IsSubclassOf((Type) types[j]))
                    {
                        // FIXME Wow, we're using an ArrayList and calling
                        // RemoveAt all the time?
                        types.RemoveAt(i);
                        bRemove = true;
                        break;
                    }
                }
                if (bRemove)
                    break;
                start++;
            }

            if (bRemove == false)
                break;
        }
        return types;
    }

    XInterfaceTypeDescription2 resolveInterfaceTypedef(
        XTypeDescription type)
    {
        XInterfaceTypeDescription2 xIfaceTd = type as XInterfaceTypeDescription2;

        if (xIfaceTd != null)
            return xIfaceTd;

        XIndirectTypeDescription xIndTd = type as XIndirectTypeDescription;
        if (xIndTd == null)
            throw new System.Exception(
                "resolveInterfaceTypedef was called with an invalid argument");

        return resolveInterfaceTypedef(xIndTd.getReferencedType());
    }
}

