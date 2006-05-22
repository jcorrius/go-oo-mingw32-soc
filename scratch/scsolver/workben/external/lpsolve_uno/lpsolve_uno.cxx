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


#include "lpsolve_uno.hxx"
#include "lp_lib.h"
#include "org/openoffice/sc/solver/XLpModel.hpp"
#include "com/sun/star/lang/XMultiComponentFactory.hpp"

#include <iostream>
#include <vector>
#include <sstream>

using namespace ::com::sun::star::uno;
using namespace ::org::openoffice::sc::solver;
using namespace ::std;
using rtl::OUString;

namespace scsolver {

static void Debug( const char* str )
{
#ifdef DEBUG
	::std::cout << str << ::std::endl;
#endif
}

static rtl::OUString ascii( const sal_Char* sAscii )
{
	return rtl::OUString::createFromAscii( sAscii );
}

static uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames_LpSolveImpl();
static rtl::OUString getImplementationName_LpSolveImpl();
static Reference< uno::XInterface > SAL_CALL create_LpSolveImpl(
	Reference< uno::XComponentContext > const & xContext )
	SAL_THROW( () );

struct LpSolveImplData
{
	LpSolveImplData() :
		CompContext( NULL ), LpModel( NULL )
	{
	}

	~LpSolveImplData() throw()
	{
	}
	
	Reference<XComponentContext> CompContext;
	Reference<XLpModel> LpModel;
	::std::vector<double> DecVariables;
};

LpSolveImpl::LpSolveImpl( const Reference<XComponentContext>& xContext )
	: m_pData( new LpSolveImplData )
{
	m_pData->CompContext = xContext;
}

LpSolveImpl::~LpSolveImpl() throw()
{
}

void LpSolveImpl::initialize( const Sequence<Any>& cn ) throw ( Exception )
{
}

OUString LpSolveImpl::getImplementationName() throw ( RuntimeException )
{
	return OUString( RTL_CONSTASCII_USTRINGPARAM(IMPLEMENTATION_NAME) );
}

sal_Bool LpSolveImpl::supportsService( const OUString& sSrvName ) throw ( RuntimeException )
{
	return sSrvName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM(SERVICE_NAME) );
}

Sequence<OUString> LpSolveImpl::getSupportedServiceNames() throw ( RuntimeException )
{
	return getSupportedServiceNames_LpSolveImpl();
}

Reference< frame::XDispatch > SAL_CALL LpSolveImpl::queryDispatch(
	const util::URL& aURL, const OUString& sTargetFrameName, sal_Int32 nSearchFlags )
	throw ( RuntimeException )
{   
	Reference< frame::XDispatch > xRet;
	if ( aURL.Protocol.compareToAscii( "scsolver.LpSolveImpl:" ) == 0 )
	{
		if ( aURL.Path.compareToAscii( "solve" ) == 0 )
			xRet = this;
	}
	return xRet;
}

