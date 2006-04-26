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

#include <db_cxx.h>
#include <cstdlib>

#include "../Record.hxx" //FIXME
#include "minor.hxx"
#include "major.hxx"

#include "argparse.hxx"

/**
   Major Usages we will support
   
   ImportDb   Import the keys from a database you supply to ours
   CheckIn    Create a database representation from a file representation
   CheckOut   Create a file representation from a database representation
   
   Minor Usages
   Stat          Print some database statistics
   SanityCheck   Perform some basic sanity checks   --done
 */


void DbErrorCallback(const DbEnv *dbenv, const char *errpfx, const char *msg)
{
    std::cerr << "berkelyDb error" << msg << std::endl; 
}

Db* openDatabase(const char* path, const bool open_ro, const bool create= false)
{
    OSL_ASSERT(path);
    OSL_ASSERT(open_ro || create);
    u_int32_t open_flags= 0;
    const u_int32_t db_flags= DB_CXX_NO_EXCEPTIONS;
    Db* aDatabase= new Db(NULL, db_flags);

    OSL_ASSERT(aDatabase);
    aDatabase->set_errcall(DbErrorCallback);
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

//FIXME: make this work


SAL_IMPLEMENT_MAIN_WITH_ARGS(argc, argv)
{
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
                argName:     "help",
                shortName:   'h',
                optional:    true,
                hasValue:    false,
                flagStore:   &helpFlag,
                valueStore:  NULL,
                usageString: "this help"
            },
            {
                argName:     "database",
                shortName:   'd',
                optional:    false,
                hasValue:    true,
                flagStore:   &dbFlag,
                valueStore:  &db_path,
                usageString: "Path to the database to operate on"
            },
            {
                argName:     "code",
                shortName:   'c',
                optional:    true,
                hasValue:    true,
                flagStore:   &codeFlag,
                valueStore:  &code,
                usageString: "conversion code to use for path"
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
