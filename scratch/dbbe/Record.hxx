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
#define CONFIGMGR_DBBE_RECORD_HXX_

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif //_SAL_TYPES_H_

#ifndef _OSL_ENDIAN_H_
#include <osl/endian.h>
#endif//_OSL_ENDIAN_H_

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif//_RTL_USTRING_HXX_

#ifdef SYSTEM_DB
#include <db_cxx.h>
#else
#include <berkeleydb/db_cxx.h>
#endif

#include <vector>

namespace configmgr 
{ 
    namespace dbbe 
    {

//
//  FIXME
//   make marshal do the bytesex
//


/**
   An object representation of the database schema.
   
*/
#pragma pack(push, 4)
        class Record
        {
        public:
            sal_Int64  date;          /** Unix Epoch */
            sal_uInt32 blobSize;      /** XML Blob Size */
            sal_uInt32 numSubLayers;
            sal_Char   *pSubLayers;   /** ARGV style list */
            sal_Char   *pBlob;        /** XML Blob */
        
            /**  ctor   */
            Record(void);

            /**
               Fix the pointers from their marshalled invalid state 
            */
            void unMarshal(void);
        
            /**
               Spit out a Record that is marshalled to be put in the database
           
               @param size, the size of the marshalled record will be written here
            */
            Record* Marshal(size_t &size) const;
        
            /**
               Change order from little to big endian or vice-versa
            */
            void bytesex(void);

            /**
               Give me alist of sublayers this record has
           
               @return a vector containing sal_Char*'s
            */
            std::vector<sal_Char*> listSubLayers(void) const;

            /**
               Set the sublayers that this record has
           
               @param aSubLayers   vector<sal_Char*> of sublayers
            */
            void setSubLayers(std::vector<sal_Char*>& aSubLayers);

            /**
               Set the blobsize, date, and blob data
           
               @param aFileURL   the URL of the blob data
            */
            void setBlobFromFile(const rtl::OUString &aFileURL);

            /**
               Make a file from this Record
           
               @param aFileURL   the URL of the file you wish to write
            */
            void setFileFromBlob(const rtl::OUString &aFileURL);

            /**
               Set the date to now
            */
            void touch(void);

            /**
               Return a Dbt that points to this record
            */
            Dbt getDbt(void) const;

            /**
               Put this record in the database
               Record is not expected to be serialized before hand
           
               @param  aDatabase  the database to stuff it in
               @return sal_True if succesfull and sal_False otherwise
            */
            sal_Bool putRecord(Db& aDatabase, const char* key) const;
            sal_Bool putRecord(Db& aDatabase, const rtl::OString key) const;

        
            /**
               Produce an record ready to be used from a database from a key
           
               @param aDatabase       the database to look in
               @param key             the key to look for
               @param ppRecord[out]   the Record that will be generated if found
               @returns               the return value that berkeleyDb returns
            */
            static sal_Int32 getRecord(Db& aDatabase, const char* key, Record **ppRecord, sal_Bool partial=sal_True);
            static sal_Int32 getRecord(Db& aDatabase, const rtl::OString &key, Record **ppRecord, sal_Bool partial=sal_True);

            //this is so!! useless!!
            static Record* Record::getFromDbt(const Dbt &aDbt);
        
        protected:
            size_t SubLayerLen(void) const;
            size_t getSize(void) const;
            bool isMarshaled(void) const;
#if OSL_DEBUG_LEVEL > 0
            friend class RecordTester;
#endif
        };
#pragma pack(pop)    

    }
}; //namespace

#if OSL_DEBUG_LEVEL > 0
#include "RecordTest.hxx"
#endif

#endif //CONFIGMGR_DBBE_RECORD_HXX_
