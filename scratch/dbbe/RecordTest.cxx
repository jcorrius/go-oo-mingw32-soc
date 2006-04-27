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
#ifndef CONFIGMGR_DBBE_RECORDTEST_HXX_
#include "RecordTest.hxx"
#endif

#include <time.h>
#include <assert.h>
#include <rtl/alloc.h>
#include <rtl/memory.h>

//purposeful syntax error

namespace configmgr
{
    namespace dbbe
    {
        RecordTester::RecordTester()
        {
            srand(time(0));
        }
        
        int RecordTester::gimmeRand(int to, int from)
        {
            double fTo= to;
            double fFrom= from;
            return from + (int) (fTo * (rand() / (RAND_MAX + fFrom)));
        }

        void RecordTester::randomString(char* string, size_t len, bool nullTerm)
        {
            OSL_ASSERT(string);
            for (unsigned int i= 0; i < len; i++)
            {
                string[i]= rand() % (26) + 'a';
            }
            if (nullTerm)
            {
                string[len - 1]= 0;
            }
        }

        void RecordTester::testSubLayerLen()
        {
            using namespace std;
    
            cerr << "testing the empty sublayer case" << endl;
            {
                Record aRecord;
                cerr << "\tSubLayerLen() reports " << aRecord.SubLayerLen() << endl;
            }
            cerr << endl;

            cerr << "testing a single string sublayerlen" << endl;
            {
                Record aRecord;
                size_t len= gimmeRand(100);
                cerr << "\tstring length is "<< len << endl;
                aRecord.numSubLayers= 1;        
                aRecord.pSubLayers= (char*)malloc(len);
                assert(aRecord.pSubLayers);
                randomString(aRecord.pSubLayers, len);
                cerr << "\tSubLayerLen() reports " << aRecord.SubLayerLen() << endl;
                free(aRecord.pSubLayers);
            }
            cerr << endl;
            cerr << "testing multiple string sublayerlen" << endl;
            {
                Record aRecord;
                size_t len= gimmeRand(100);
                cerr << "\tstring length is "<< len << endl;
                aRecord.pSubLayers= (char*)malloc(len);
                assert(aRecord.pSubLayers);
                randomString(aRecord.pSubLayers, len);
                aRecord.numSubLayers= 2;
                cerr << "\tnumber of sublayers is " << aRecord.numSubLayers << endl;
                aRecord.pSubLayers[len/2]= 0;
                cerr << "\tSubLayerLen() reports " << aRecord.SubLayerLen() << endl;
                free(aRecord.pSubLayers);
            }    
        }

        bool RecordTester::RecordMatch(const configmgr::dbbe::Record *p1,
                                       const configmgr::dbbe::Record *p2)
        {
            OSL_ASSERT(p1);
            OSL_ASSERT(p2);
            if (p1->date != p2->date)
                return false;
            if (p1->blobSize != p2->blobSize)
                return false;
            if (p1->numSubLayers != p2->numSubLayers)
                return false;
            if (p1->pBlob && p2->pBlob)
                if (memcmp(p1->pBlob, p2->pBlob, p1->blobSize))
                    return false;
            if (p1->pSubLayers && p2->pSubLayers)
                if (memcmp(p1->pSubLayers, p2->pSubLayers, p1->SubLayerLen()))
                    return false;
            return true;
        }

