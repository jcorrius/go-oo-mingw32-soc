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

#ifndef _SCSOLVER_UI_RESMGR_HXX_
#define _SCSOLVER_UI_RESMGR_HXX_

#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/resource/XStringResourceManager.hpp>
#include "rtl/ustring.hxx"

#include <vector>

namespace com { namespace sun { namespace star { 
    namespace lang {
        struct Locale;
    }
}}}

namespace scsolver {

class CalcInterface;

class StringResMgr
{
public:
    struct Entry
    {
        ::rtl::OUString Name;
        ::rtl::OUString Value;
    };

    StringResMgr(CalcInterface* pCalc);
    ~StringResMgr();

    /** 
     * Load locale strings from embedded translation file based on current
     * system locale.
     */
    void loadStrings();

    /** 
     * Get current system locale. 
     * 
     * @return const::rtl::OUString current system locale, or empty string if 
     *         it is unknown.
     */
    const ::rtl::OUString getSystemLocale() const;

private:
    void init();
    void loadStrings(const ::rtl::OUString& dialogName, const ::com::sun::star::lang::Locale& locale);
    void parsePropertiesStream(const ::com::sun::star::uno::Sequence<sal_Int8>& bytes,
                               ::std::vector<Entry>& rEntries);
    void parseStream(const ::com::sun::star::uno::Sequence<sal_Int8>& bytes);

private:
    ::com::sun::star::uno::Reference< ::com::sun::star::resource::XStringResourceManager >
        mxStrResMgr;
    CalcInterface* mpCalc;
    ::rtl::OUString msBaseTransDirPath;
};

// ---------------------------------------------------------------------------

class PropStreamParser
{
public:

    PropStreamParser(const ::com::sun::star::uno::Sequence<sal_Int8>& bytes);
    ~PropStreamParser();

    void parse();
    void getEntries(::std::vector<StringResMgr::Entry>& rEntries) const;

private:
    PropStreamParser();

    void advanceToLinefeed(sal_Int32& i) const;
    void purgeBuffer(::rtl::OUString& rValue, ::std::vector<sal_Char>& rBuf) const;
    void pushEntry(const ::rtl::OUString& name, const ::rtl::OUString& value);

private:
    const ::com::sun::star::uno::Sequence<sal_Int8>& mrBytes;
    ::std::vector<StringResMgr::Entry> mEntries;
};










}

#endif
