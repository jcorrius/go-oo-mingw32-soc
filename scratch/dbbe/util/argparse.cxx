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
#include "argparse.hxx"
#endif//CONFIGMGR_DBBE_UTIL_ARGPARSE_HXX_


#include <iostream>

#include <osl/diagnose.h>

#include <strings.h>


argParser::argParser(int Argc, char** Argv, arg* Args, const char* Usage):
    argc(Argc),
    argv(Argv),
    args(Args),
    usage(Usage)
{
    numArgs= 0;
    arg* pArg= args;
    OSL_ASSERT(pArg);
    while (pArg->argName)
    {
        numArgs++;
        pArg++;
    }
    for (int i= 0; i < numArgs; i++)
    {
        OSL_ASSERT(args[i].flagStore);
        *(args[i].flagStore) = false;
    }
}

argParser::~argParser()
{    
}

void argParser::printUsage(const char* header, const char* footer) const
{
    using namespace std;
    
    if (header)
        cerr << header << endl;
    
    cerr << "Usage: " << argv[0] << " ";
    if (usage)
    {
        cerr << usage << endl;
    }
    else
    {
        cerr << "[options]" << endl;
    }
    
    cerr << "Mandatory arguments:" << endl;
    for (int i= 0; i < numArgs; i++)
    {
        arg* p= &(args[i]);
        if (p->optional == false)
        {
            cerr << "  -" << p->shortName << ", --" << p->argName;
            cerr << "\t" << p->usageString << endl;
        }
    }
    cerr << endl;
    cerr << "Optional arguments:" << endl;
    for (int i= 0; i < numArgs; i++)
    {
        arg* p= args + i*sizeof(arg);
        if (p->optional == true)
        {
            cerr << "  -" << p->shortName << ", --" << p->argName;
            cerr << "\t" << p->usageString << endl;
        }
    }
    if (footer)
        cerr << endl << footer;
}

arg* argParser::argMatch(char *name) const
{
    OSL_ASSERT(name);
    if (strlen(name) < 2)
        return NULL;
    if (name[0] != '-')
        return NULL;        
    for (int i= 0; i < numArgs; i++)
    {
        if (name[1] != '-')
        { //short option
            if (args[i].shortName == name[1])
                return &(args[i]);
        }
        else
        { //long option
            if (!name[2])
                return NULL;
            char *equals= index(name + 2, '=');
            size_t len;
            if (equals)
            {
                len= equals - (name + 2) - 1;
            }
            else
            {
                len= strlen(name + 2);
            }
            if (!strncmp(args[i].argName, name + 2, len))
                return &(args[i]);
        }
    }
    return NULL;
}

int argParser::parseArgs()
{
    if (argc == 1)
    {
        return 0;
    }
    
    int optind= 1;
    while (optind < argc)
    {
        arg* match= argMatch(argv[optind]);
        if (match)
        {
            OSL_ASSERT(match->flagStore);
            *(match->flagStore)= true;
            if (match->hasValue)
            {
                char *equals= index(argv[optind], '=');
                if (equals)
                {
                    OSL_ASSERT(match->valueStore);
                    *(match->valueStore)= strdup(equals + 1);
                    OSL_ASSERT(match->valueStore);
                }
            }
            optind++;
        }
        else
            return optind;
    }
    return optind;
}

bool argParser::mandatoryOptsSet() const
{
    for (int i= 0; i < numArgs; i++)
    {
        if (args[i].optional == false)
        {
            OSL_ASSERT(args[i].flagStore);
            if (*(args[i].flagStore) == false)
            {
                return false;
            }
        }
    }
    return true;
}
