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

//I use getopt parsing here, which is a POSIX thing
//This is probably bad, but I just can't help myself
#include <getopt.h> 

/**
   Major Usages we will support
   
   ImportDb   Import the keys from a database you supply to ours
   CheckIn    Create a database representation from a file representation
   CheckOut   Create a file representation from a database representation
   
   Minor Usages
   Stat          Print some database statistics
   SanityCheck   Perform some basic sanity checks   --done
 */


void printUsage(const char* progName)
{
    using namespace std;
    
    cerr << "Usage: " << progName << " --database=<database filename> [--code=<code>] action options" << endl;
    cerr << "\t where actions can be one of 'import', 'stat', or 'sanity'" << endl << endl;
    cerr << "\t options for import:" << endl << endl;
    cerr << "\t\t import <path>" << endl;
    cerr << "\t\t where <path> is either a path containing xcu files or " << endl;
    cerr << "\t\t an xcu file" << endl << endl;
    cerr << "\t\t Note that the path will be taken relative to the path" << endl;
    cerr << "\t\t to the database file" << endl << endl;
}

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

SAL_IMPLEMENT_MAIN_WITH_ARGS(argc, argv)
{
    Db* aDatabase= NULL;
    char *db_path= NULL;
    char *code= NULL;
    int c;

    /* modes */
    const char* sanity_check = "sanity";
    const char* stat         = "stat";
    const char* import       = "import";

    while (1)
    {
        static struct option long_options[] =
            {
                {"help",      no_argument,       0, 'h'},
                {"database",  required_argument, 0, 'd'},
                {"code",      required_argument, 0, 'c'},
                {0, 0, 0, 0}
            };

        int option_index = 0;
        
        c = getopt_long (argc, argv, "hd:",
                         long_options, &option_index);
        
        /* Detect the end of the options. */
        if (c == -1)
            break;
        
        switch (c)
        {
            case 'h':
                printUsage(argv[0]);                
                exit(0);
                break;
                
            case 'd':
                std::cerr << "Using database file \"" << optarg << "\"\n";
                db_path= strdup(optarg);
                OSL_ASSERT(db_path);
                break;

            case 'c':
                printf("option -c with value '%s'\n", optarg);
                code= strdup(optarg);
                OSL_ASSERT(code);
                break;
                
            case '?':
                /* getopt_long already printed an error message. */
                break;

            default:
                abort();
        }
    }
    if (db_path == NULL)
    {
        std::cerr << "No database file specified!" << std::endl << std::endl;
        printUsage(argv[0]);
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
            }
            else
            {
                printUsage(argv[0]);
                abort();
            }
        }

        printf ("non-option ARGV-elements: ");
        while (optind < argc)
            printf ("%s ", argv[optind++]);
        putchar ('\n');
    } 
    
    return 0;
}
