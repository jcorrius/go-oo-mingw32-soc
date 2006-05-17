/*************************************************************************
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2006 by Kohei Yoshida.
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

#ifndef _TESTMODEL_HXX_
#define _TESTMODEL_HXX_

#include "cppuhelper/implbase5.hxx"
#include "cppuhelper/implementationentry.hxx"
#include "com/sun/star/lang/XInitialization.hpp"
#include "com/sun/star/frame/XNotifyingDispatch.hpp"
#include "com/sun/star/frame/XDispatchProvider.hpp"
#include "com/sun/star/lang/XServiceInfo.hpp"

#include "org/openoffice/sc/solver/XLpModel.hpp"
#include "org/openoffice/sc/solver/Goal.hpp"
#include "org/openoffice/sc/solver/Bound.hpp"

#include <memory>

#define	SERVICE_NAME		"org.openoffice.sc.solver.TestModel"
#define	IMPLEMENTATION_NAME	"scsolver.TestModelImpl"

using namespace ::com::sun::star;

namespace org { namespace openoffice { namespace sc { namespace solver {
	class XLpModel;
}}}}

namespace scsolver {

struct TestModelData;

class TestModelImpl : public ::cppu::WeakImplHelper5<
	lang::XInitialization,
	lang::XServiceInfo,
	frame::XDispatchProvider,
	frame::XNotifyingDispatch,
	::org::openoffice::sc::solver::XLpModel >
{
public:

	TestModelImpl( const uno::Reference<uno::XComponentContext>& );
	virtual ~TestModelImpl() throw();

	//--------------------------------------------------------------------------
	// UNO Component Interface Methods

	// XInitialization
	virtual void SAL_CALL initialize( const uno::Sequence<uno::Any>& )
		throw ( uno::Exception );

	// XServiceInfo
	virtual rtl::OUString SAL_CALL getImplementationName()
		throw ( uno::RuntimeException );
	virtual sal_Bool SAL_CALL supportsService( const rtl::OUString& )
		throw ( uno::RuntimeException );
	virtual uno::Sequence<rtl::OUString> SAL_CALL getSupportedServiceNames()
		throw ( uno::RuntimeException );

	// XDispatchProvider
	virtual uno::Reference<frame::XDispatch> SAL_CALL queryDispatch( 
		const util::URL&, const rtl::OUString &, sal_Int32 )
		throw ( uno::RuntimeException );
        
	virtual uno::Sequence< uno::Reference<frame::XDispatch> > SAL_CALL queryDispatches(
		const uno::Sequence<frame::DispatchDescriptor>& )
		throw ( uno::RuntimeException );
        
	// XDispatch
	virtual void SAL_CALL dispatch( const util::URL&, const uno::Sequence<beans::PropertyValue>& )
		throw( uno::RuntimeException );
        
	virtual void SAL_CALL addStatusListener( const uno::Reference<frame::XStatusListener>&, 
		const util::URL& ) 
		throw ( uno::RuntimeException );
        
	virtual void SAL_CALL removeStatusListener( const uno::Reference< frame::XStatusListener >&, const util::URL& )
		throw ( uno::RuntimeException );

	// XNotifyingDispatch
	virtual void SAL_CALL dispatchWithNotification( 
		const util::URL&, 
		const uno::Sequence< beans::PropertyValue >&, 
		const uno::Reference< frame::XDispatchResultListener >& )
		throw ( uno::RuntimeException );

	// XLpModel	
	virtual void SAL_CALL print() throw ( uno::RuntimeException );

	virtual void SAL_CALL setCost( sal_Int32, double ) throw ( uno::RuntimeException );
	virtual double SAL_CALL getCost( sal_Int32 ) throw ( uno::RuntimeException );

	virtual void SAL_CALL setVarBound( sal_Int32, org::openoffice::sc::solver::Bound, double )
		throw ( uno::RuntimeException );
	virtual double SAL_CALL getVarBound( sal_Int32, org::openoffice::sc::solver::Bound )
		throw ( uno::RuntimeException );
	virtual sal_Bool SAL_CALL isVarBounded( sal_Int32, org::openoffice::sc::solver::Bound )
		throw ( uno::RuntimeException );

	virtual void SAL_CALL setRhsValue( sal_Int32, double ) throw ( uno::RuntimeException );
	virtual double SAL_CALL getRhsValue( sal_Int32 ) throw ( uno::RuntimeException );

	virtual void SAL_CALL setConstraint( sal_Int32, sal_Int32, double ) throw ( uno::RuntimeException );
	virtual double SAL_CALL getConstraint( sal_Int32, sal_Int32 ) throw ( uno::RuntimeException );

	// getter & setter for attributes
	virtual void SAL_CALL setGoal( org::openoffice::sc::solver::Goal ) throw ( uno::RuntimeException );
	virtual org::openoffice::sc::solver::Goal SAL_CALL getGoal() throw ( uno::RuntimeException );

	virtual void SAL_CALL setVerbose( sal_Bool ) throw ( uno::RuntimeException );
	virtual sal_Bool SAL_CALL getVerbose() throw ( uno::RuntimeException );

	virtual void SAL_CALL setPrecision( long ) throw ( uno::RuntimeException );
	virtual long SAL_CALL getPrecision() throw ( uno::RuntimeException );

	virtual void SAL_CALL setVarPositive( sal_Bool ) throw ( uno::RuntimeException );
	virtual sal_Bool SAL_CALL getVarPositive() throw ( uno::RuntimeException );

	virtual void SAL_CALL setDecisionVarSize( long ) throw ( uno::RuntimeException );
	virtual long SAL_CALL getDecisionVarSize() throw ( uno::RuntimeException );

	virtual void SAL_CALL setConstraintCount( long ) throw ( uno::RuntimeException );
	virtual long SAL_CALL getConstraintCount() throw ( uno::RuntimeException );

private:
	::std::auto_ptr<TestModelData> m_pData;
};

}

#endif
