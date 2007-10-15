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
#include <com/sun/star/lang/Locale.hpp>
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
    init();

    OUString localeName = getSystemLocale();
    if (!localeName.getLength())
        // falls back to en-US in case the system locale is unknown.
        localeName = ascii("en-US");

    OUStringBuffer filePathBuf(msBaseTransDirPath);
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

    lang::Locale en_US;
    en_US.Language = ascii("en");
    en_US.Country = ascii("US");
    en_US.Variant = ascii("");
    loadStrings(ascii("SolverDialog"), en_US);

    lang::Locale ja_JP;
    ja_JP.Language = ascii("ja");
    ja_JP.Country = ascii("JP");
    ja_JP.Variant = ascii("");
    loadStrings(ascii("SolverDialog"), ja_JP);

    Sequence<OUString> resids = mxStrResMgr->getResourceIDs();
    for (sal_Int32 i = 0; i < resids.getLength(); ++i)
    {
        fprintf(stdout, "StringResMgr::loadStrings: id = '%s'\n",
                OUStringToOString(resids[i], RTL_TEXTENCODING_UTF8).getStr());fflush(stdout);
        OUString str = mxStrResMgr->resolveString(resids[i]);
        fprintf(stdout, "StringResMgr::loadStrings: str = '%s'\n",
                OUStringToOString(str, RTL_TEXTENCODING_UTF8).getStr());fflush(stdout);
    }
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

void StringResMgr::init()
{
    fprintf(stdout, "StringResMgr::init: --begins\n");fflush(stdout);

    // Get the base directory path where the translation files are stored.

    Reference<XPackageInformationProvider> xPkgInfo = PackageInformationProvider::get(
        mpCalc->getComponentContext() );

    OUStringBuffer filePathBuf( xPkgInfo->getPackageLocation(ascii("org.go-oo.CalcSolver")) );
    filePathBuf.appendAscii("/translation/");
    msBaseTransDirPath = filePathBuf.makeStringAndClear();

    // Initialize the UNO string resource manager.

    Reference<lang::XMultiComponentFactory> xFactory = mpCalc->getServiceManager();

    mxStrResMgr.set(
        xFactory->createInstanceWithContext( 
            ascii("com.sun.star.resource.StringResource"),
            mpCalc->getComponentContext() ), 
        UNO_QUERY );

    if (!mxStrResMgr.is())
        return;

#if 0    
    lang::Locale en_US;
    en_US.Language = ascii("en");
    en_US.Country = ascii("US");
    en_US.Variant = ascii("");
    mxStrResMgr->newLocale(en_US);
    mxStrResMgr->setDefaultLocale(en_US);
    mxStrResMgr->setCurrentLocale(en_US, false);
    mxStrResMgr->setString( ascii("foo"), ascii("I am foo") );

    OUString foo = mxStrResMgr->resolveString( ascii("foo") );
    fprintf(stdout, "StringResMgr::init: foo = '%s'\n",
            OUStringToOString(foo, RTL_TEXTENCODING_UTF8).getStr());fflush(stdout);
#endif    
}

void StringResMgr::loadStrings(const OUString& dialogName, const lang::Locale& locale)
{
    // Construct the file path.

    OUStringBuffer buf(msBaseTransDirPath);
    buf.append(dialogName);
    do
    {
        // language
        if (!locale.Language.getLength())
            break;
        buf.appendAscii("_");
        buf.append(locale.Language);

        // country
        if (!locale.Country.getLength())
            break;
        buf.appendAscii("_");
        buf.append(locale.Country);

        // variant
        if (!locale.Variant.getLength())
            break;
        buf.appendAscii("_");
        buf.append(locale.Variant);
    }
    while (false);

    buf.appendAscii(".properties");
    OUString filePath = buf.makeStringAndClear();
    fprintf(stdout, "StringResMgr::loadStrings: %s\n",
            OUStringToOString(filePath, RTL_TEXTENCODING_UTF8).getStr());


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
    vector<Entry> entries;
    parsePropertiesStream(bytes, entries);
    mxStrResMgr->newLocale(locale);
    vector<Entry>::const_iterator itr = entries.begin(), itrEnd = entries.end();
    for (; itr != itrEnd; ++itr)
    {
        fprintf(stdout, "StringResMgr::loadStrings: '%s' = '%s'\n",
                OUStringToOString(itr->Name, RTL_TEXTENCODING_UTF8).getStr(),
                OUStringToOString(itr->Value, RTL_TEXTENCODING_UTF8).getStr());fflush(stdout);
        mxStrResMgr->setString(itr->Name, itr->Value);
    }
}

void StringResMgr::parsePropertiesStream(const Sequence<sal_Int8>& bytes, 
                                         vector<Entry>& rEntries)
{
    PropStreamParser parser(bytes);
    parser.parse();
    parser.getEntries(rEntries);
    
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

// ---------------------------------------------------------------------------

PropStreamParser::PropStreamParser(const Sequence<sal_Int8>& bytes) :
    mrBytes(bytes)
{
}

PropStreamParser::~PropStreamParser()
{
}

void PropStreamParser::parse()
{
    sal_Int32 size = mrBytes.getLength();
    vector<sal_Char> buf;
    OUString name, value;
    buf.reserve(80);
    bool inRHS = false;
    for (sal_Int32 i = 0; i < size; ++i)
    {
        switch (mrBytes[i])
        {
            case 0x23: // '#'
                advanceToLinefeed(i);
            case 0x0a: // linefeed
                purgeBuffer(value, buf);
                pushEntry(name, value);
                inRHS = false;
            break;
            case 0x3D: // '='
                if (inRHS)
                {
                    buf.push_back(mrBytes[i]);
                    break;
                }
                inRHS = true;
                purgeBuffer(name, buf);
            break;
            default:
                buf.push_back(mrBytes[i]);
            break;
        }
    }
}

void PropStreamParser::getEntries(vector<StringResMgr::Entry>& rEntries) const
{
    vector<StringResMgr::Entry> entries(mEntries.begin(), mEntries.end());
    rEntries.swap(entries);
}

void PropStreamParser::advanceToLinefeed(sal_Int32& i) const
{
    sal_Int32 size = mrBytes.getLength();
    for (; i < size; ++i)
        if (mrBytes[i] == 0x0a)
            return;
}

void PropStreamParser::purgeBuffer(OUString& rValue, vector<sal_Char>& rBuf) const
{
    if (rBuf.empty())
        rValue = OUString();
    else
    {
        const sal_Char* p = &rBuf[0];
        OUString _value(p, rBuf.size(), RTL_TEXTENCODING_UTF8);
        rBuf.clear();
        rValue = _value;
    }
}

void PropStreamParser::pushEntry(const OUString& name, const OUString& value)
{
    if (!name.getLength())
        return;

    StringResMgr::Entry entry;
    entry.Name = name.trim();
    entry.Value = value.trim();
    mEntries.push_back(entry);
}

}
