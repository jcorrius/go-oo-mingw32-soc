/*************************************************************************
 *
 *  $RCSfile: Climaker.cs,v $
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
using System.IO;
using System.Reflection;
using System.Reflection.Emit;

using uno.util;
using unoidl.com.sun.star.container;
using unoidl.com.sun.star.lang;
using unoidl.com.sun.star.reflection;
using unoidl.com.sun.star.registry;
using unoidl.com.sun.star.uno;

struct OptionInfo
{
    public string m_name;
    public char m_short_option;
    public bool m_has_argument;

    private OptionInfo(string name, char short_option, bool has_argument)
    {
        m_name = name;
        m_short_option = short_option;
        m_has_argument = has_argument;
    }

    private static OptionInfo[] s_option_infos = new OptionInfo[] {
        new OptionInfo("out", 'O', true),
        new OptionInfo("types", 'T', true),
        new OptionInfo("extra", 'X', true),
        new OptionInfo("reference", 'r', true),
        new OptionInfo("keyfile", 'k', true),
        new OptionInfo("delaySign", 'd', true),
        new OptionInfo("assembly-version", '\0', true),
        new OptionInfo("assembly-description", '\0', true),
        new OptionInfo("assembly-product", '\0', true),
        new OptionInfo("assembly-company", '\0', true),
        new OptionInfo("assembly-copyright", '\0', true),
        new OptionInfo("assembly-trademark", '\0', true),
        new OptionInfo("verbose", 'v', false),
        new OptionInfo("help", 'h', false)
    };

    public bool is_valid()
    {
        return (m_name != null && m_short_option != '\0');
    }

    public static OptionInfo get_option_info(string opt, char copt)
    {
        foreach (OptionInfo option_info in s_option_infos)
        {
            if (opt.Length > 0)
            {
                if (opt == option_info.m_name &&
                    (copt == '\0' || copt == option_info.m_short_option))
                {
                    return option_info;
                }
            } else
            {
                Trace.Assert(copt != '\0');
                if (copt == option_info.m_short_option)
                {
                    return option_info;
                }
            }
        }
        Trace.Assert(false);
        return new OptionInfo();
    }

    public static OptionInfo get_option_info(string opt)
    {
        return get_option_info(opt, '\0');
    }

    public static bool is_option(OptionInfo option_info, IEnumerator argIter)
    {
        string arg;
        try
        {
            arg = (string) argIter.Current;
        } catch (InvalidOperationException e)
        {
            return false;
        }

        if (arg.Length < 2 || arg[0] != '-')
            return false;

        if (arg.Length == 2 && arg[1] == option_info.m_short_option)
        {
            Trace.WriteLine("Identified option '" +
                            option_info.m_short_option +
                            "'");
            return true;
        }

        if (arg[1] == '-' && arg.Substring(2) == option_info.m_name)
        {
            Trace.WriteLine("Identified option '" +
                            option_info.m_name +
                            "'");
            return true;
        }

        return false;
    }

    public static bool read_option(out bool flag, OptionInfo option_info,
                                   IEnumerator argIter)
    {
        flag = is_option(option_info, argIter);
        return flag;
    }

    public static bool read_argument(out string pValue, OptionInfo option_info,
                                     IEnumerator argIter)
    {
        if (is_option(option_info, argIter))
        {
            if (argIter.MoveNext())
            {
                pValue = (string) argIter.Current;
                Trace.Write("argument value: ");
                Trace.WriteLine(pValue);
                return true;
            }
        }
        pValue = null;
        return false;
    }
}

class SharedLibraryLoader
{
    private SharedLibraryLoader() {}

    public static XSingleComponentFactory loadSharedLibComponentFactory(
        string libName, string path, string implName,
        XMultiServiceFactory xMgr, XRegistryKey xKey)
    {
        throw new NotImplementedException();
    }
}

public class Climaker
{
    public static bool g_verbose = false;

    static string s_usingText = @"
using: climaker <switches> [registry-file-1 registry-file-2 ...]

switches:
 -O, --out <output-file>       output assembly file;
                               defaults to cli_unotypes.dll if more than one
                               registry-file is given, else <registry-file>.dll
 -T, --types                   types to be generated (if none is given,
   <type1[;type2;...]>         then all types of given registries are emitted
 -X, --extra <rdb-file>        additional rdb to saturate referenced types in
                               given registry file(s); these types will not be
                               emitted into the output assembly file
 -r, --reference               reference metadata from assembly file
   <assembly-file>
 -k, --keyfile                 keyfile needed for strong name
 --assembly-version <version>  sets assembly version
 --assembly-description <text> sets assembly description text
 --assembly-product <text>     sets assembly product name
 --assembly-company <text>     sets assembly company
 --assembly-copyright <text>   sets assembly copyright
 --assembly-trademark <text>   sets assembly trademark
 -v, --verbose                 verbose output to stdout
 -h, --help                    this message

example: climaker --out cli_mytypes.dll \
                  --reference cli_types.dll \
                  --extra types.rdb \
                  mytypes.rdb
";

    static string path_make_absolute_file_url(string path)
    {
        Uri file_uri = new Uri(new Uri(new Uri("file:///"), 
                                       Environment.CurrentDirectory),
                               path);
        return file_uri.ToString();        
    }

    static XSimpleRegistry open_registries(ArrayList registries,
                                           XComponentContext xContext)
    {
        throw new NotImplementedException();
    }

    public static int Main(string[] args)
    {
        if (args.Length == 0)
        {
            Console.Write(s_usingText);
            return 0;
        }

        int ret = 0;
        XComponentContext xContext = null;

//         try
        {
            OptionInfo info_help =
                OptionInfo.get_option_info("help");
            OptionInfo info_verbose =
                OptionInfo.get_option_info("verbose");
            OptionInfo info_out =
                OptionInfo.get_option_info("out");
            OptionInfo info_types =
                OptionInfo.get_option_info("types");
            OptionInfo info_reference =
                OptionInfo.get_option_info("reference");
            OptionInfo info_extra =
                OptionInfo.get_option_info("extra");
            OptionInfo info_keyfile =
                OptionInfo.get_option_info("keyfile");
            OptionInfo info_delaySign =
                OptionInfo.get_option_info("delaySign");
            OptionInfo info_version =
                OptionInfo.get_option_info("assembly-version");
            OptionInfo info_product =
                OptionInfo.get_option_info("assembly-product");
            OptionInfo info_description =
                OptionInfo.get_option_info("assembly-description");
            OptionInfo info_company =
                OptionInfo.get_option_info("assembly-company");
            OptionInfo info_copyright =
                OptionInfo.get_option_info("assembly-copyright");
            OptionInfo info_trademark =
                OptionInfo.get_option_info("assembly-trademark");
            
            string output = "";
            ArrayList mandatory_registries = new ArrayList();
            ArrayList extra_registries = new ArrayList();
            ArrayList extra_assemblies = new ArrayList();
            ArrayList explicit_types = new ArrayList();
            string version = "";
            string product = "";
            string description = "";
            string company = "";
            string copyright = "";
            string trademark = "";
            string keyfile = "";
            string delaySign = "";

            string cmd_arg;
            for (IEnumerator argIter = args.GetEnumerator(); argIter.MoveNext(); )
            {
                // options
                if (OptionInfo.is_option(info_help, argIter))
                {
                    Console.Write(s_usingText);
                    return 0;
                } else if (OptionInfo.read_argument(out cmd_arg,
                                                    info_types, argIter))
                {
                    explicit_types.AddRange(cmd_arg.Split(';'));
                } else if (OptionInfo.read_argument(out cmd_arg,
                                                    info_extra, argIter))
                {
                    extra_registries.Add(path_make_absolute_file_url(cmd_arg));
                } else if (OptionInfo.read_argument(out cmd_arg,
                                                    info_reference, argIter))
                {
                    extra_assemblies.Add(path_make_absolute_file_url(cmd_arg));
                } else if (!OptionInfo.read_option(out g_verbose,
                                                   info_verbose, argIter) &&
                           !OptionInfo.read_argument(out output,
                                                     info_out, argIter) &&
                           !OptionInfo.read_argument(out version,
                                                     info_version, argIter) &&
                           !OptionInfo.read_argument(out description,
                                                     info_description, argIter) &&
                           !OptionInfo.read_argument(out product,
                                                     info_product, argIter) &&
                           !OptionInfo.read_argument(out company,
                                                     info_company, argIter) &&
                           !OptionInfo.read_argument(out copyright,
                                                     info_copyright, argIter) &&
                           !OptionInfo.read_argument(out trademark,
                                                     info_trademark, argIter) &&
                           !OptionInfo.read_argument(out keyfile,
                                                     info_keyfile, argIter) &&
                           !OptionInfo.read_argument(out delaySign,
                                                     info_delaySign, argIter))
                {
                    cmd_arg = ((string) argIter.Current).Trim();
                    if (cmd_arg.Length > 0)
                    {
                        if (cmd_arg[0] == '-') // is option
                        {
                            OptionInfo option_info = new OptionInfo();
                            if (cmd_arg.Length > 2 && cmd_arg[1] == '-')
                            {
                                // long option
                                option_info = OptionInfo.get_option_info(
                                    cmd_arg.Substring(2), '\0');
                            } else if (cmd_arg.Length == 2 && cmd_arg[1] != '-')
                            {
                                // short option
                                option_info = OptionInfo.get_option_info(
                                    "", cmd_arg[1]);
                            }
                            if (!option_info.is_valid())
                            {
                                throw new System.Exception(
                                    String.Format("unknown option {0}!  "+
                                                  "Use climaker --help " +
                                                  "to print all options.",
                                                  cmd_arg));
                                                  
                            } else
                            {
                                Trace.Assert(false, "unhandled valid option?!");
                                if (option_info.m_has_argument)
                                    argIter.MoveNext();
                            }
                        } else
                        {
                            mandatory_registries.Add(
                                path_make_absolute_file_url(cmd_arg));
                        }
                    }
                }                    
            }

            // bootstrap uno
            xContext = Bootstrap.bootstrap_InitialComponentContext();
            XHierarchicalNameAccess xTDmgr =
                (XHierarchicalNameAccess) xContext.getValueByName(
                    "/singletons/com.sun.star.reflection." +
                    "theTypeDescriptionManager").Value;

            // get rdb tdprovided factory
            XSingleComponentFactory xTDprov_factory =
                Bootstrap.loadRegistryTypeDescriptionProvider(xContext);

            if (xTDprov_factory == null)
            {
                throw new System.Exception(
                    "cannot get registry typedescription provider: " +
                    "regtypeprov.uno.so!"); // FIXME actually it's .uno.so or .uno.dll
            }

            // create registry td provider for mandatory registry files
            uno.Any arg = new uno.Any(typeof(XSimpleRegistry),
                                      open_registries(mandatory_registries,
                                                      xContext));
            object xTD_provider =
                xTDprov_factory.createInstanceWithArgumentsAndContext(
                    new uno.Any[] { arg }, xContext);
            // insert provider to tdmgr
            XSet xSet = (XSet) xTDmgr;
            uno.Any provider = new uno.Any (typeof(System.Object), xTD_provider);
            xSet.insert(provider);
            Trace.Assert(xSet.has(provider));
            if (extra_registries.Count != 0)
            {
                arg = new uno.Any(typeof(XSimpleRegistry),
                                  open_registries(extra_registries, xContext));
                provider = new uno.Any(
                    typeof(System.Object),
                    xTDprov_factory.createInstanceWithArgumentsAndContext(
                        new uno.Any[] { arg }, xContext));
                xSet.insert(provider);
                Trace.Assert(xSet.has(provider));
            }

            int dot;
            if (output.Length == 0) // no output file specified
            {
                // if only one rdb has been given, then take rdb name
                if (mandatory_registries.Count == 1)
                {
                    output = (string) mandatory_registries[0];
                    output = output.Substring(output.LastIndexOf('/') + 1);
                    dot = output.LastIndexOf('.');
                    if (dot > 0)
                        output = output.Substring(0, dot);
                }
                else
                {
                    output = "cli_unotypes";
                }
            }

            output = path_make_absolute_file_url(output);
            int slash = output.LastIndexOf('/');
            string sys_output_dir =
                (new Uri(output.Substring(0, slash))).LocalPath;
            if (sys_output_dir == null)
            {
                throw new System.Exception(
                    "cannot get system path from file url " +
                    output.Substring(0, slash));
            }
            string filename = output.Substring(slash + 1);
            dot = filename.LastIndexOf('.');
            string name = filename;
            if (dot < 0) // has no extension
                filename += ".dll";
            else
                name = name.Substring(0, dot);
            string output_dir = sys_output_dir; // FIXME redundant
            string output_file = filename; // FIXME redundant

            // Get the key pair for making a strong name
            StrongNameKeyPair kp = null;
            if (keyfile.Length > 0)
            {
                try
                {
                    FileStream fs = new FileStream(keyfile, FileMode.Open);
                    kp = new StrongNameKeyPair(fs);
                    fs.Close();
                }
                catch (FileNotFoundException)
                {
                    throw new System.Exception ("Could not find the keyfile. " + 
                                                "Verify the --keyfile argument!");
                }
            }
            else
            {
                if (g_verbose)
                {
                    Console.Error.WriteLine(
                        "> no key file specified. Cannot create strong name!");
                }
            }
            // setup assembly info: FIXME set more? e.g. avoid strong versioning
            AssemblyName assembly_name = new AssemblyName();
            assembly_name.CodeBase = output_dir;
            assembly_name.Name = name;
            if (kp != null)
                assembly_name.KeyPair = kp;

            if (version.Length != 0)
                assembly_name.Version = new Version(version);

            // app domain
            AppDomain current_appdomain = AppDomain.CurrentDomain;
            // target assembly
            AssemblyBuilder assembly_builder =
                current_appdomain.DefineDynamicAssembly(
                    assembly_name, AssemblyBuilderAccess.Save, output_dir);

            if (product.Length != 0)
            {
                Type[] types = new Type[] { typeof(System.String) };
                object[] paras = new object[] { product };
                assembly_builder.SetCustomAttribute(
                    new CustomAttributeBuilder(
                        typeof(AssemblyProductAttribute).GetConstructor(types),
                        paras));
            }
            if (description.Length != 0)
            {
                Type[] types = new Type[] { typeof(System.String) };
                object[] paras = new object[] { description };
                assembly_builder.SetCustomAttribute(
                    new CustomAttributeBuilder(
                        typeof(AssemblyDescriptionAttribute).GetConstructor(types),
                        paras));
            }
            if (company.Length != 0)
            {
                Type[] types = new Type[] { typeof(System.String) };
                object[] paras = new object[] { company };
                assembly_builder.SetCustomAttribute(
                    new CustomAttributeBuilder(
                        typeof(AssemblyCompanyAttribute).GetConstructor(types),
                        paras));
            }
            if (copyright.Length != 0)
            {
                Type[] types = new Type[] { typeof(System.String) };
                object[] paras = new object[] { copyright };
                assembly_builder.SetCustomAttribute(
                    new CustomAttributeBuilder(
                        typeof(AssemblyCopyrightAttribute).GetConstructor(types),
                        paras));
            }
            if (trademark.Length != 0)
            {
                Type[] types = new Type[] { typeof(System.String) };
                object[] paras = new object[] { trademark };
                assembly_builder.SetCustomAttribute(
                    new CustomAttributeBuilder(
                        typeof(AssemblyTrademarkAttribute).GetConstructor(types),
                        paras));
            }

            // load extra assemblies
            Assembly[] assemblies = new Assembly[extra_assemblies.Count];
            for (int pos = 0; pos < extra_assemblies.Count; ++pos)
            {
                assemblies[pos] =
                    Assembly.LoadFrom((string) extra_assemblies[pos]);
            }

            // type emitter
            TypeEmitter type_emitter = new TypeEmitter(
                assembly_builder.DefineDynamicModule(output_file), assemblies);
            // add handler resolving assembly's types
            ResolveEventHandler type_resolver =
                new ResolveEventHandler(type_emitter.type_resolve);
            current_appdomain.TypeResolve += type_resolver;

            // and emit types to it
            if (explicit_types.Count == 0)
            {
                XTypeDescriptionEnumeration xTD_enum =
                    ((XTypeDescriptionEnumerationAccess)xTD_provider)
                    .createTypeDescriptionEnumeration(
                        "", // all IDL modules
                        new TypeClass[0], // all classes of types
                        TypeDescriptionSearchDepth.INFINITE);
                while (xTD_enum.hasMoreElements())
                {
                    type_emitter.get_type(xTD_enum.nextTypeDescription());
                }                    
            }
            else
            {
                XHierarchicalNameAccess xHNA =
                    (XHierarchicalNameAccess) xTD_provider;
                foreach (string type in explicit_types)
                {
                    type_emitter.get_type(
                        (XTypeDescription) xHNA.getByHierarchicalName(type).Value);
                }
            }                                        
            type_emitter.Dispose();

            if (g_verbose)
            {
                Console.Write("> saving assembly {0}...",
                              Path.Combine(output_dir, output_file));
            }
            assembly_builder.Save(output_file);
            if (g_verbose)
            {
                Console.WriteLine("ok");
            }
            current_appdomain.TypeResolve -= type_resolver;
        }
//         catch (System.Exception exc)
//         {
//             Console.Error.WriteLine("\n> error: {0}\n> dying abnormally...",
//                                     exc.Message);
//             ret = 1;
//             throw;
//         }

        try
        {
            XComponent xComp = xContext as XComponent;
            if (xComp != null)
                xComp.dispose();
        }
        catch (System.Exception exc)
        {
            Console.Error.WriteLine("\n> error disposing component context: {0}" +
                                    "\n> dying abnormally...", exc.Message);
            ret = 1;
        }

        return ret;
    }
}

