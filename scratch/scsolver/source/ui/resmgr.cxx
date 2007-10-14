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
#include "solver.hxx"

#include "rtl/ustrbuf.hxx"

#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/deployment/PackageInformationProvider.hpp>
#include <com/sun/star/deployment/XPackageInformationProvider.hpp>
#include <com/sun/star/ucb/XSimpleFileAccess.hpp>
#include <com/sun/star/io/XInputStream.hpp>

#include <stdio.h>

using namespace ::com::sun::star;

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

void StringResMgr::test()
{
    Reference<XPackageInformationProvider> xPkgInfo = PackageInformationProvider::get(
        mpCalc->getComponentContext() );

    OUStringBuffer aPkgRoot( xPkgInfo->getPackageLocation( ascii("org.go-oo.CalcSolver") ) );
    aPkgRoot.appendAscii("/translation/en-US.txt");
    OUString filePath = aPkgRoot.makeStringAndClear();

    Reference<lang::XMultiComponentFactory> xFactory = mpCalc->getServiceManager();

    Reference<ucb::XSimpleFileAccess> xFileAccess(
        xFactory->createInstanceWithContext( 
            ascii("com.sun.star.ucb.SimpleFileAccess"),
            mpCalc->getComponentContext() ), 
        UNO_QUERY );

    if (!xFileAccess.is())
        return;

    if (!xFileAccess->exists(filePath))
    {
        fprintf(stdout, "StringResMgr::test: file does not exist.\n");fflush(stdout);
        return;
    }

    sal_Int32 fileSize = xFileAccess->getSize(filePath);
    fprintf(stdout, "StringResMgr::test: '%s' exists (%ld bytes).\n",
            OUStringToOString(filePath, RTL_TEXTENCODING_UTF8).getStr(),
            fileSize);fflush(stdout);

    Reference<XInputStream> xInStrm = xFileAccess->openFileRead(filePath);
    if (!xInStrm.is())
        return;

    Sequence<sal_Int8> bytes;
    xInStrm->readBytes(bytes, fileSize);

    for (sal_Int32 i = 0; i < fileSize; ++i)
        fprintf(stdout, "0x%.2x", bytes[i]);
    fprintf(stdout, "\n");
}

}
