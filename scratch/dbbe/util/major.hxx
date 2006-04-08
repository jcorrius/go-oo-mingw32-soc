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
#ifndef CONFIGMGR_DBBE_UTIL_MAJOR_HXX_
#define CONFIGMGR_DBBE_UTIL_MAJOR_HXX_

#include "../Record.hxx" //awfull!

#include <rtl/string.hxx>

//#include <hash_set>
//#include <hash_map>

//#include <algorithm>
//#include <functional>

//#include <db_cxx.h>


/**
   This file contains the prototypes for the major modes:
   import DB
   import file-tree
   export file-tree
 */

namespace configmgr
{
    namespace dbbe
    {
        /**
           A class that manages the mangling of paths to keys and vice versa
           
           We use a code for mangling/demangling names:
           
           we look at xcu files as:
           /path/.../path/path.xcu
           
           we look at keys as:
           namespace::layer::sublayer (or "children")
           
           we define the codes n, s, and l to be as follows:
           n interprets /path/ to be a namespace
           s interprets /path/ as a "sublayer"
           l is greedy and interprets /path/.../path/ as a layer
        */
        class mangler
        {
        public:
            /** ctor */
            mangler(const char *Code);
            
            /** dtor */
            ~mangler();

            void mangle(const char *key);
            void demangle(char *aPath);
            

            /** make a record from this mangler */
            Record makeRecord();
            
            /** make a file from this mangler */
            void makeFile();
            
        private:
            /**
               takes a path: foo/bar/baz with a start
               index and gives you foo, bar, or baz
               
               @param inString   The input string
               @param outString  The string to append to
               @param startIndex Where to start looking from
               @return           the index after the trailing slash
            */            
            sal_Int32 getSegment(const rtl::OString &inString,
                                 rtl::OString &outString,
                                 const sal_Int32 startIndex);
                                 

            rtl::OString key, path, file, sep, xcu;
            const char *code;
        };


    };
};


#endif
