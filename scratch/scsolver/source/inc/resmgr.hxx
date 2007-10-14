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

namespace scsolver {

class CalcInterface;

class StringResMgr
{
public:
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
    void parseStream(const ::com::sun::star::uno::Sequence<sal_Int8>& bytes);

private:
    CalcInterface* mpCalc;
};










}

#endif