Sequence< Reference< frame::XDispatch > > SAL_CALL LpSolveImpl::queryDispatches(
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


void SAL_CALL LpSolveImpl::dispatch( 
	const util::URL& aURL, const Sequence< beans::PropertyValue >& lArgs )
	throw ( RuntimeException )
{
	if ( aURL.Protocol.compareToAscii( "scsolver.LpSolveImpl:" ) == 0 )
		if ( aURL.Path.compareToAscii( "solve" ) == 0 )
			solve();
}

void SAL_CALL LpSolveImpl::addStatusListener( 
	const Reference< frame::XStatusListener >& xControl, const util::URL& aURL )
	throw ( RuntimeException )
{
}

void SAL_CALL LpSolveImpl::removeStatusListener( 
	const Reference< frame::XStatusListener >& xControl, const util::URL& aURL )
	throw ( RuntimeException )
{
}

void SAL_CALL LpSolveImpl::dispatchWithNotification(
	const util::URL& aURL, const Sequence< beans::PropertyValue >& lArgs,
	const Reference< frame::XDispatchResultListener >& xDRL )
	throw ( RuntimeException )
{
}

void SAL_CALL LpSolveImpl::setModel( 
	const Reference< org::openoffice::sc::solver::XLpModel >& xLpModel )
	throw ( RuntimeException )
{
	m_pData->LpModel = xLpModel;
}

Reference< XLpModel > SAL_CALL LpSolveImpl::getModel()
	throw ( RuntimeException )
{
	return m_pData->LpModel;
}

void SAL_CALL LpSolveImpl::solve() throw( RuntimeException )
{
	using ::std::vector;

	Debug( "solve ------------------------------------" );

	if ( m_pData->LpModel == NULL )
	{
		Debug( "model is not set" );
		return;
	}

	Reference<XLpModel> model = m_pData->LpModel;
	if ( model->getGoal() == Goal_MAXIMIZE )
		cout << "let's maximize this" << endl;

	if ( model->getVerbose() )
	{
		cout << "verbose set to on" << endl;
	}

	cout << "precision set to " << model->getPrecision() << endl;
	if ( model->getVarPositive() )
		cout << "variable assummed to be all positive" << endl;
	else
		cout << "variable not positive" << endl;

	// OK - Start putting things together for lp_solve to run.

	int nConstCount = model->getConstraintCount();
	int nDecVarSize = model->getDecisionVarSize();

	cout << "decision var (" << nDecVarSize << ")" << endl;
	cout << "constraint   (" << nConstCount << ")" << endl;
	
	lprec *lp = make_lp( 0, nDecVarSize );
	if ( lp == NULL )
		throw RuntimeException( ascii("lp_solve error"), *this );

	for ( int i = 1; i <= nDecVarSize; ++i )
	{
		::std::ostringstream os;
		os << "x" << i;
		set_col_name( lp, i, const_cast<char *>(os.str().c_str()) );
	}

	// map constraints
	set_add_rowmode( lp, true );
	vector<double> row( nConstCount );
	vector<int> cols( nConstCount );
	for ( int i = 0; i < nConstCount; ++i )
		cols.at(i) = i+1;

	for ( int i = 0; i < nConstCount; ++i )
	{
		for ( int j = 0; j < nDecVarSize; ++j )
			row.at(j) = model->getConstraint( i, j );
		int nEqual;
		switch ( model->getEquality(i) )
		{
		case org::openoffice::sc::solver::Equality_GREATER_EQUAL:
			nEqual = GE;
			break;
		case org::openoffice::sc::solver::Equality_LESS_EQUAL:
			nEqual = LE;
			break;
		case org::openoffice::sc::solver::Equality_EQUAL:
			nEqual = EQ;
			break;
		}
		add_constraintex( lp, nDecVarSize, &row[0], &cols[0], nEqual,
			model->getRhsValue(i) );
	}

	set_add_rowmode( lp, false );

	// set objective function
	for ( int i = 0; i < nDecVarSize; ++i )
		row.at(i) = model->getCost(i);
	set_obj_fnex( lp, nDecVarSize,  &row[0],  &cols[0] );

	// set goal
	switch ( model->getGoal() )
	{
	case org::openoffice::sc::solver::Goal_MAXIMIZE:
		set_maxim(lp);
		break;
	case org::openoffice::sc::solver::Goal_MINIMIZE:
		set_minim(lp);
		break;
	default:
		throw RuntimeException( ascii("Unknown goal"), *this );
	}

	write_LP(lp, stdout);

	set_verbose(lp, IMPORTANT);

	if ( ::solve(lp) == OPTIMAL )
	{
		cout << "--- solution found ---" << endl;

		// objective value
		cout << "Objective value: " << get_objective(lp) << endl;

		// variable values
		get_variables(lp, &row[0]);
		::std::vector<double> cnVars( row.begin(), row.end() );
		m_pData->DecVariables.swap( cnVars );
		for ( int i = 0; i < nDecVarSize; ++i )
			cout << get_col_name( lp, i + 1 ) << ": " << row[i] << endl;
	}
	else
		cout << "--- solution not found ---" << endl;

	delete_lp(lp);
}

double LpSolveImpl::getVar( sal_Int32 id ) throw ( RuntimeException )
{
	if ( id < 0 )
		throw RuntimeException( ascii("Negative argument not allowed"), *this );

	size_t nColId = static_cast<size_t>(id);
	if ( m_pData->DecVariables.size() <= id )
		throw RuntimeException( ascii("ID out of range"), *this );

	return m_pData->DecVariables[id];
}

Sequence<double> LpSolveImpl::getVars() throw ( RuntimeException )
{
	Sequence<double> cnVars( m_pData->DecVariables.size() );
	double* pVars = cnVars.getArray();
	vector<double>::iterator itr, 
		itrBeg = m_pData->DecVariables.begin(),
		itrEnd = m_pData->DecVariables.end();
	for ( itr = itrBeg; itr != itrEnd; ++itr )
	{
		size_t i = distance( itrBeg, itr );
		pVars[i] = *itr;
	}
	return cnVars;
}

//---------------------------------------------------------------------------
// Component operations

static Sequence< rtl::OUString > getSupportedServiceNames_LpSolveImpl()
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

static rtl::OUString getImplementationName_LpSolveImpl()
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

static Reference< uno::XInterface > SAL_CALL create_LpSolveImpl(
	Reference< uno::XComponentContext > const & xContext )
	SAL_THROW( () )
{
	return static_cast< lang::XTypeProvider * >( new LpSolveImpl( xContext ) );
}

static struct ::cppu::ImplementationEntry s_component_entries [] =
{
	{
		create_LpSolveImpl, getImplementationName_LpSolveImpl,
		getSupportedServiceNames_LpSolveImpl, ::cppu::createSingleComponentFactory,
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
		cout << "getenv called" << endl;
		*ppEnvTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME;
	}
    
	sal_Bool SAL_DLLPUBLIC_EXPORT component_writeInfo(
		lang::XMultiServiceFactory * xMgr, registry::XRegistryKey * xRegistry )
	{		
		cout << "writeinfo" << endl;
		return ::cppu::component_writeInfoHelper(
			xMgr, xRegistry, ::scsolver::s_component_entries );
	}

	void SAL_DLLPUBLIC_EXPORT *component_getFactory(
		sal_Char const * implName, lang::XMultiServiceFactory * xMgr,
		registry::XRegistryKey * xRegistry )
	{
		cout << "getfactory" << endl;
		return ::cppu::component_getFactoryHelper(
			implName, xMgr, xRegistry, ::scsolver::s_component_entries );
	}
}


