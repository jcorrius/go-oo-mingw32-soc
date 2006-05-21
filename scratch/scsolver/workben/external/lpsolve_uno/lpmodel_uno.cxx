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


#include "lpmodel_uno.hxx"
#include "osl/diagnose.h"

#include "com/sun/star/uno/XComponentContext.hpp"

#include "org/openoffice/sc/solver/XLpModel.hpp"
#include "org/openoffice/sc/solver/Goal.hpp"
#include "org/openoffice/sc/solver/Equality.hpp"

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
using ::std::exception;

namespace scsolver {

typedef bnu::matrix<double, bnu::row_major, std::vector<double> > Mx;

static void Debug( const char* str )
{
#ifdef DEBUG
	::std::cout << str << ::std::endl;
#endif
}

static rtl::OUString ascii( const sal_Char* str )
{
	return rtl::OUString::createFromAscii( str );
}

template<typename Container>
void printElements( const Container& cn, const char* sep = " " )
{
	using namespace ::std;
	copy( cn.begin(), cn.end(), ostream_iterator<typename Container::value_type>( cout, sep ) );
	cout << endl;
}


template<typename DataType>
void ensureSize( ::std::vector<DataType>& cn, size_t size )
{
	using namespace std;

	if ( cn.empty() || cn.size() < size )
	{
		vector<DataType> cnTmp( size );
		typename vector<DataType>::iterator itr, 
			itrBeg = cn.begin(), itrEnd = cn.end();
		for ( itr = itrBeg; itr != itrEnd; ++itr )
			cnTmp.at( distance( itrBeg, itr ) ) = *itr;
		cn.swap( cnTmp );
	}
}

static uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames_TestModelImpl();
static rtl::OUString getImplementationName_TestModelImpl();
static Reference< uno::XInterface > SAL_CALL create_TestModelImpl(
	Reference< uno::XComponentContext > const & xContext )
	SAL_THROW( () );

struct LpModelData
{	
	LpModelData() : 
		CompContext( NULL ),
		Goal( org::openoffice::sc::solver::Goal_MAXIMIZE ),
		Verbose( false ),
		Precision( 2 ),
		VarPositive( true ),
		Cost( 1 ),
		DecisionVar( 1 ),
		RhsValue( 1 ),
		Equal( 1 ),
		ConstraintMatrix( 1, 1 )
	{
	}

	~LpModelData() throw()
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
	::std::vector<Equality> Equal;
	Mx ConstraintMatrix;
};

LpModelImpl::LpModelImpl( const Reference<XComponentContext>& xContext )
	: m_pData( new LpModelData )
{
	m_pData->CompContext = xContext;
}

LpModelImpl::~LpModelImpl() throw()
{
}

void LpModelImpl::initialize( const Sequence<Any>& cn ) throw ( Exception )
{
}

OUString LpModelImpl::getImplementationName() throw ( RuntimeException )
{
	return OUString( RTL_CONSTASCII_USTRINGPARAM(IMPLEMENTATION_NAME) );
}

sal_Bool LpModelImpl::supportsService( const OUString& sSrvName ) throw ( RuntimeException )
{
	return sSrvName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(SERVICE_NAME) );
}

Sequence<OUString> LpModelImpl::getSupportedServiceNames() throw ( RuntimeException )
{
	return getSupportedServiceNames_TestModelImpl();
}

