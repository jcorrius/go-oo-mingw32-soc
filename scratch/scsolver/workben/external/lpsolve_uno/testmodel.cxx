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


#include "testmodel.hxx"
#include "osl/diagnose.h"

#include "com/sun/star/uno/XComponentContext.hpp"

#include "org/openoffice/sc/solver/XLpModel.hpp"
#include "org/openoffice/sc/solver/Goal.hpp"

#include <iostream>
#include <vector>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix_expression.hpp>

using namespace ::com::sun::star::uno;
using namespace ::org::openoffice::sc::solver;

using rtl::OUString;
namespace bnu = ::boost::numeric::ublas;
using ::std::cout;
using ::std::endl;

namespace scsolver {

typedef bnu::matrix<double, bnu::row_major, std::vector<double> > Mx;

void Debug( const char* str )
{
#ifdef DEBUG
	::std::cout << str << ::std::endl;
#endif
}

static uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames_TestModelImpl();
static rtl::OUString getImplementationName_TestModelImpl();
static Reference< uno::XInterface > SAL_CALL create_TestModelImpl(
	Reference< uno::XComponentContext > const & xContext )
	SAL_THROW( () );

struct TestModelData
{	
	TestModelData() : 
		CompContext( NULL ),
		Goal( org::openoffice::sc::solver::Goal_MAXIMIZE ),
		Verbose( false ),
		Precision( 2 ),
		VarPositive( true ),
		Cost( 10 ),
		DecisionVar( 10 ),
		RhsValue( 10 ),
		ConstraintMatrix( 10, 10 )
	{
	}

	~TestModelData() throw()
	{
	}

	Reference<XComponentContext> CompContext;

	org::openoffice::sc::solver::Goal Goal;
	bool Verbose;
	long Precision;
	bool VarPositive;

	::std::vector<double> Cost;
	::std::vector<double> DecisionVar;
	::std::vector<double> RhsValue;
	Mx ConstraintMatrix;
};

TestModelImpl::TestModelImpl( const Reference<XComponentContext>& xContext )
	: m_pData( new TestModelData )
{
	m_pData->CompContext = xContext;
}

TestModelImpl::~TestModelImpl() throw()
{
}

void TestModelImpl::initialize( const Sequence<Any>& cn ) throw ( Exception )
{
}

OUString TestModelImpl::getImplementationName() throw ( RuntimeException )
{
	return OUString( RTL_CONSTASCII_USTRINGPARAM(IMPLEMENTATION_NAME) );
}

sal_Bool TestModelImpl::supportsService( const OUString& sSrvName ) throw ( RuntimeException )
{
	return sSrvName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(SERVICE_NAME) );
}

Sequence<OUString> TestModelImpl::getSupportedServiceNames() throw ( RuntimeException )
{
	return getSupportedServiceNames_TestModelImpl();
}

Reference< frame::XDispatch > SAL_CALL TestModelImpl::queryDispatch(
	const util::URL& aURL, const OUString& sTargetFrameName, sal_Int32 nSearchFlags )
	throw ( RuntimeException )
{   
	Reference< frame::XDispatch > xRet;
	if ( aURL.Protocol.compareToAscii( "scsolver.TestModelImpl:" ) == 0 )
	{
		if ( aURL.Path.compareToAscii( "print" ) == 0 )
			xRet = this;
	}
	return xRet;
}

Sequence< Reference< frame::XDispatch > > SAL_CALL TestModelImpl::queryDispatches(
	const Sequence< frame::DispatchDescriptor >& seqDescripts )
	throw ( RuntimeException )
{
	sal_Int32 nCount = seqDescripts.getLength();
	Sequence< Reference< frame::XDispatch > > lDispatcher( nCount );
    
	for ( sal_Int32 i = 0; i < nCount; ++i )
		lDispatcher[i] = queryDispatch( 
			seqDescripts[i].FeatureURL, 
			seqDescripts[i].FrameName, 
			seqDescripts[i].SearchFlags );
    
	return lDispatcher;
}


