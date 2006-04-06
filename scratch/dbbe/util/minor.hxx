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
#ifndef CONFIGMGR_DBBE_UTIL_MINOR_HXX_
#define CONFIGMGR_DBBE_UTIL_MINOR_HXX_

#include "../Record.hxx" //awfull!

#include <hash_set>
#include <hash_map>

#include <algorithm>
#include <functional>

#include <db_cxx.h>

/**
   This file contains the routines for the minor modes of the utility:
     sanity_check
     view
 */

namespace configmgr
{
    namespace dbbe 
    {
        /**
           Do a basic sanity check on the database

           @param aDatabase an open database to look at

           @return  True for sane or False for insane
        */
        sal_Bool SanityCheck(Db& aDatabase);

        /**
           Print some basic statistics about the database
           and optionally draw a graph of the keys
           
           @param aDatabase   an open database to look at
           @param GraphUrl    URL for graph output (dot format), if empty then supressed
        */
        void Stat(Db& aDatabase, const sal_Char* graphFile);
        
    }; //dbbe
}; //configmgr
#endif//CONFIGMGR_DBBE_UTIL_MINOR_HXX_
