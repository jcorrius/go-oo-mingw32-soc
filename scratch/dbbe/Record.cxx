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

    void Record::unMarshall()
    {
        pSubLayers= (sal_Char*)(this) + sizeof(Record);
        pBlob= (sal_Char*)(this) + sizeof(Record) + SubLayerLen();
    }
    
    Record* Record::Marshall(size_t &size)
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
}}; //namespace
    