Reference< frame::XDispatch > SAL_CALL LpModelImpl::queryDispatch(
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

Sequence< Reference< frame::XDispatch > > SAL_CALL LpModelImpl::queryDispatches(
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


void SAL_CALL LpModelImpl::dispatch( 
	const util::URL& aURL, const Sequence< beans::PropertyValue >& lArgs )
	throw ( RuntimeException )
{
	if ( aURL.Protocol.compareToAscii( "scsolver.TestModelImpl:" ) == 0 )
		if ( aURL.Path.compareToAscii( "print" ) == 0 )
			print();
}

void SAL_CALL LpModelImpl::addStatusListener( 
	const Reference< frame::XStatusListener >& xControl, const util::URL& aURL )
	throw ( RuntimeException )
{
}

void SAL_CALL LpModelImpl::removeStatusListener( 
	const Reference< frame::XStatusListener >& xControl, const util::URL& aURL )
	throw ( RuntimeException )
{
}

void SAL_CALL LpModelImpl::dispatchWithNotification(
	const util::URL& aURL, const Sequence< beans::PropertyValue >& lArgs,
	const Reference< frame::XDispatchResultListener >& xDRL )
	throw ( RuntimeException )
{
}

void SAL_CALL LpModelImpl::print() throw ( RuntimeException )
{
	Debug( "print model ----------------------------------------" );
	
	cout << "cost: ";
	printElements( m_pData->Cost );
	cout << m_pData->ConstraintMatrix << endl;
	cout << "rhs: ";
	printElements( m_pData->RhsValue );
}

void SAL_CALL LpModelImpl::setCost( sal_Int32 id, double value )
	throw ( RuntimeException )
{
	if ( id < 0 )
		throw RuntimeException( ascii("Negative argument not allowed"), *this );

	try
	{
		m_pData->Cost.at( id ) = value;
	}
	catch ( const exception& e )
	{
		throw RuntimeException( ascii( "Cost array out of range" ), *this );
	}
}

double SAL_CALL LpModelImpl::getCost( sal_Int32 id )
	throw ( RuntimeException )
{
	if ( id < 0 )
		throw RuntimeException( ascii("Negative argument not allowed"), *this );

	try
	{
		return m_pData->Cost.at( id );
	}
	catch ( const exception& e )
	{
		throw RuntimeException( ascii( "Cost array out of range" ), *this );
	}
}

void SAL_CALL LpModelImpl::setVarBound( sal_Int32 id, Bound bound, double value )
	throw ( RuntimeException )
{	
	if ( id < 0 )
		throw RuntimeException( ascii("Negative argument not allowed"), *this );
}

double SAL_CALL LpModelImpl::getVarBound( sal_Int32 id, Bound bound )
	throw ( RuntimeException )
{
	if ( id < 0 )
		throw RuntimeException( ascii("Negative argument not allowed"), *this );

	return 0.0;
}

sal_Bool SAL_CALL LpModelImpl::isVarBounded( sal_Int32 id, Bound bound )
	throw ( RuntimeException )
{
	if ( id < 0 )
		throw RuntimeException( ascii("Negative argument not allowed"), *this );

	return false;
}

void SAL_CALL LpModelImpl::setRhsValue( sal_Int32 id, double value )
	throw ( RuntimeException )
{	
	if ( id < 0 )
		throw RuntimeException( ascii("Negative argument not allowed"), *this );

	try
	{
		m_pData->RhsValue.at( id ) = value;
	}
	catch ( const exception& e )
	{
		throw RuntimeException( ascii( "Rhs array out of range" ), *this );
	}
}

double SAL_CALL LpModelImpl::getRhsValue( sal_Int32 id )
	throw ( RuntimeException )
{
	if ( id < 0 )
		throw RuntimeException( ascii("Negative argument not allowed"), *this );
	try
	{
		return m_pData->RhsValue.at( id );
	}
	catch ( const exception& e )
	{
		throw RuntimeException( ascii( "Rhs array out of range" ), *this );
	}
	
}

void SAL_CALL LpModelImpl::setConstraint( sal_Int32 rowId, sal_Int32 colId, double value )
	throw ( RuntimeException )
{
	if ( rowId < 0 || colId < 0 )
		throw RuntimeException( ascii("Negative argument not allowed"), *this );

	if ( m_pData->ConstraintMatrix.size1() > static_cast<size_t>(rowId) && 
		 m_pData->ConstraintMatrix.size2() > static_cast<size_t>(colId) )
		m_pData->ConstraintMatrix( rowId, colId ) = value;
	else
		throw RuntimeException( ascii("Matrix too small"), *this );
}

double SAL_CALL LpModelImpl::getConstraint( sal_Int32 rowId, sal_Int32 colId )
	throw ( RuntimeException )
{
	if ( rowId < 0 || colId < 0 )
		throw RuntimeException( ascii("Negative argument not allowed"), *this );

	if ( m_pData->ConstraintMatrix.size1() > static_cast<size_t>(rowId) && 
		 m_pData->ConstraintMatrix.size2() > static_cast<size_t>(colId) )
		return m_pData->ConstraintMatrix( rowId, colId );
	else
		throw RuntimeException( ascii("Matrix too small"), *this );
}

void SAL_CALL LpModelImpl::setEquality( sal_Int32 rowId, Equality eq )
	throw ( RuntimeException )
{
	if ( rowId < 0 )
		throw RuntimeException( ascii("Negative argument not allowed"), *this );

	try
	{
		m_pData->Equal.at( rowId ) = eq;
	}
	catch ( const exception& ex )
	{
		throw RuntimeException( ascii( "Equality array out of range" ), *this );
	}
}

Equality SAL_CALL LpModelImpl::getEquality( sal_Int32 rowId )
	throw ( RuntimeException )
{	
	if ( rowId < 0 )
		throw RuntimeException( ascii("Negative argument not allowed"), *this );

	try
	{
		return m_pData->Equal.at( rowId );
	}
	catch ( const exception& ex )
	{
		throw RuntimeException( ascii( "Equality array out of range" ), *this );
	}
}

//-----------------------------------------------------------------
// Attribute getters & setters start here

void SAL_CALL LpModelImpl::setGoal( org::openoffice::sc::solver::Goal goal )
	throw ( RuntimeException )
{
	m_pData->Goal = goal;
}

org::openoffice::sc::solver::Goal SAL_CALL LpModelImpl::getGoal()
	throw ( RuntimeException )
{
	return m_pData->Goal;
}

void SAL_CALL LpModelImpl::setVerbose( sal_Bool b )
	throw ( RuntimeException )
{
	m_pData->Verbose = b;
}

sal_Bool SAL_CALL LpModelImpl::getVerbose()
	throw ( RuntimeException )
{
	return m_pData->Verbose;
}

void SAL_CALL LpModelImpl::setPrecision( sal_Int32 prec )
	throw ( RuntimeException )
{
	m_pData->Precision = prec;
}

long SAL_CALL LpModelImpl::getPrecision() throw ( RuntimeException )
{
	return m_pData->Precision;
}

void SAL_CALL LpModelImpl::setVarPositive( sal_Bool b )
	throw ( RuntimeException )
{
	m_pData->VarPositive = b;
}

sal_Bool SAL_CALL LpModelImpl::getVarPositive()
	throw ( RuntimeException )
{
	return m_pData->VarPositive;
}

void SAL_CALL LpModelImpl::setDecisionVarSize( sal_Int32 size0 )
	throw ( RuntimeException )
{
	if ( size0 < 0 )
		return;

	size_t size = static_cast<size_t>(size0);
	
	ensureSize( m_pData->DecisionVar, size );
	ensureSize( m_pData->Cost, size );

	// Perform a destructive resize and resize only the column size.
	if ( m_pData->ConstraintMatrix.size2() < size )
	{
		Mx mxTemp( m_pData->ConstraintMatrix.size1(), size );
		m_pData->ConstraintMatrix.swap( mxTemp );
	}
}

long SAL_CALL LpModelImpl::getDecisionVarSize()
	throw ( RuntimeException )
{
	return m_pData->ConstraintMatrix.size2();
}

void SAL_CALL LpModelImpl::setConstraintCount( sal_Int32 size0 )
	throw ( RuntimeException )
{
	if ( size0 < 0 )
		return;

	size_t size = static_cast<size_t>(size0);

	ensureSize( m_pData->RhsValue, size );
	ensureSize( m_pData->Equal, size );
	
	// Perform a destructive resize and resize only the row size.
	if ( m_pData->ConstraintMatrix.size1() < size )
	{
		Mx mxTemp( size, m_pData->ConstraintMatrix.size2() );
		m_pData->ConstraintMatrix.swap( mxTemp );
	}
}

long SAL_CALL LpModelImpl::getConstraintCount()
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
	return static_cast< lang::XTypeProvider * >( new LpModelImpl( xContext ) );
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


