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

#define DEBUG_SCSOLVER_RESMGR 0

using namespace ::com::sun::star;
using namespace ::std;

using ::com::sun::star::uno::Any;
using ::com::sun::star::uno::Exception;
using ::com::sun::star::uno::UNO_QUERY;
using ::com::sun::star::uno::UNO_QUERY_THROW;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Sequence;
using ::com::sun::star::uno::XComponentContext;
using ::com::sun::star::deployment::PackageInformationProvider;
using ::com::sun::star::deployment::XPackageInformationProvider;
using ::com::sun::star::io::XInputStream;
using ::com::sun::star::resource::MissingResourceException;
using ::com::sun::star::ucb::XSimpleFileAccess;

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

    // Get all text files with names formatted this way (/<locale name>/*.properties).
    vector<PropertiesFile> files;
    getPropertiesFiles(files);

    vector<PropertiesFile>::const_iterator itr = files.begin(), itrEnd = files.end();
    for (; itr != itrEnd; ++itr)
        loadStrings(*itr);

#if DEBUG_SCSOLVER_RESMGR
    Sequence<OUString> resids = mxStrResMgr->getResourceIDs();
    for (sal_Int32 i = 0; i < resids.getLength(); ++i)
    {
        fprintf(stdout, "StringResMgr::loadStrings: id = '%s'\n",
                OUStringToOString(resids[i], RTL_TEXTENCODING_UTF8).getStr());fflush(stdout);
        OUString str = mxStrResMgr->resolveString(resids[i]);
        fprintf(stdout, "StringResMgr::loadStrings: str = '%s'\n",
                OUStringToOString(str, RTL_TEXTENCODING_UTF8).getStr());fflush(stdout);
    }
#endif    
    lang::Locale en_US;
    en_US.Language = ascii("en");
    en_US.Country = ascii("US");
    en_US.Variant = ascii("");
    mxStrResMgr->setDefaultLocale(en_US);
    mxStrResMgr->setCurrentLocale(getSystemLocale(), true);
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

#if DEBUG_SCSOLVER_RESMGR || 1
    fprintf(stdout, "StringResMgr::getSystemLocale: language = '%s'  country = '%s'\n",
            OUStringToOString(locale.Language, RTL_TEXTENCODING_UTF8).getStr(),
            OUStringToOString(locale.Country, RTL_TEXTENCODING_UTF8).getStr());fflush(stdout);