void SAL_CALL TestModelImpl::dispatch( 
	const util::URL& aURL, const Sequence< beans::PropertyValue >& lArgs )
	throw ( RuntimeException )
{
	if ( aURL.Protocol.compareToAscii( "scsolver.TestModelImpl:" ) == 0 )
		if ( aURL.Path.compareToAscii( "print" ) == 0 )
			print();
}

void SAL_CALL TestModelImpl::addStatusListener( 
	const Reference< frame::XStatusListener >& xControl, const util::URL& aURL )
	throw ( RuntimeException )
{
}

void SAL_CALL TestModelImpl::removeStatusListener( 
	const Reference< frame::XStatusListener >& xControl, const util::URL& aURL )
	throw ( RuntimeException )
{
}

void SAL_CALL TestModelImpl::dispatchWithNotification(
	const util::URL& aURL, const Sequence< beans::PropertyValue >& lArgs,
	const Reference< frame::XDispatchResultListener >& xDRL )
	throw ( RuntimeException )
{
}

void SAL_CALL TestModelImpl::print() throw ( RuntimeException )
{
	Debug( "print model" );
	cout << m_pData->ConstraintMatrix << endl;
}

void SAL_CALL TestModelImpl::setCost( sal_Int32 id, double value )
	throw ( RuntimeException )
{
	m_pData->Cost.at( id ) = value;
}

double SAL_CALL TestModelImpl::getCost( sal_Int32 id )
	throw ( RuntimeException )
{
	return m_pData->Cost.at( id );
}

void SAL_CALL TestModelImpl::setVarBound( sal_Int32 id, Bound bound, double value )
	throw ( RuntimeException )
{
	
}

double SAL_CALL TestModelImpl::getVarBound( sal_Int32 id, Bound bound )
	throw ( RuntimeException )
{
	return 0.0;
}

sal_Bool SAL_CALL TestModelImpl::isVarBounded( sal_Int32 id, Bound bound )
	throw ( RuntimeException )
{
	return false;
}

void SAL_CALL TestModelImpl::setRhsValue( sal_Int32 id, double value )
	throw ( RuntimeException )
{
	m_pData->RhsValue.at( id ) = value;
}

double SAL_CALL TestModelImpl::getRhsValue( sal_Int32 id )
	throw ( RuntimeException )
{
	return m_pData->RhsValue.at( id );
}

void SAL_CALL TestModelImpl::setConstraint( sal_Int32 rowId, sal_Int32 colId, double value )
	throw ( RuntimeException )
{
	m_pData->ConstraintMatrix( rowId, colId ) = value;
}

double SAL_CALL TestModelImpl::getConstraint( sal_Int32 rowId, sal_Int32 colId )
	throw ( RuntimeException )
{
	return m_pData->ConstraintMatrix( rowId, colId );
}

void SAL_CALL TestModelImpl::setGoal( org::openoffice::sc::solver::Goal goal )
	throw ( RuntimeException )
{
	m_pData->Goal = goal;
}

org::openoffice::sc::solver::Goal SAL_CALL TestModelImpl::getGoal()
	throw ( RuntimeException )
{
	return m_pData->Goal;
}

void SAL_CALL TestModelImpl::setVerbose( sal_Bool b )
	throw ( RuntimeException )
{
	m_pData->Verbose = b;
}

sal_Bool SAL_CALL TestModelImpl::getVerbose()
	throw ( RuntimeException )
{
	return m_pData->Verbose;
}

void SAL_CALL TestModelImpl::setPrecision( sal_Int32 prec )
	throw ( RuntimeException )
{
	m_pData->Precision = prec;
}

long SAL_CALL TestModelImpl::getPrecision() throw ( RuntimeException )
{
	return m_pData->Precision;
}

void SAL_CALL TestModelImpl::setVarPositive( sal_Bool b )
	throw ( RuntimeException )
{
	m_pData->VarPositive = b;
}

