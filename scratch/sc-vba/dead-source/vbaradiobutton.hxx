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
#ifndef SC_VBA_RADIOBUTTON_HXX
#define SC_VBA_RADIOBUTTON_HXX
#include <cppuhelper/implbase1.hxx>
#include <org/openoffice/msforms/XRadioButton.hpp>

#include "vbacontrol.hxx"
#include "vbahelper.hxx"

typedef cppu::ImplInheritanceHelper1< ScVbaControl, oo::msforms::XRadioButton > RadioButtonImpl_BASE;

class ScVbaRadioButton : public RadioButtonImpl_BASE
{
public:
    ScVbaRadioButton( const css::uno::Reference< css::uno::XComponentContext >& xContext,
                    const css::uno::Reference< css::drawing::XControlShape >& xControlShape );
   // Attributes
    virtual rtl::OUString SAL_CALL getCaption() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setCaption( const rtl::OUString& _caption ) throw (css::uno::RuntimeException);
    virtual sal_Bool SAL_CALL getValue() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setValue( sal_Bool _value ) throw (css::uno::RuntimeException);

};
#endif //SC_VBA_RADIOBUTTON_HXX
