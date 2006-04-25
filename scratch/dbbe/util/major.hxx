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

#include "graph.hxx"

#include <db_cxx.h>


// ARGHH!!!
// our stuff must transition to OSL file URL's b/c windoze paths are \
// osl file stuff requires absolute URL's argh.




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
            // FIXME: docs
            //  This is total foobared and needs to use OUStrings and URL's and stuff
        public:
            /** ctor */
            mangler(const char *Code);
            
            /** dtor */
            ~mangler();

            void mangle(const char *key);
            void demangle(const rtl::OUString &aUrl, const rtl::OUString &aBaseURL);
            
            const char* getKey(void) const;
            const rtl::OUString& getFileUrl(void) const;

            rtl::OString getNamespace(void) const;
            rtl::OString mangler::getNamespaceKey(void) const;
            
        private:
            rtl::OString key, path, file, sep, xcu;
            rtl::OUString fileUrl;
            const char *code;
        };

        /**
           class that exposes the methods to import/export/merge
         */
        class repository
        {
        public:
            /** ctor */
            repository(Db &aDb, char *aDbPath);

            /**
               Import this file/directory
               
               @param path  file or directory
               @param code  conversion code or NULL if using default rules
             */
            void importPath(const char *path, const char *code);

            /**
               merge the database file given into this one
               
               @param path  a database file
             */
            void mergeDatabase(const char *path);
            

        private:
            Db& aDatabase;
            const char* aDatabasePath;
            //graph mGraph; //perhaps not necessary
            //graph::keySet mChangeList; //perhaps not necessary

            void importPathInnerLoop(const rtl::OUString &aUrl, 
                                     const rtl::OUString &aBaseUrl, 
                                     const char *code);
                

            /**
               Import just this file
               
               @param aURL      the URL of the file to import
               @param aBaseURL  the base URL for path conversion
               @param code      the mangler code

               @see mangler
             */
            void importFile(const rtl::OUString &aURL, const rtl::OUString &aBaseURL, 
                            const char *code);
            
            /**
               Guess which conversion code to use based on conventions in localbe
               
               @param   aRelUrl    a relative URL to look at
               @return  a code string or NULL if it could not guess
            */
            const char* guessCode(const rtl::OUString& aRelUrl) const;
        };
    };
};


#endif