sal_Bool SAL_CALL TestModelImpl::getVarPositive()
	throw ( RuntimeException )
{
	return m_pData->VarPositive;
}

void SAL_CALL TestModelImpl::setDecisionVarSize( sal_Int32 size )
	throw ( RuntimeException )
{
	OSL_ASSERT( size >= 0 );
	m_pData->DecisionVar.reserve( size );

	// Perform a destructive resize and resize only the column size.
	if ( m_pData->ConstraintMatrix.size2() < static_cast<size_t>(size) )
	{
		Mx mxTemp( m_pData->ConstraintMatrix.size1(), size );
		m_pData->ConstraintMatrix.swap( mxTemp );
	}
}

long SAL_CALL TestModelImpl::getDecisionVarSize()
	throw ( RuntimeException )
{
	return m_pData->ConstraintMatrix.size2();
}

void SAL_CALL TestModelImpl::setConstraintCount( sal_Int32 size )
	throw ( RuntimeException )
{
	OSL_ASSERT( size > 0 );
	m_pData->RhsValue.reserve( size );
	
	// Perform a destructive resize and resize only the row size.
	if ( m_pData->ConstraintMatrix.size1() < static_cast<size_t>(size) )
	{
		Mx mxTemp( size, m_pData->ConstraintMatrix.size2() );
		m_pData->ConstraintMatrix.swap( mxTemp );
	}
}

long SAL_CALL TestModelImpl::getConstraintCount()
	throw ( RuntimeException )
{
	return m_pData->ConstraintMatrix.size1();
}

//---------------------------------------------------------------------------
// Component operations

static Sequence< rtl::OUString > getSupportedServiceNames_TestModelImpl()
{
	static Sequence < rtl::OUString > *pNames = 0;
	if ( !pNames )
	{
		static Sequence< rtl::OUString > seqNames(1);
		seqNames.getArray()[0] = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(SERVICE_NAME));
		pNames = &seqNames;
	}
	return *pNames;
}

static rtl::OUString getImplementationName_TestModelImpl()
{
	static rtl::OUString *pImplName = 0;
	if ( !pImplName )
	{
		static rtl::OUString implName(
			RTL_CONSTASCII_USTRINGPARAM(IMPLEMENTATION_NAME));
		pImplName = &implName;
	}
	return *pImplName;
}

static Reference< uno::XInterface > SAL_CALL create_TestModelImpl(
	Reference< uno::XComponentContext > const & xContext )
	SAL_THROW( () )
{
	return static_cast< lang::XTypeProvider * >( new TestModelImpl( xContext ) );
}

static struct ::cppu::ImplementationEntry s_component_entries [] =
{
	{
		create_TestModelImpl, getImplementationName_TestModelImpl,
		getSupportedServiceNames_TestModelImpl, ::cppu::createSingleComponentFactory,
		0, 0
	},
	{ 0, 0, 0, 0, 0, 0 }
};


}

extern "C" 
{
	void SAL_DLLPUBLIC_EXPORT component_getImplementationEnvironment(
		sal_Char const ** ppEnvTypeName, uno_Environment ** ppEnv )
	{
		::scsolver::Debug( "testmodel: getenv called" );
		*ppEnvTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME;
	}
    
	sal_Bool SAL_DLLPUBLIC_EXPORT component_writeInfo(
		lang::XMultiServiceFactory * xMgr, registry::XRegistryKey * xRegistry )
	{		
		::scsolver::Debug( "testmodel: writeinfo" );
		return ::cppu::component_writeInfoHelper(
			xMgr, xRegistry, ::scsolver::s_component_entries );
	}

	void SAL_DLLPUBLIC_EXPORT *component_getFactory(
		sal_Char const * implName, lang::XMultiServiceFactory * xMgr,
		registry::XRegistryKey * xRegistry )
	{
		::scsolver::Debug( "testmodel: getfactory" );
		return ::cppu::component_getFactoryHelper(
			implName, xMgr, xRegistry, ::scsolver::s_component_entries );
	}
}


