/*************************************************************************
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Kohei Yoshida.
 *    1039 Kingsway Dr., Apex, NC 27502, USA
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

#include "resmgr.hxx"
#include "xcalc.hxx"
#include "unoglobal.hxx"
#include "tool/global.hxx"
#include "solver.hxx"

#include "rtl/ustrbuf.hxx"

#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/deployment/PackageInformationProvider.hpp>
#include <com/sun/star/deployment/XPackageInformationProvider.hpp>
#include <com/sun/star/ucb/XSimpleFileAccess.hpp>
#include <com/sun/star/io/XInputStream.hpp>

#include <vector>
#include <stdio.h>

using namespace ::com::sun::star;
using namespace ::std;

using ::com::sun::star::uno::Any;
using ::com::sun::star::uno::UNO_QUERY;
using ::com::sun::star::uno::UNO_QUERY_THROW;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Sequence;
using ::com::sun::star::uno::XComponentContext;
using ::com::sun::star::deployment::PackageInformationProvider;
using ::com::sun::star::deployment::XPackageInformationProvider;
using ::com::sun::star::io::XInputStream;

using ::rtl::OUString;
using ::rtl::OUStringBuffer;

namespace scsolver {

StringResMgr::StringResMgr(CalcInterface* pCalc) :
    mpCalc(pCalc)
{
}

StringResMgr::~StringResMgr()
{
}

void StringResMgr::loadStrings()
{
    Reference<XPackageInformationProvider> xPkgInfo = PackageInformationProvider::get(
        mpCalc->getComponentContext() );

    OUString localeName = getSystemLocale();
    if (!localeName.getLength())
        // falls back to en-US in case the system locale is unknown.
        localeName = ascii("en-US");

    OUStringBuffer filePathBuf( xPkgInfo->getPackageLocation(ascii("org.go-oo.CalcSolver")) );
    filePathBuf.appendAscii("/translation/");
    filePathBuf.append(localeName);
    filePathBuf.appendAscii(".txt");
    OUString filePath = filePathBuf.makeStringAndClear();

    Reference<lang::XMultiComponentFactory> xFactory = mpCalc->getServiceManager();

    Reference<ucb::XSimpleFileAccess> xFileAccess(
        xFactory->createInstanceWithContext( 
            ascii("com.sun.star.ucb.SimpleFileAccess"),
            mpCalc->getComponentContext() ), 
        UNO_QUERY );

    if (!xFileAccess.is())
        return;

    if (!xFileAccess->exists(filePath))
        // file does not exist.
        return;

    Reference<XInputStream> xInStrm = xFileAccess->openFileRead(filePath);
    if (!xInStrm.is())
        // The input stream is empty.  Bail out.
        return;

    sal_Int32 fileSize = xFileAccess->getSize(filePath);
    Sequence<sal_Int8> bytes;
    xInStrm->readBytes(bytes, fileSize);
    parseStream(bytes);
}

const OUString StringResMgr::getSystemLocale() const
{
    Reference<lang::XMultiComponentFactory> xFactory = mpCalc->getServiceManager();

    try
    {
        Reference<lang::XMultiServiceFactory> xConfig(
            xFactory->createInstanceWithContext(
                ascii("com.sun.star.configuration.ConfigurationProvider"),
                mpCalc->getComponentContext() ),
            UNO_QUERY_THROW );
    
        beans::PropertyValue prop;
        prop.Name = ascii("nodepath");
        prop.Value = asciiAny("/org.openoffice.Setup/L10N");
        Any any;
        any <<= prop;
        Sequence<Any> props(1);
        props[0] = any;
        Reference<container::XNameAccess> xNA(
            xConfig->createInstanceWithArguments(
                ascii("com.sun.star.configuration.ConfigurationAccess"), props),
            UNO_QUERY_THROW );
    
        any = xNA->getByName(ascii("ooLocale"));
        OUString localeName;
        if (any >>= localeName)
            // successfully obtained the system locale name!
            return localeName;
    }
    catch (const Exception&)
    {
        Debug("exception caught during locale query.");
    }

    return OUString();
}

void StringResMgr::parseStream(const Sequence<sal_Int8>& bytes)
{
    vector<sal_Char> buf;
    sal_Int32 fileSize = bytes.getLength();
    for (sal_Int32 i = 0; i < fileSize; ++i)
    {
        if (bytes[i] == 0x0a)
        {
            if (!buf.empty())
            {
                const sal_Char* p = &buf[0];
                OUString line(p, buf.size(), RTL_TEXTENCODING_UTF8);
                buf.clear();
                fprintf(stdout, "'%s'\n",
                        OUStringToOString(line, RTL_TEXTENCODING_UTF8).getStr());
            }
        }
        else
            buf.push_back(bytes[i]);
    }
    fflush(stdout);
}

}
