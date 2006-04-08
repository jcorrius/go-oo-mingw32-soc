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
#include "major.hxx"

namespace configmgr 
{
    namespace dbbe 
    {
        mangler::mangler(const char *Code):
            code(Code),
            sep("::"),
            xcu(".xcu")
        {
        }

        mangler::~mangler()
        {
        }

        sal_Int32 mangler::getSegment(const rtl::OString &inString,
                                      rtl::OString &outString,
                                      const sal_Int32 startIndex)
        {
            sal_Int32 end_slash= inString.indexOf('/', startIndex);
            OSL_ASSERT(end_slash > 0);
            rtl::OString aTempString= inString.copy(startIndex, 
                                                    end_slash - 1 - startIndex);
            outString+= aTempString;
            return end_slash + 1;
        }


        void mangler::demangle(char *aPath)
        {
            OSL_ASSERT(aPath);
            sal_Int32 p= 0;
            //eat leading / and .
            while (*aPath && (*aPath == '.' || *aPath == '/'))
            {
                aPath++;
            }
            path= rtl::OString(aPath);
            rtl::OString nameSpace, layer, sublayer;
            
            for (int c= 0; c < strlen(code); c++)
            {
                switch (code[c])
                {
                    case 'n':
                        p= getSegment(path, nameSpace, p);
                        break;
                        
                    case 'l':
                        //this is tricky
                        //if we are the last code,
                        //   just eat everything
                        //else
                        //  keep eating up to the last slash
                        break;
                        
                    case 's':
                        if (code[c+1] == NULL)
                        {//this is the last code -- eat .xcu
                            sublayer= path.copy(p, 
                                                path.getLength() - 
                                                xcu.getLength() - p);
                        }
                        else
                        {//this is not the last code -- go to /
                            p= getSegment(path, sublayer, p);
                        }
                        break;

                    default:
                        std::cerr << "Encountered illegal conversion char: " << code[c] << std::endl;
                        abort();
                        break;
                }
            }
        }
    };
};
