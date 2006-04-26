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
#ifndef CONFIGMGR_DBBE_UTIL_ARGPARSE_HXX_
#define CONFIGMGR_DBBE_UTIL_ARGPARSE_HXX_

#include <iostream>

/**
   a simple struct to specify arguments
   
   @see argParser
 */
struct arg
{
    char*         argName;      //the name of the argument (foo, etc)
    char          shortName;    //the (f in -f, etc)
    bool          optional;     //true if optional false otherwise
    bool          hasValue;     //true if this option has a value (-foo=bar)
    bool*         flagStore;    //true is written here if option is set
    char**        valueStore;   //string is copied here if option is set
    char*         usageString;  //the string that explains this usage
                                //in the printUsage method
};

/**
   a simple argument parser
 */
class argParser
{
public:
    /**
       construtor
       
       @param Argc   C's argc
       @param Argv   C's argv
       @param Args   pointer to array of args, where the last
                     element is all zeros
       @param Usage  a string that gives a summary of what this program does
     */
    argParser(int Argc, char** Argv, arg* Args, const char* Usage= 0);
    ~argParser();
    
    /**
       prints the usage to stderr
     */
    void printUsage(const char* header= NULL, const char* footer= NULL) const;
    
    /**
       parse the arguments

       @note    this function is fairly niave, it assumes
                it is not an error for a mandatory option
                with a value to not specify the value
                also, short options cannot be combined
       
       @return  the index where it stopped parsing
                it eats all the args with --foo -bar, -c and 
                gives you back the next guy to look at:
                progname --foo --bar baz
                will return you 3, b/c you need to figure out what 
                to do with baz
     */
    int parseArgs();

    /**
       check to see if all the mandatory opts are set

       @note     can only be called after parseArgs 
       @return   true if all mandatory options are set false otherwise
    */
    bool mandatoryOptsSet() const;

private:
    argParser();

    arg* argMatch(char *name) const;
    
    int argc;
    char **argv;
    arg* args;
    int numArgs;
    const char* usage;
};

#endif