        void RecordTester::testMarshal()
        {
            using namespace std;
    
            cerr << "testing the trivial Record" << endl;
            {
                Record aRecord;
                aRecord.touch();
                size_t size;
                Record* pRecord= aRecord.Marshal(size);
                cerr << "\tsizeof(Record) is " << sizeof(Record) << endl;
                cerr << "\tmarshalled size is " << size << endl;
                if (!memcmp(&aRecord, pRecord, sizeof(Record)))            
                {
                    cerr << "\tthe contents match" << endl;
                }
                else
                {
                    cerr << "\tthe contents do not match!" << endl;
                }
                pRecord->unMarshal();        
                if (RecordMatch(&aRecord, pRecord))
                    cerr << "\tthe unmarshalled records match" << endl;
                else
                    cerr << "\tthe unmasrhalled records do not match!" << endl;

                rtl_freeMemory(pRecord);
        
            }
            cerr << endl;
    
            cerr << "testing a record with no sublayers" << endl;
            {
                Record aRecord;
                aRecord.touch();
                aRecord.blobSize= gimmeRand(400*1024);
                aRecord.pBlob= (char*)malloc(aRecord.blobSize);
                assert(aRecord.pBlob);
                cerr << "\tusing a blob size of " << aRecord.blobSize << endl;
                cerr << "\tthat makes a total size of " << sizeof(Record) + aRecord.blobSize << endl;
                randomString(aRecord.pBlob, aRecord.blobSize, false);
                size_t size;
                Record* pRecord= aRecord.Marshal(size);
                cerr << "\tmarshalled size is " << size << endl;
                if (RecordMatch(&aRecord, pRecord))
                {
                    cerr << "\tThe Records match" << endl;
                }
                else
                {
                    cerr << "\tThe Records do not match!" << endl;
                }
                if (!memcmp(aRecord.pBlob, (char*)pRecord+sizeof(Record), aRecord.blobSize))
                {
                    cerr << "\tthe blobs match" << endl;
                }
                else
                {
                    cerr << "\tthe blobs do not match!" << endl;
                }
                pRecord->unMarshal();        
                if (RecordMatch(&aRecord, pRecord))
                    cerr << "\tthe unmarshalled records match" << endl;
                else
                    cerr << "\tthe unmasrhalled records do not match!" << endl;

                rtl_freeMemory(pRecord);                
            }
            cerr << endl;

            cerr << "testing a Record with sublayers but no blob" << endl;
            {
                Record aRecord;
                aRecord.touch();
                size_t len= gimmeRand(100);
                cerr << "\tstring length is "<< len << endl;
                aRecord.pSubLayers= (char*)malloc(len);
                assert(aRecord.pSubLayers);
                randomString(aRecord.pSubLayers, len);
                aRecord.numSubLayers= 2;
                cerr << "\tnumber of sublayers is " << aRecord.numSubLayers << endl;
                aRecord.pSubLayers[len/2]= 0;
                cerr << "\tThe total size is then " << sizeof(Record)+aRecord.SubLayerLen() << endl;
        
                size_t size= 0;
                Record* pRecord= aRecord.Marshal(size);
                cerr << "\tMarshal reports the size as " << size << endl;
                if (RecordMatch(&aRecord, pRecord))
                {
                    cerr << "\tThe Records match" << endl;
                }
                else
                {
                    cerr << "\tThe Records do not match!" << endl;
                }
                if (!memcmp(aRecord.pSubLayers, (char*)pRecord+sizeof(Record), aRecord.SubLayerLen()))
                {
                    cerr << "\tthe sublayers match" << endl;
                }
                else
                {
                    cerr << "\tthe sublayers do not match!" << endl;
                }
                pRecord->unMarshal();        
                if (RecordMatch(&aRecord, pRecord))
                    cerr << "\tthe unmarshalled records match" << endl;
                else
                    cerr << "\tthe unmasrhalled records do not match!" << endl;
        
                free(aRecord.pSubLayers);
                free(pRecord);
            }
            cerr << endl;

            cerr << "testing a Record with sublayers and a blob" << endl;
            {
                Record aRecord;
                aRecord.touch();
                size_t len= gimmeRand(100);
                cerr << "\tstring length is "<< len << endl;
                aRecord.pSubLayers= (char*)malloc(len);
                assert(aRecord.pSubLayers);
                randomString(aRecord.pSubLayers, len);
                aRecord.numSubLayers= 2;
                cerr << "\tnumber of sublayers is " << aRecord.numSubLayers << endl;
                aRecord.pSubLayers[len/2]= 0;
                aRecord.blobSize= gimmeRand(400*1024);
                aRecord.pBlob= (char*)malloc(aRecord.blobSize);
                assert(aRecord.pBlob);
                cerr << "\tusing a blob size of " << aRecord.blobSize << endl;
                randomString(aRecord.pBlob, aRecord.blobSize, false);
                cerr << "\tthe total size is then " << sizeof(Record) + aRecord.SubLayerLen() + aRecord.blobSize << endl;
                size_t size;
                Record *pRecord= aRecord.Marshal(size);
                cerr << "\tMarshal reports the size as " << size << endl;
                if (RecordMatch(&aRecord, pRecord))
                {
                    cerr << "\tThe Records match" << endl;
                }
                else
                {
                    cerr << "\tThe Records do not match!" << endl;
                }
                if (!memcmp(aRecord.pSubLayers, (char*)pRecord+sizeof(Record), aRecord.SubLayerLen()))
                {
                    cerr << "\tthe sublayers match" << endl;
                }
                else
                {
                    cerr << "\tthe sublayers do not match!" << endl;
                }
                if (!memcmp(aRecord.pBlob, (char*)pRecord+sizeof(Record)+aRecord.SubLayerLen(), aRecord.SubLayerLen()))
                {
                    cerr << "\tthe blobs match" << endl;
                }
                else
                {
                    cerr << "\tthe blobs do not match!" << endl;
                }
                if (RecordMatch(&aRecord, pRecord))
                    cerr << "\tthe marshalled records match" << endl;
                else
                    cerr << "\tthe masrhalled records do not match!" << endl;
                pRecord->unMarshal();        
                if (RecordMatch(&aRecord, pRecord))
                    cerr << "\tthe unmarshalled records match" << endl;
                else
                    cerr << "\tthe unmasrhalled records do not match!" << endl;

                free(aRecord.pSubLayers);
                free(aRecord.pBlob);
                free(pRecord);
            }
        }