#endif    

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
        ascii("SolverDialog.Title"),
        // SCSOLVER_STR_DEFINE_MODEL
        ascii("SolverDialog.flModel.Label"),
        // SCSOLVER_STR_SET_TARGET_CELL
        ascii("SolverDialog.ftTargetCell.Label"),
        // SCSOLVER_STR_GOAL
        ascii("SolverDialog.ftObj.Label"),
        // SCSOLVER_STR_MAXIMIZE
        ascii("SolverDialog.rbMax.Label"),
        // SCSOLVER_STR_MINIMIZE
        ascii("SolverDialog.rbMin.Label"),
        // SCSOLVER_STR_DECISIONVAR_CELLS
        ascii("SolverDialog.ftDecVars.Label"),
        // SCSOLVER_STR_CONSTRAINT_SEP
        ascii("SolverDialog.flConstraints.Label"),
        // SCSOLVER_STR_CONSTRAINTDLG_TITLE
        ascii("ConstEditDialog.Title"),
        // SCSOLVER_STR_CELL_REFERENCE
        ascii("ConstEditDialog.ftLeft.Label"),
        // SCSOLVER_STR_CONSTRAINT
        ascii("ConstEditDialog.ftRight.Label"),
        // SCSOLVER_STR_BTN_OK
        ascii("Common.OK.Label"),
        // SCSOLVER_STR_BTN_CANCEL
        ascii("Common.Cancel.Label"),
        // SCSOLVER_STR_MSG_REF_CON_RANGE_MISMATCH
        ascii("Common.ConstRangeMismatch.Label"),
        // SCSOLVER_STR_BTN_ADD
        ascii("SolverDialog.btnConstAdd.Label"),
        // SCSOLVER_STR_BTN_CHANGE
        ascii("SolverDialog.btnConstChange.Label"),
        // SCSOLVER_STR_BTN_DELETE
        ascii("SolverDialog.btnConstDelete.Label"),
        // SCSOLVER_STR_BTN_SOLVE
        ascii("SolverDialog.btnSolve.Label"),
        // SCSOLVER_STR_BTN_RESET
        ascii("SolverDialog.btnReset.Label"),
        // SCSOLVER_STR_BTN_OPTIONS
        ascii("SolverDialog.btnOptions.Label"),
        // SCSOLVER_STR_BTN_SAVE_MODEL
        ascii("SolverDialog.btnSave.Label"),
        // SCSOLVER_STR_BTN_LOAD_MODEL
        ascii("SolverDialog.btnLoad.Label"),
        // SCSOLVER_STR_BTN_CLOSE
        ascii("SolverDialog.btnClose.Label"),
        // SCSOLVER_STR_MSG_SOLUTION_NOT_FOUND
        ascii("Common.SolutionNotFound.Label"),
        // SCSOLVER_STR_MSG_SOLUTION_FOUND
        ascii("Common.SolutionFound.Label"),
        // SCSOLVER_STR_MSG_CELL_GEOMETRIES_DIFFER
        ascii("Common.CellGeometriesDiffer.Label"),
        // SCSOLVER_STR_MSG_MAX_ITERATION_REACHED
        ascii("Common.MaxIterationReached.Label"),
        // SCSOLVER_STR_MSG_STD_EXCEPTION_CAUGHT
        ascii("Common.StdException.Label"),
        // SCSOLVER_STR_MSG_ITERATION_TIMED_OUT
        ascii("Common.IterationTimedOut.Label"),
        // SCSOLVER_STR_MSG_GOAL_NOT_SET
        ascii("Common.GoalNotSet.Label"),
        // SCSOLVER_STR_OPTIONDLG_TITLE
        ascii("OptionDialog.Title"),
        // SCSOLVER_STR_OPTION_ASSUME_LINEAR
        ascii("OptionDialog.cbLinear.Label"),
        // SCSOLVER_STR_OPTION_VAR_POSITIVE
        ascii("OptionDialog.cbPositiveValue.Label"),
        // SCSOLVER_STR_OPTION_VAR_INTEGER
        ascii("OptionDialog.cbIntegerValue.Label")
    };

    if (resid - SCSOLVER_RES_START >= sizeof(resNameList)/sizeof(resNameList[0]))
        return ascii("");

    return resNameList[resid - SCSOLVER_RES_START];
}

void StringResMgr::loadStrings(const PropertiesFile& propFile)
{
    Reference<ucb::XSimpleFileAccess> xFileAccess = getSimpleFileAccess();

    if (!xFileAccess.is())
        return;

    if (!xFileAccess->exists(propFile.FilePath))
        // file does not exist.
        return;

    Reference<XInputStream> xInStrm = xFileAccess->openFileRead(propFile.FilePath);
    if (!xInStrm.is())
        // The input stream is empty.  Bail out.
        return;

    sal_Int32 fileSize = xFileAccess->getSize(propFile.FilePath);
    Sequence<sal_Int8> bytes;
    xInStrm->readBytes(bytes, fileSize);
    vector<Entry> entries;
    parsePropertiesStream(bytes, entries);
    try
    {
        mxStrResMgr->newLocale(propFile.Locale);
    }
    catch(const container::ElementExistException&)
    {
    }
    mxStrResMgr->setCurrentLocale(propFile.Locale, false);
    vector<Entry>::const_iterator itr = entries.begin(), itrEnd = entries.end();
    for (; itr != itrEnd; ++itr)
    {
#if DEBUG_SCSOLVER_RESMGR        
        fprintf(stdout, "StringResMgr::loadStrings: '%s' = '%s'\n",
                OUStringToOString(itr->Name, RTL_TEXTENCODING_UTF8).getStr(),
                OUStringToOString(itr->Value, RTL_TEXTENCODING_UTF8).getStr());fflush(stdout);
#endif        
        mxStrResMgr->setString(itr->Name, itr->Value);
    }
}

