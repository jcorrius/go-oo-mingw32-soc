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
#ifndef CONFIGMGR_DBBE_UTIL_GRAPH_HXX_
#define CONFIGMGR_DBBE_UTIL_GRAPH_HXX_

#include "../Record.hxx" //FIXME

#include <hash_set>
#include <hash_map>
#include <iostream>
#ifdef SYSTEM_DB
#include <db_cxx.h>
#else
#include <berkeleydb/db_cxx.h>
#endif

namespace configmgr 
{
    namespace dbbe
    {
        class graph
        {
        private:
            struct eqstr
            {
                bool operator()(const char* s1, const char* s2) const
                    {
                        return strcmp(s1, s2) == 0;
                    }
            };

        public:
            typedef std::hash_map<const sal_Char*, configmgr::dbbe::Record, std::hash<const sal_Char*>, eqstr> nodeHash;
            typedef std::hash_set<sal_Char*, std::hash<sal_Char*>, eqstr> keySet;
//            typedef std::hash_map<sal_Char*, configmgr::dbbe::Record, std::hash<sal_Char*>, eqstr> nodeHash;
//            typedef std::hash_set<sal_Char*, std::hash<sal_Char*>, eqstr> keySet;


            graph(Db& aDatabase);
            void traverse(void callback(sal_Char *, const Record&, void*),
                          void* state);
            void traverse(void callback(sal_Char *, const Record&, void*),
                          keySet::iterator it,
                          void* state);
            void graphviz(std::ostream &out);
            
            keySet nodes, orphans, children;
            nodeHash NodeHash;
        private:
            graph();
            void descend(void callback(sal_Char *, const Record&, void*), 
                         nodeHash::iterator it,
                         void *state);
        };
    };
};


#endif
