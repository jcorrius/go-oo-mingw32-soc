#include "sal/main.h"
#include "sal/types.h"

#include <db_cxx.h>
#include <cstdlib>

#include "../Record.hxx" //that's awful!
#include "minor.hxx"


//I use getopt parsing here, which is a POSIX thing
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

void openDatabase(Db* aDatabase, const char* path, const bool open_ro)
{
    u_int32_t open_flags= DB_THREAD;
    OSL_ASSERT(aDatabase == NULL);
    const u_int32_t db_flags= DB_CXX_NO_EXCEPTIONS;
    aDatabase= new Db(NULL, db_flags);
    OSL_ASSERT(aDatabase);
    if (open_ro)
    {
        open_flags |= DB_RDONLY;
    }
    int ret;
    ret= aDatabase->set_lorder(1234); //little endian
    ret= aDatabase->open(NULL, path, NULL,  DB_UNKNOWN, 0, open_flags);
    if (ret)
    { 
        std::cerr << "Unable to open database \"" << path << "\"" << std::endl;
        abort();
    }    
}

SAL_IMPLEMENT_MAIN_WITH_ARGS(argc, argv)
{
    Db* aDatabase= NULL;
    char *db_path= NULL;
    bool db_ro;
    int c;

    /* modes */
    const char* sanity_check = "sanity";
    const char* stat         = "stat";
    
    while (1)
    {
        static struct option long_options[] =
            {
                {"help",     no_argument,       0, 'h'},
                {"database",  required_argument, 0, 'd'},
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
                puts ("option -h\n");
                break;
                
            case 'd':
                printf("option -d with value `%s'\n", optarg);
                db_path= strdup(optarg);
                OSL_ASSERT(db_path);
                break;
                
            case '?':
                /* getopt_long already printed an error message. */
                break;
                
            default:
                abort();
        }
    }
    if (optind < argc)
    {
        if (!strcmp(argv[optind], sanity_check))
        {
            db_ro= true;
            openDatabase(aDatabase, db_path, db_ro);
            if (configmgr::dbbe::SanityCheck(*aDatabase))
            {
                std::cout << "Everything seems normal" << std::endl;
                exit(0);
            }
            else
            {
                std::cerr << "Errors detected in database!" << std::endl;
                exit(1);
            }
            
        }
        else if(!strcmp(argv[optind], stat))
        {
            db_ro= true;
            openDatabase(aDatabase, db_path, db_ro);
            optind++;
            char* graphFile= NULL;
            if (optind < argc)
            {
                graphFile= argv[optind];
            }
            configmgr::dbbe::Stat(*aDatabase, graphFile);
            exit(0);
            
        }
        printf ("non-option ARGV-elements: ");
        while (optind < argc)
            printf ("%s ", argv[optind++]);
        putchar ('\n');
    } 
    
    return 0;
}
