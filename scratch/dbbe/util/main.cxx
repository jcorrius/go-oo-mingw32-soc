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
#include "sal/main.h"
#include "sal/types.h"

#ifdef SYSTEM_DB
#include <db_cxx.h>
#else
#include <berkeleydb/db_cxx.h>
#endif
#include <cstdlib>
#include <stdlib.h>

#include "../Record.hxx" //FIXME
#include "minor.hxx"
#include "major.hxx"

#include "argparse.hxx"

#include <rtl/alloc.h>
#include <rtl/memory.h>


#include <assert.h> //fixme

const char* errpfx="BerkeleyDB error";

/**
   Major Usages we will support
   
   ImportDb   Import the keys from a database you supply to ours
   CheckIn    Create a database representation from a file representation
   CheckOut   Create a file representation from a database representation
   
   Minor Usages
   Stat          Print some database statistics
   SanityCheck   Perform some basic sanity checks   --done
 */


Db* openDatabase(const char* path, const bool open_ro, const bool create= false)
{
    OSL_ASSERT(path);
    OSL_ASSERT(open_ro || create);
    u_int32_t open_flags= 0;
    const u_int32_t db_flags= DB_CXX_NO_EXCEPTIONS;
    Db* aDatabase= new Db(NULL, db_flags);

    OSL_ASSERT(aDatabase);
    aDatabase->set_errpfx(errpfx);
    aDatabase->set_error_stream(&(std::cerr));
    if (open_ro)
    {
        open_flags |= DB_RDONLY;
    }
    if (create)
    {
        open_flags |= DB_CREATE;
    }
    int ret;
    OSL_VERIFY(!aDatabase->set_lorder(1234)); //little endian
    ret= aDatabase->open(NULL, path, 
                         NULL, DB_BTREE, open_flags, 0);
    if (ret)
    { 
        std::cerr << "Unable to open database \"" << path << "\"" << std::endl;
        aDatabase->err(ret, NULL);
        std::cerr << "ret= " << ret << std::endl;
        abort();
    }   
    return aDatabase;

}

//begin temp debugging stuff
int gimmeRand(int to, int from= 0)
{
    double fTo= to;
    double fFrom= from;
    return from + (int) (fTo * (rand() / (RAND_MAX + fFrom)));
}

void randomString(char* string, size_t len, bool nullTerm=true)
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

void testSubLayerLen()
{
    using namespace configmgr::dbbe;
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

bool RecordMatch(configmgr::dbbe::Record *p1,
                 configmgr::dbbe::Record *p2)
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

void testMarshal()
{
    using namespace configmgr::dbbe;
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
        if (!memcmp(aRecord.pBlob, pRecord+sizeof(Record), aRecord.blobSize))
        {
            cerr << "\tthe blobs match" << endl;
        }
        else
        {
            cerr << "\tthe blobs do not match!" << endl;
        }

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
        if (!memcmp(aRecord.pSubLayers, pRecord+sizeof(Record), aRecord.SubLayerLen()))
        {
            cerr << "\tthe sublayers match" << endl;
        }
        else
        {
            cerr << "\tthe sublayers do not match!" << endl;
        }
        
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
        if (!memcmp(aRecord.pSubLayers, pRecord+sizeof(Record), aRecord.SubLayerLen()))
        {
            cerr << "\tthe sublayers match" << endl;
        }
        else
        {
            cerr << "\tthe sublayers do not match!" << endl;
        }
        if (!memcmp(aRecord.pBlob, pRecord+sizeof(Record)+aRecord.SubLayerLen(), aRecord.SubLayerLen()))
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
      
        
        
        free(aRecord.pSubLayers);
        free(aRecord.pBlob);
        free(pRecord);
    }
}



//end temp debugging stuff

SAL_IMPLEMENT_MAIN_WITH_ARGS(argc, argv)
{
    //testSubLayerLen(); //passed
    testMarshal(); //passed
    exit(0);
    

    const char* summary= "[options] mode mode-options";
    const char* footer=  
        "modes:\n"
        "\timport <path/file>\t\n"
        "\tstat\n"
        "\tsanity\n";
    
    /* flags */
    bool helpFlag= false;
    bool dbFlag= false;
    bool codeFlag= false;

    /* values */
    char *db_path= NULL;
    char *code= NULL;
    

    static arg args[]=
        {
            {
                "help",
                'h',
                true,
                false,
                &helpFlag,
                NULL,
                "this help"
            },
            {
                "database",
                'd',
                false,
                true,
                &dbFlag,
                &db_path,
                "Path to the database to operate on"
            },
            {
                "code",
                'c',
                true,
                true,
                &codeFlag,
                &code,
                "conversion code to use for path"
            },
            {0, 0, 0, 0, 0, 0}
        };
    argParser aParser(argc, argv, args, summary);
    int optind= aParser.parseArgs();
    if (helpFlag)
    {
        aParser.printUsage(NULL, footer);
        exit(0);
    }
    if (!aParser.mandatoryOptsSet())
    {
        aParser.printUsage(NULL, footer);
        abort();
    }
    Db* aDatabase= NULL;

    /* modes */
    const char* sanity_check = "sanity";
    const char* stat         = "stat";
    const char* import       = "import";

    if (db_path == NULL)
    {
        std::cerr << "No database file specified!" << std::endl << std::endl;
        aParser.printUsage(NULL, footer);
        abort();
    }
    if (optind < argc)
    {
        if (!strcmp(argv[optind], sanity_check))
        {
            aDatabase= openDatabase(db_path, true);
            if (configmgr::dbbe::SanityCheck(*aDatabase))
            {
                std::cout << "Everything seems normal" << std::endl;
                exit(0);
            }
            else
            {
                std::cerr << "Errors detected in database!" << std::endl;
                abort();
            }
            
        }
        else if(!strcmp(argv[optind], stat))
        {
            aDatabase= openDatabase(db_path, true);
            optind++;
            char* graphFile= NULL;
            if (optind < argc)
            {
                graphFile= argv[optind];
            }
            configmgr::dbbe::Stat(*aDatabase, graphFile);
            exit(0);
            
        }
        else if(!strcmp(argv[optind], import))
        {
            aDatabase= openDatabase(db_path, false, true);
            configmgr::dbbe::repository aRepository(*aDatabase, db_path);
            optind++;
            if (optind < argc)
            {
                aRepository.importPath(argv[optind++], code);
                exit(0);
            }
            else
            {
                aParser.printUsage(NULL, footer);
                abort();
            }
        }
        aParser.printUsage(NULL, footer);
        abort();
    } 
    aParser.printUsage(NULL, footer);
    abort();
    
    return 0;
}
