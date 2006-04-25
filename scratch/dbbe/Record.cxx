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
#ifndef CONFIGMGR_DBBE_RECORD_HXX_
#include "Record.hxx"
#endif //CONFIGMGR_DBBE_RECORD_HXX_

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif //_OSL_DIAGNOSE_H_

#ifndef _RTL_ALLOC_H_
#include <rtl/alloc.h>
#endif //_RTL_ALLOC_H_

#ifndef _RTL_MEMORY_H_
#include <rtl/memory.h>
#endif

#ifndef _OSL_ENDIAN_H_
#include <osl/endian.h>
#endif

#ifndef _OSL_FILE_HXX_
#include <osl/file.hxx>
#endif//_OSL_FILE_HXX_

#ifndef _OSL_TIME_H_
#include <osl/time.h>
#endif//_OSL_TIME_H_ 

namespace configmgr { namespace dbbe {

    Record::Record(void)    
    {
        //please forgive my crusty c-programmer ways
        memset(this, 0, sizeof(Record));
    }        
    
    size_t Record::SubLayerLen(void) const
    {
        size_t ret= 0;
        if (numSubLayers)
        {
            sal_Char *p= pSubLayers;
            OSL_ASSERT(p);
            for (int i= 0; i < numSubLayers; i++)
            {
                size_t len= strlen(p) + 1; //to include the NULL
                ret+= len;
                p+= len + 1; //to push us into the next string
            }
        }
        return ret;
    }

    void Record::unMarshal()
    {
        pSubLayers= (sal_Char*)(this) + sizeof(Record);
        pBlob= (sal_Char*)(this) + sizeof(Record) + SubLayerLen();
    }
    
    Record* Record::Marshal(size_t &size) const
    {
        Record* pRecord;
                
        size= sizeof(Record) + SubLayerLen() + blobSize;
        pRecord= (Record*)(rtl_allocateMemory(size));
        OSL_ASSERT(pRecord);
        rtl_copyMemory(pRecord, this, sizeof(Record));
        rtl_copyMemory(pRecord + sizeof(Record), pSubLayers, SubLayerLen());
        rtl_copyMemory(pRecord + sizeof(Record) + SubLayerLen(), pBlob, blobSize);
        //set pointers to NULL to serve as a warning to others
        pRecord->pBlob= NULL;
        pRecord->pSubLayers= NULL;
        return pRecord;
    }
    
    void Record::bytesex(void)
    {
        //32-bit fields
        OSL_SWAPDWORD(blobSize);
        OSL_SWAPDWORD(numSubLayers);
        //64-bit fields
        OSL_SWAPDWORD(date);        
    }

    std::vector<sal_Char*> Record::listSubLayers(void) const
    {
        std::vector<sal_Char*> ret(numSubLayers);
        
        sal_uInt32 i= numSubLayers;
        sal_Char* pString= pSubLayers;
        while (i)
        {
            ret.push_back(pString);
            while (*pString)
                pString++;
            pString++; //kick us into the next string
            i--;
        }        
        return ret;
    }

    void Record::setSubLayers(std::vector<sal_Char*>& aSubLayers)
    {
        std::vector<sal_Char*>::iterator it;
        size_t len= 0;
        for (it= aSubLayers.begin(); it != aSubLayers.end(); it++)
        {
            OSL_ASSERT(*it);
//            std::cerr << "it=" << *it << std::endl;
            len+= strlen(*it) + 1; //one for the null
        }
        
        //if this is a serialized copy, then don't try to free the memory, just overwrite it
        if (pSubLayers && 
            (pSubLayers != (sal_Char*)(this) + sizeof(Record)))
            rtl_freeMemory(pSubLayers);
        pSubLayers= static_cast<sal_Char*>(rtl_allocateMemory(len));
        OSL_ASSERT(pSubLayers);
        sal_Char* pString= pSubLayers;
        for (it= aSubLayers.begin(); it != aSubLayers.end(); it++)
        {
            size_t len= strlen(*it);
            rtl_copyMemory(pString, *it, len);
            pString+= len + 1;
            *pString= 0;
            pString++;
        }
    }