void StringResMgr::getPropertiesFiles(vector<PropertiesFile>& files)
{
    Reference<XSimpleFileAccess> xFileAccess = getSimpleFileAccess();
    if (!xFileAccess.is())
        return;

    Sequence<OUString> alldirs = xFileAccess->getFolderContents(msBaseTransDirPath, true);
    sal_Int32 dirCount = alldirs.getLength();
    if (!dirCount)
        return;

    const sal_Int32 beginPos = msBaseTransDirPath.getLength();

    const sal_Unicode dash   = sal_Unicode('-');
    const sal_Unicode a = sal_Unicode('a');
    const sal_Unicode z = sal_Unicode('z');
    const sal_Unicode A = sal_Unicode('A');
    const sal_Unicode Z = sal_Unicode('Z');

    const OUString ext = ascii(".properties");

    files.clear();
    for (sal_Int32 i = 0; i < dirCount; ++i)
    {
        if (!xFileAccess->isFolder(alldirs[i]))
            continue;

        // Parse the directory name to make sure it's a valid locale name.

        const sal_Unicode* chars = alldirs[i].getStr();
        sal_Int32 strSize = alldirs[i].getLength();
        OUStringBuffer buf;
        vector<OUString> names;
        lang::Locale locale;
        bool validName = true;
        for (sal_Int32 j = beginPos; j < strSize; ++j)
        {
            const sal_Unicode c = chars[j];
            if (c == dash)
            {
                if (!buf.getLength())
                {
                    validName = false;
                    break;
                }

                if (!locale.Language.getLength())
                    locale.Language = buf.makeStringAndClear();
                else if (!locale.Country.getLength())
                    locale.Country = buf.makeStringAndClear();
                else if (!locale.Variant.getLength())
                    locale.Variant = buf.makeStringAndClear();
                else
                {
                    validName = false;
                    break;
                }
            }
            else if ( !((a <= c && c <= z) || (A <= c && c <= Z)) )
            {
                // only alphabets are allowed.
                validName = false;
                break;
            }
            else
                buf.append(c);
        }

        if (buf.getLength())
        {
            if (!locale.Language.getLength())
                locale.Language = buf.makeStringAndClear();
            else if (!locale.Country.getLength())
                locale.Country = buf.makeStringAndClear();
            else if (!locale.Variant.getLength())
                locale.Variant = buf.makeStringAndClear();
            else
                validName = false;
        }

        if (!validName)
            continue;

#if DEBUG_SCSOLVER_RESMGR        
        fprintf(stdout, "StringResMgr::getPropertiesFiles: locale '%s' '%s' '%s'\n",
                OUStringToOString(locale.Language, RTL_TEXTENCODING_UTF8).getStr(),
                OUStringToOString(locale.Country, RTL_TEXTENCODING_UTF8).getStr(),
                OUStringToOString(locale.Variant, RTL_TEXTENCODING_UTF8).getStr());
        fflush(stdout);
#endif        

        // Pick up all *.properties files.
        Sequence<OUString> allfiles = xFileAccess->getFolderContents(alldirs[i], false);
        sal_Int32 fileCount = allfiles.getLength();
        for (sal_Int32 j = 0; j < fileCount; ++j)
        {
            sal_Int32 extPos = allfiles[j].indexOf(ext);
            if (extPos < 0 || extPos != allfiles[j].getLength() - ext.getLength())
                // the desired extension not found.  skip it.
                continue;

            PropertiesFile file;
            file.FilePath = allfiles[j];
            file.Locale = locale;
            files.push_back(file);
        }
    }
}

void StringResMgr::parsePropertiesStream(const Sequence<sal_Int8>& bytes, 
                                         vector<Entry>& rEntries)
{
    PropStreamParser parser(bytes);
    parser.parse();
    parser.getEntries(rEntries);
    
}

Reference<ucb::XSimpleFileAccess> StringResMgr::getSimpleFileAccess()
{
    if (!mxFileAccess.is())
    {
        Reference<lang::XMultiComponentFactory> xFactory = mpCalc->getServiceManager();
        mxFileAccess.set( 
            xFactory->createInstanceWithContext(
                ascii("com.sun.star.ucb.SimpleFileAccess"),
                mpCalc->getComponentContext() ), 
            UNO_QUERY );
    }

    return mxFileAccess;
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
    bool inNumericID = true;
    for (sal_Int32 i = 0; i < size; ++i)
    {
        switch (mrBytes[i])
        {
            case '#':
                advanceToLinefeed(i);
            case 0x0A: // linefeed
                purgeBuffer(value, buf);
                pushEntry(name, value);
                inRHS = false;
                inNumericID = true;
                name = OUString();
            break;
            case '=':
                if (inRHS)
                    buf.push_back(mrBytes[i]);
                else
                {
                    inRHS = true;
                    purgeBuffer(name, buf);
                }
            break;
            case '.':
                if (inNumericID)
                    // First dot is encountered.
                    inNumericID = false;
                else
                    buf.push_back(mrBytes[i]);
            break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                if (!inNumericID)
                    buf.push_back(mrBytes[i]);
            break;
            default:
                buf.push_back(mrBytes[i]);
                inNumericID = false;
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
