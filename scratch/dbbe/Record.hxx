#ifndef CONFIGMGR_DBBE_RECORD_HXX_
#define CONFIGMGR_DBBE_RECORD_HXX_

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif //_SAL_TYPES_H_

#include <db_cxx.h>

namespace configmgr { namespace dbbe {

/**
   An object representation of the database schema.
   
*/
    class Record
    {
    public:
        sal_Int64 date;          /** Unix Epoch */
        sal_uInt32 blobSize;     /** XML Blob Size */
        sal_uInt32 numSubLayers;
        sal_Char   *pSubLayers;  /** ARGV style list */
        sal_Char   *pBlob;        /** XML Blob */
        
        
        /**
           Fix the pointers from their marshalled invalid state 
        */
        void unMarshall(void);
        
        /**
           Spit out a Record that is marshalled to be put in the database
           
           @param size, the size of the marshalled record will be written here
        */
        Record* Marshall(size_t &size);

    private:
        size_t SubLayerLen(void);

    };
    
}}; //namespace

#endif //CONFIGMGR_DBBE_RECORD_HXX_
