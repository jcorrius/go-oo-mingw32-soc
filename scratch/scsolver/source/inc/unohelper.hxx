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


#ifndef _UNOHELPER_HXX_
#define _UNOHELPER_HXX_
 
#include <unoglobal.hxx>

using namespace ::com::sun::star;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::XInterface;

namespace scsolver { namespace unohelper {

uno::Any getPropertyValue( const Reference< XInterface >&, const rtl::OUString& );
void getPropertyValue( const Reference< XInterface >&, const rtl::OUString&, sal_Int16& );
void setPropertyValue( const Reference< uno::XInterface >&, const rtl::OUString&, const sal_Int32 );
void setPropertyValue( const Reference< XInterface >&, const rtl::OUString&, const uno::Any& );

}}
 
 
#endif
