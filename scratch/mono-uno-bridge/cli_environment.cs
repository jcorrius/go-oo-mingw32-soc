/*************************************************************************
 *
 *  $RCSfile: $
 *
 *  $Revision: $
 *
 *  last change: $Author: $ $Date: $
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
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

namespace cli_uno
{

using System;
using System.Collections;
using System.Runtime.Remoting;
using System.Runtime.Remoting.Proxies;
using System.Text;

using com.sun.star.bridges.mono_uno;

public class Cli_environment
{
    static string sOidPart = ";cli[0];";

    static Hashtable m_objects = Hashtable.Synchronized(new Hashtable());

    static string createKey(string oid, Type t)
    {
        return oid + t.FullName;
    }

//     FIXME setup debugging info here
//     public Cli_environment()
//     {
//     }

//     FIXME assert there are no more registered objects
//     public ~Cli_environment()
//     {
//     }

    /**
       Registers an UNO object as being mapped by this bridge. The resulting
       cli object is represents all interfaces of the UNO object. Therefore the
       object can be registered only with its OID; a type is not necessary.
    */
    public object registerInterface(object obj, string oid)
    {
        // FIXME debugging stuff
        m_objects.Add(oid, obj); // new WeakReference(obj));
        return obj;
    }

    /**
       Registers a CLI object as being mapped by this bridge. The resulting
       object represents exactly one UNO interface.
     */
    public object registerInterface(object obj, string oid, Type type)
    {
        // FIXME debugging stuff
        string key = createKey(oid, type);
        m_objects.Add(key, obj); // new WeakReference(obj));
        return obj;
    }

    /**
       By revoking an interface it is declared that the respective interface has
       not been mapped. The proxy implementations call revoke interface in their
       destructors.
     */
    public void revokeInterface(string oid)
    {
        revokeInterface(oid, null);
    }

    public void revokeInterface(string oid, Type type)
    {
        // FIXME debugging stuff
        string key = type != null ? createKey(oid, type) : oid;
        m_objects.Remove(key);
    }

    /**
     * Retrieves an interface identified by its object id and type from this
     * environment.
     *
     * @param oid object id of interface to be retrieved
     * @param type the type description of the interface to be retrieved
     * @see com.sun.star.uno.IEnvironment#getRegisteredInterface
     */
    public object getRegisteredInterface(string oid, Type type)
    {
        // try if it is a UNO interface
        object ret = null;
        ret = m_objects[oid];
        if (ret == null)
        {
            // try if it is a proxy for a cli object
            oid = createKey(oid, type);
            ret = m_objects[oid];
        }
/*        if (ret != null)
        {
            WeakReference weakIface = (WeakReference)ret;
            ret = weakIface.Target;
        } */

        if (ret == null)
            m_objects.Remove(oid);

        return ret;
    }

        /**
         * Generates a worldwide unique object identifier (oid) for the given object. It is
     * guaranteed, that subsequent calls to the method with the same object
     * will give the same id.
         * <p>
         * @return     the generated oid.
         * @param      object     the object for which a Oid should be generated.
         */
    public static string getObjectIdentifier(object obj)
    {
        string oid = null;
        RealProxy realProxy = null;

        if (RemotingServices.IsTransparentProxy(obj))
            realProxy = RemotingServices.GetRealProxy(obj);

        if (realProxy != null)
        {
            UnoInterfaceProxy proxyImpl = realProxy as UnoInterfaceProxy;
            if (proxyImpl != null)
                oid = proxyImpl.Oid;
        }

        if (oid == null)
        {
            Guid gd = typeof(Cli_environment).GUID; // FIXME apparently not a good idea with mono
            StringBuilder buf = new StringBuilder(128);
            buf.Append(obj.GetHashCode());
            buf.Append(sOidPart);
            buf.Append(gd);
            oid = buf.ToString();
        }

        return oid;
    }

}

}
