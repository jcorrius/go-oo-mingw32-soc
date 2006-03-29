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

namespace configmgr { namespace dbbe {
    
    size_t Record::SubLayerLen(void)
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
    
    Record* Record::Marshal(size_t &size)
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

    std::vector<sal_Char*> Record::listSubLayers(void)
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

}}; //namespace
    
