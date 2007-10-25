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
#include "scsolver.hrc"

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
using ::com::sun::star::resource::MissingResourceException;

using ::rtl::OUString;
using ::rtl::OUStringBuffer;

namespace scsolver {

StringResMgr::StringResMgr(CalcInterface* pCalc) :
    mpCalc(pCalc),
    mbStringLoaded(false)
{
}

StringResMgr::~StringResMgr()
{
}

void StringResMgr::loadStrings()
{
    init();

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
    mxStrResMgr->setDefaultLocale(en_US);
}

const OUString StringResMgr::getSystemLocaleString() const
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

const lang::Locale StringResMgr::getSystemLocale() const
{
    lang::Locale locale;

    OUString localeStr = getSystemLocaleString();
    sal_Int32 n = localeStr.getLength();
    const sal_Unicode* chars = localeStr.getStr();
    OUStringBuffer buf;
    for (sal_Int32 i = 0; i < n; ++i)
    {
        if (i == 2)
            // Ignore the 3rd character ('_').
            continue;

        const sal_Unicode c = chars[i];
        buf.append(c);
        if (i == 1)
            locale.Language = buf.makeStringAndClear();
        else if (i == 4)
        {
            locale.Country = buf.makeStringAndClear();
            break;
        }
    }

    fprintf(stdout, "StringResMgr::getSystemLocale: language = '%s'  country = '%s'\n",
            OUStringToOString(locale.Language, RTL_TEXTENCODING_UTF8).getStr(),
            OUStringToOString(locale.Country, RTL_TEXTENCODING_UTF8).getStr());fflush(stdout);

    return locale;
}

const OUString StringResMgr::getLocaleStr(int resid)
{
    return getLocaleStr(getResNameByID(resid));
}

const OUString StringResMgr::getLocaleStr(const OUString& resName)
{
    if (!mbStringLoaded)
    {
        loadStrings();
        mbStringLoaded = true;
    }

    if (!mxStrResMgr.is() || !resName.getLength())
        return ascii("(empty)");

    mxStrResMgr->setCurrentLocale(getSystemLocale(), true);

    try
    {
        return mxStrResMgr->resolveString(resName);
    }
    catch (const MissingResourceException&)
    {
    }

    return ascii("(missing)");
}

void StringResMgr::init()
{
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
}

OUString StringResMgr::getResNameByID(int resid)
{
    static const OUString resNameList[] = {
        // SCSOLVER_STR_MAINDLG_TITLE
        ascii("1.SolverDialog.Title"),
        // SCSOLVER_STR_DEFINE_MODEL
        ascii("3.SolverDialog.flModel.Label"),
        // SCSOLVER_STR_SET_TARGET_CELL
        ascii("5.SolverDialog.ftTargetCell.Label"),
        // SCSOLVER_STR_GOAL
        ascii("7.SolverDialog.ftObj.Label"),
        // SCSOLVER_STR_MAXIMIZE
        ascii("9.SolverDialog.rbMax.Label"),
        // SCSOLVER_STR_MINIMIZE
        ascii("11.SolverDialog.rbMin.Label")
        // SCSOLVER_STR_DECISIONVAR_CELLS
        // SCSOLVER_STR_CONSTRAINT_SEP
        // SCSOLVER_STR_CONSTRAINTDLG_TITLE
        // SCSOLVER_STR_CELL_REFERENCE
        // SCSOLVER_STR_CONSTRAINT
        // SCSOLVER_STR_BTN_OK
        // SCSOLVER_STR_BTN_CANCEL
        // SCSOLVER_STR_MSG_REF_CON_RANGE_MISMATCH
        // SCSOLVER_STR_BTN_ADD
        // SCSOLVER_STR_BTN_CHANGE
        // SCSOLVER_STR_BTN_DELETE
        // SCSOLVER_STR_BTN_SOLVE
        // SCSOLVER_STR_BTN_RESET
        // SCSOLVER_STR_BTN_OPTIONS
        // SCSOLVER_STR_BTN_SAVE_MODEL
        // SCSOLVER_STR_BTN_LOAD_MODEL
        // SCSOLVER_STR_BTN_CLOSE
        // SCSOLVER_STR_MSG_SOLUTION_NOT_FOUND
        // SCSOLVER_STR_MSG_SOLUTION_FOUND
        // SCSOLVER_STR_MSG_CELL_GEOMETRIES_DIFFER
        // SCSOLVER_STR_MSG_MAX_ITERATION_REACHED
        // SCSOLVER_STR_MSG_STD_EXCEPTION_CAUGHT
        // SCSOLVER_STR_MSG_ITERATION_TIMED_OUT
        // SCSOLVER_STR_MSG_GOAL_NOT_SET
        // SCSOLVER_STR_OPTIONDLG_TITLE
        // SCSOLVER_STR_OPTION_ASSUME_LINEAR
        // SCSOLVER_STR_OPTION_VAR_POSITIVE
        // SCSOLVER_STR_OPTION_VAR_INTEGER
    };

    if (resid - SCSOLVER_RES_START >= sizeof(resNameList)/sizeof(resNameList[0]))
        return ascii("");

    return resNameList[resid - SCSOLVER_RES_START];
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
    mxStrResMgr->setCurrentLocale(locale, false);
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
                name = OUString();
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
