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
#include "minor.hxx"

#include "graph.hxx"

#include <db_cxx.h>

#include <iostream>
#include <fstream>

namespace configmgr 
{
    namespace dbbe
    {
        
        struct namespace_state
        {
            bool root;
            bool error;
            const char* prev_namespace;
            size_t len;
            
            namespace_state()
                :root(true),
                 error(false),
                 prev_namespace(NULL),
                 len(0)
                {
                }
        };

        static void namespace_callback(const sal_Char* key, const Record& data, void* state)
        {
            using namespace std;
            OSL_ASSERT(state);
            namespace_state* nss= static_cast<namespace_state*>(state);
            
            const char* sep= "::";
            
            if (nss->error)
                return;
            
            if (nss->root) 
            { //we are comparing this ns with the previous one
                if (strncmp(key, nss->prev_namespace, nss->len))
                {//no match
                    nss->error= true;
                    cerr << "Namespace of \"" << key << "\" does not match parent key, \"";
                    cerr << nss->prev_namespace << "\"" << endl;
                }
            }
            else
            { //we are setting the prev ns
                nss->prev_namespace= key;
                char *p= strstr(key, sep);
                if (p == NULL)
                { //no namespace!
                    cerr << "Namespace separator missing from \"" << key << "\"" << endl;
                    nss->error= true;
                }
                else
                {
                    nss->len= p - key;
                }
            }
        }

        struct uniqueness_state
        {
            bool error;
            graph::keySet children;

            uniqueness_state():
                error(false)
                {
                }
        };
        
        static void uniqueness_callback(const sal_Char* key, const Record& data, void* state)
        {
            using namespace std;

            OSL_ASSERT(state);
            uniqueness_state* us= static_cast<uniqueness_state*>(state);
            if (us->error)
                return;

            if ((us->children.insert(key)).second == false)
            {
                cerr << "duplicate paths detected to key \"" << key << "\"" << endl;
            }
        }
        
        sal_Bool SanityCheck(Db& aDatabase)
        {
            using namespace std;
            
            int ret= -1;        //garbage value to keep compiler from complaining
            u_int32_t flags= 0; //garbage value to keep compiler from complaining
            
            //duplicate keys are not allowed
            OSL_ASSERT(!aDatabase.get_flags(&flags));
            if (flags |= DB_DUP)      
            {
                cerr << "Database allows duplicate items!" << endl;
                return sal_False;
            }
            
            //little endian required
            OSL_ASSERT(!aDatabase.get_lorder(&flags));
            if (flags != 1234)
            {
                cerr << "Database is not set to little endian!" << endl;
                return sal_False;
            }
            
            graph myGraph(aDatabase);
            //orphans must meet the following conditions
            //  1) have children (no islands allowed)
            //  2) orphans must have the format namespace::::
            //  3) children of orphans must have the same namespace
            for (graph::keySet::const_iterator it= myGraph.orphans.begin();
                 it != myGraph.orphans.end();
                 it++)
            {
                const sal_Char* doublenamespace= "::::";
                
                const sal_Char *index= strstr(*it, doublenamespace);
                if (index != *it + strlen(*it) - strlen(doublenamespace))
                {
                    cerr << "Key \"" << *it << "\" is an impropererly named root node!" << endl;
                    return sal_False;
                }   
                if (myGraph.NodeHash[*it].numSubLayers == 0)
                {
                    cerr << "Key \"" << *it << "\" is an island!" << endl;
                    return sal_False;
                }
                namespace_state mnss;
                myGraph.traverse(namespace_callback, it, static_cast<void*>(&mnss));
                if (mnss.error)
                {
                    return sal_False;
                }
                uniqueness_state muss;
                myGraph.traverse(uniqueness_callback, it, static_cast<void*>(&muss));
                if (muss.error)
                {
                    return sal_False;
                }
             }
            return sal_True;
        }

        void Stat(Db& aDatabase, const sal_Char* graphFile)
        {
            using namespace std;

            graph myGraph(aDatabase);
            cout << "Database Statistics" << endl;
            cout << "\t Namespaces: " << myGraph.children.size() << endl; //correct STL usage?
            cout << "\t Total Keys: " << myGraph.nodes.size() << endl; //see above
            //perhaps more statistics
            if (graphFile)
            {
                //perhaps this should be moved to the graph object
                ofstream dotFile(graphFile);
                if (!dotFile)
                {
                    cerr << "Unable to open graph output file \"" << graphFile << "\"" << endl;
                    return;
                }
                myGraph.graphviz(dotFile);
            }
        }

    }; //dbbe
}; //configmgr
