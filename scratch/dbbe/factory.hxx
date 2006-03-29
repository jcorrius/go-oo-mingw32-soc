/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile$
 *
 *  $Revision$
 *
 *  last change: $Author$ $Date$
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/
#ifndef CONFIGMGR_DBBE_FACTORY_HXX_
#define CONFIGMGR_DBBE_FACTORY_HXX_

#include <db_cxx.h>

#include <hash_map>
#include <algorithm>

#ifndef _RTL_USTRING_HXX_
#include "rtl/ustring.hxx"
#endif

#ifndef _COM_SUN_STAR_CONFIGURATION_BACKEND_BACKENDACCESSEXCEPTION_HPP_
#include <com/sun/star/configuration/backend/BackendAccessException.hpp>
#endif // _COM_SUN_STAR_CONFIGURATION_BACKEND_BACKENDACCESSEXCEPTION_HPP_




namespace configmgr { namespace dbbe {
    namespace backend= com::sun::star::configuration::backend;
/**
   This class is intended to be used with SingletonRef, as there can
   only be one DbFactory.  This is a simple factory that give you
   references to an opened database for a specified path
 */

    class DbFactory
    {
    public:
        /** Do nothing Constructor */
        DbFactory();
        
        /** Destructor will close all databases */
        ~DbFactory();
        
        /** 
            Return a reference to an open database
            specified by path.

            @param aPath    The path to the database file
            @return  pointer to opened database
        */
        Db* getDb(rtl::OUString aPath);
        
    private:
        typedef std::hash_map<rtl::OUString, Db*, rtl::OUStringHash> DbHash;
        Db* openDb(const rtl::OUString& rPath) const
            throw (backend::BackendAccessException);
        DbHash mDbs;
    };

}}; //namespace

#endif //CONFIGMGR_DBBE_FACTORY_HXX_