    void Record::setBlobFromFile(const rtl::OUString &aFileURL)
    {
        using namespace osl;

        OSL_ASSERT(aFileURL.getLength()); //empty file URLs not allowed
        
        std::cerr << "getting blob from '" << 
            rtl::OUStringToOString(aFileURL, RTL_TEXTENCODING_UTF8).getStr()
                  << "\n";

        //get the date and type
        DirectoryItem aItem;
        OSL_VERIFY(DirectoryItem::get(aFileURL, aItem) == DirectoryItem::E_None);
        FileStatus aFileStatus(FileStatusMask_ModifyTime || FileStatusMask_Type);
        OSL_VERIFY(aItem.getFileStatus(aFileStatus) == DirectoryItem::E_None);
        OSL_VERIFY(aFileStatus.getFileType() == FileStatus::Regular);
        OSL_VERIFY(aFileStatus.isValid(FileStatusMask_ModifyTime));
        TimeValue aTimeValue= aFileStatus.getModifyTime();
        date= aTimeValue.Seconds;
        
        //get the data and time
        File aFile(aFileURL);
        OSL_VERIFY(aFile.open(OpenFlag_Read) == File::E_None);
        sal_uInt64 aBlobSize= 0;
        OSL_VERIFY(aFile.getSize(aBlobSize) == File::E_None);
        blobSize= static_cast<sal_uInt32>(aBlobSize);
        std::cerr << "before\n";
        pBlob= static_cast<sal_Char*>(rtl_allocateMemory(blobSize));
        std::cerr << "after\n";
        OSL_ASSERT(pBlob);
        sal_uInt64 aBytesToRead= blobSize;
        while (aBytesToRead)
            OSL_VERIFY(aFile.read(static_cast<void*>(pBlob + (size_t)(blobSize - aBytesToRead)), 
                                  aBytesToRead, aBytesToRead) == File::E_None);

    }

    void Record::setFileFromBlob(const rtl::OUString &aFileURL)
    {
        OSL_VERIFY(aFileURL.getLength());
        osl::File aFile(aFileURL);
        TimeValue aTimeValue= {static_cast<sal_uInt32>(date), 0};
        OSL_VERIFY(osl::File::setTime(aFileURL, aTimeValue, aTimeValue, aTimeValue) == osl::File::E_None);
        sal_uInt64 bytesLeft= 0;
        OSL_VERIFY(aFile.write(static_cast<void*>(pBlob), blobSize, bytesLeft) == osl::File::E_None);
        OSL_VERIFY(!bytesLeft);
        OSL_VERIFY(aFile.sync() == osl::FileBase::E_None);
    }

    void Record::touch(void)
    {
        TimeValue timeval= {0, 0};
        osl_getSystemTime(&timeval);
        date= timeval.Seconds;
    }

    Dbt Record::getDbt(void) const
    {
        OSL_VERIFY(isMarshaled());
        return Dbt((void*)this,
                   sizeof(Record) + SubLayerLen() + blobSize);
    }

    /**
       a simple, and not very thorough check to see if this is a marshaled Record
     */
    bool Record::isMarshaled(void) const 
    {
        if (pSubLayers == NULL && pBlob == NULL)
            return true;
        return false;
    }

    Record* Record::getFromDbt(const Dbt &aDbt)
    {
        return static_cast<Record*>(aDbt.get_data());
    }
    
    sal_Bool Record::putRecord(Db& aDatabase, rtl::OString key) const
    {
        return putRecord(aDatabase, key.getStr());
    }

    sal_Bool Record::putRecord(Db& aDatabase, const char* key) const
    {
           size_t dataSize= 0;
           Record* pRecord= Marshal(dataSize);
           if (!pRecord)
               return sal_False;
           int swap= 0;
           OSL_VERIFY(!aDatabase.get_byteswapped(&swap));
           if (swap)
               pRecord->bytesex();
           Dbt Key((void*)key, strlen(key) + 1);
           Dbt Data((void*)(pRecord), dataSize);
           if (aDatabase.put(NULL, &Key, &Data, 0))
               return sal_False;
           rtl_freeMemory(pRecord);
           return sal_True;
    }

    sal_Int32 Record::getRecord(Db& aDatabase, const rtl::OString &key, Record &aResult)
    {
        return getRecord(aDatabase, key.getStr(), aResult);
    }

    sal_Int32 Record::getRecord(Db& aDatabase, const char* key, Record &aResult)
    {
        OSL_VERIFY(key);
        Dbt Key((void*)key, strlen(key) + 1);
        Dbt Data;
        sal_Int32 ret= aDatabase.get(NULL, &Key, &Data, 0);
        if (!ret)
        {// found
            OSL_VERIFY(Data.get_data());
            aResult= *(static_cast<Record*>(Data.get_data()));
            int swap= 0;
            OSL_VERIFY(!aDatabase.get_byteswapped(&swap));
            if (swap)
                aResult.bytesex();
            aResult.unMarshal();
        }
        return ret;
    }


}}; //namespace
    