        void RecordTester::testSetSubLayer()
        {
            using namespace std;

            cerr << "The trivial case of no sublayers" << endl;
            {
                Record aRecord;
                aRecord.touch();
                vector<sal_Char*> vec= aRecord.listSubLayers();
                cerr << "\twe have been returned a vector with " << vec.size() << " elements" << endl;
            }
            cerr << endl;

            cerr << "Set some sublayers and then read them out" << endl;
            {
                Record aRecord;
                aRecord.touch();
                int count= gimmeRand(100);
                cerr << "\tWe will make " << count << " sublayers and add them" << endl;
                vector<sal_Char*> vec;
                size_t totStrSize= 0;
                for (int i=0; i < count; i++)
                {
                    size_t strSize= gimmeRand(100, 3);
                    totStrSize+= strSize;
                    sal_Char* str= (sal_Char*)malloc(strSize);
                    randomString(str, strSize);
                    vec.push_back(str);
                }
                aRecord.setSubLayers(vec);
                assert(aRecord.pSubLayers);
                cerr << "\tTotal size of sublayer strings is " << totStrSize << endl;
                cerr << "\tSubLayerLen is " << aRecord.SubLayerLen() << endl;
                cerr << "\tWe will now fetch them back out" << endl;
                vector<sal_Char*> vec2;
                vec2= aRecord.listSubLayers();
                cerr << "\tThe sublayers we got back has " << vec2.size() << " elements" << endl;
                bool match= true;
                for (int i= 0; i < count; i++)
                {
                    if (strcmp(vec[i], vec2[i]))
                        match= false;
                }
                if (match)
                    cerr << "\tThe sublayers match" << endl;
                else
                    cerr << "\tThe sublayers do not match!" << endl;
            }
            cerr << endl;
    
            cerr << "Set some sublayers and then set some more" << endl;
            {
                Record aRecord;
                aRecord.touch();
                int count= gimmeRand(100);
                cerr << "\tWe will make " << count << " sublayers and add them" << endl;
                vector<sal_Char*> vec;
                size_t totStrSize= 0;
                for (int i=0; i < count; i++)
                {
                    size_t strSize= gimmeRand(100, 3);
                    totStrSize+= strSize;
                    sal_Char* str= (sal_Char*)malloc(strSize);
                    randomString(str, strSize);
                    vec.push_back(str);
                }
                cerr << "\tTotal size of these sublayer strings is " << totStrSize << endl;   
                aRecord.setSubLayers(vec);
                assert(aRecord.pSubLayers);
                cerr << "\tSubLayerLen reports " << aRecord.SubLayerLen() << endl;
        
                count= gimmeRand(100);
                cerr << "\tWe will make " << count << " aditional sublayers and add them" << endl;
                vector<sal_Char*> vec1;
                totStrSize= 0;
                for (int i=0; i < count; i++)
                {
                    size_t strSize= gimmeRand(100, 3);
                    totStrSize+= strSize;
                    sal_Char* str= (sal_Char*)malloc(strSize);
                    randomString(str, strSize);
                    vec1.push_back(str);
                }
                cerr << "\tTotal size of these sublayer strings is " << totStrSize << endl;
                aRecord.setSubLayers(vec1);
                assert(aRecord.pSubLayers);
                cerr << "\tThat corresponds to " << aRecord.numSubLayers << " sublayers in the structure" << endl;
                cerr << "\tThat makes a new SubLayerLen of " << aRecord.SubLayerLen() << endl;
        
                bool match=true;
                vector<sal_Char*> vec2= aRecord.listSubLayers();
                cerr << "\tWe got back " << vec2.size() << " sublayers with listSubLayers" << endl;
                for (int i= 0; i < vec2.size(); i++)
                {
                    if (strcmp(vec1[i], vec2[i]))
                        match= false;
                }
                if (match)
                    cerr << "\tthe sublayers match" << endl;
                else
                    cerr << "\tthe sublayers do not match!" << endl;
            }

        }
    };
};

