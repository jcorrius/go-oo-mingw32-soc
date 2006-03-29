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
#include "factory.hxx"

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif //_OSL_DIAGNOSE_H_

#ifndef _OSL_FILE_HXX_
#include <osl/file.hxx>
#endif

namespace configmgr { namespace dbbe {

    DbFactory::DbFactory() { }

    DbFactory::~DbFactory()
    {
        DbHash::iterator it;

        for (it= mDbs.begin(); it != mDbs.end(); it++)
        {
            //throwing exceptions in destructors is double-plus ungood
            OSL_ASSERT(!((*it).second->close(0))); 
        }
    }
    
    Db* DbFactory::getDb(rtl::OUString aPath)
    {
        //Empty paths are not allowed.
        OSL_ASSERT(aPath.getlength());

        Db* pDb= NULL;
        DbHash::iterator it;

        it= mDbs.find(aPath);
        if (it == mDbs.end())
        { //not in there, make a new one
            pDb= openDb(aPath);
            mDbs[aPath]= pDb;
        }
        else
        { //it's in there
            pDb= (*it).second;
        }
        OSL_ASSERT(pDb);
        return pDb;
    }

    Db* DbFactory::openDb(const rtl::OUString& rPath) const
                    throw (backend::BackendAccessException)
    {
        u_int32_t open_flags= DB_THREAD;
        const u_int32_t db_flags= DB_CXX_NO_EXCEPTIONS;
        const char* path= OUStringToOString(rPath, RTL_TEXTENCODING_UTF8).getStr();
        Db* pDb;

        try 
        {
            pDb= new Db(NULL, db_flags);
        }
        catch (std::bad_alloc)
        {
            throw backend::BackendAccessException();
        }
        
        //if path is /path/to/foo instead of file:// will this work?
        osl::DirectoryItem aDirItem;
        if (osl::DirectoryItem::E_None != osl::DirectoryItem::get(rPath, aDirItem))
        {
            throw backend::BackendAccessException();
        }
        osl::FileStatus aFileStatus(FileStatusMask_Attributes);
        if (aDirItem.getFileStatus(aFileStatus) == osl::DirectoryItem::E_None)
        {
            if (aFileStatus.getAttributes() & Attribute_ReadOnly)
            {
                open_flags |= DB_RDONLY;
            }          
        }
        else
        {
            throw backend::BackendAccessException();
        }        

        int ret;
        ret= pDb->set_lorder(1234); //little endian
        ret= pDb->open(NULL, path, NULL,  DB_UNKNOWN, 0, open_flags);
        if (ret)
        { 
            throw backend::BackendAccessException();
        }
        return pDb;
    }

}}; //namespaces

#endif //CONFIGMGR_DBBE_FACTORY_HXX_
