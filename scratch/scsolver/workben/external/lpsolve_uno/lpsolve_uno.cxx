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

using namespace ::com::sun::star::uno;
using namespace ::org::openoffice::sc::solver;
using namespace ::std;
using rtl::OUString;

namespace scsolver {

static rtl::OUString ascii( const sal_Char* sAscii )
{
	return rtl::OUString::createFromAscii( sAscii );
}

static uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames_LpSolveImpl();
static rtl::OUString getImplementationName_LpSolveImpl();
static Reference< uno::XInterface > SAL_CALL create_LpSolveImpl(
	Reference< uno::XComponentContext > const & xContext )
	SAL_THROW( () );

static int demo();

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
};

LpSolveImpl::LpSolveImpl( const Reference<XComponentContext>& xContext )
	: m_pData( new LpSolveImplData )
{
	cout << "c'tor called" << endl;
	m_pData->CompContext = xContext;
}

LpSolveImpl::~LpSolveImpl() throw()
{
	cout << "d'tor called" << endl;
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
		if ( aURL.Path.compareToAscii( "run" ) == 0 )
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
		if ( aURL.Path.compareToAscii( "run" ) == 0 )
			run();
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

void SAL_CALL LpSolveImpl::run() throw ( RuntimeException )
{
	cout << "run this" << endl;
	Reference<lang::XMultiComponentFactory> xSM = m_pData->CompContext->getServiceManager();
	Reference<XInterface> model = xSM->createInstanceWithContext( 
		ascii( "org.openoffice.sc.solver.TestModel" ), m_pData->CompContext );

	demo();
}

void SAL_CALL LpSolveImpl::solve() throw( RuntimeException )
{
	if ( m_pData->LpModel == NULL )
	{
		cout << "model is not set" << endl;
		return;
	}

	cout << "print model information" << endl;

	Reference<XLpModel> model = m_pData->LpModel;
	if ( model->getGoal() == Goal_MAXIMIZE )
	{
		cout << "let's maximize this" << endl;
	}

	if ( model->getVerbose() )
	{
		cout << "verbose set to on" << endl;
	}

	cout << "precision set to " << model->getPrecision() << endl;
	if ( model->getVarPositive() )
		cout << "variable assummed to be all positive" << endl;
	else
		cout << "variable not positive" << endl;
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

static int demo()
{
	lprec *lp;
	int Ncol, *colno = NULL, j, ret = 0;
	REAL *row = NULL;

	/* We will build the model row by row
	   So we start with creating a model with 0 rows and 2 columns */
	Ncol = 2; /* there are two variables in the model */
	lp = make_lp(0, Ncol);
	if (lp == NULL)
		ret = 1; /* couldn't construct a new model... */

	if (ret == 0)
	{
		/* let us name our variables. Not required, but can be useful for debugging */
		set_col_name(lp, 1, "x");
		set_col_name(lp, 2, "y");

		/* create space large enough for one row */
		colno = (int *) malloc(Ncol * sizeof(*colno));
		row = (REAL *) malloc(Ncol * sizeof(*row));
		if ((colno == NULL) || (row == NULL))
			ret = 2;
	}

	if (ret == 0)
	{
		set_add_rowmode(lp, TRUE);	/* makes building the model faster if it is done rows by row */

		/* construct first row (120 x + 210 y <= 15000) */
		j = 0;

		colno[j] = 1; /* first column */
		row[j++] = 120;

		colno[j] = 2; /* second column */
		row[j++] = 210;

		/* add the row to lpsolve */
		if (!add_constraintex(lp, j, row, colno, LE, 15000))
			ret = 3;
	}

	if (ret == 0)
	{
		/* construct second row (110 x + 30 y <= 4000) */
		j = 0;

		colno[j] = 1; /* first column */
		row[j++] = 110;

		colno[j] = 2; /* second column */
		row[j++] = 30;

		/* add the row to lpsolve */
		if (!add_constraintex(lp, j, row, colno, LE, 4000))
			ret = 3;
	}

	if (ret == 0)
	{
		/* construct third row (x + y <= 75) */
		j = 0;

		colno[j] = 1; /* first column */
		row[j++] = 1;

		colno[j] = 2; /* second column */
		row[j++] = 1;

		/* add the row to lpsolve */
		if (!add_constraintex(lp, j, row, colno, LE, 75))
			ret = 3;
	}

	if (ret == 0)
	{
		set_add_rowmode(lp, FALSE);	/* rowmode should be turned off again when done building the model */

		/* set the objective function (143 x + 60 y) */
		j = 0;

		colno[j] = 1; /* first column */
		row[j++] = 143;

		colno[j] = 2; /* second column */
		row[j++] = 60;

		/* set the objective in lpsolve */
		if (!set_obj_fnex(lp, j, row, colno))
			ret = 4;
	}

	if (ret == 0)
	{
		/* set the object direction to maximize */
		set_maxim(lp);

		/* just out of curioucity, now show the model in lp format on screen */
		/* this only works if this is a console application. If not, use write_lp and a filename */
		write_LP(lp, stdout);
		/* write_lp(lp, "model.lp"); */

		/* I only want to see important messages on screen while solving */
		set_verbose(lp, IMPORTANT);

		/* Now let lpsolve calculate a solution */
		ret = solve(lp);
		if (ret == OPTIMAL)
			ret = 0;
		else
			ret	= 5;
	}

	if (ret == 0)
	{
		/* a solution is calculated, now lets get some results */

		/* objective value */
		printf("Objective value: %f\n", get_objective(lp));

		/* variable values */
		get_variables(lp, row);
		for (j = 0; j < Ncol; j++)
			printf("%s: %f\n", get_col_name(lp, j + 1), row[j]);

		/* we are done now */
	}

	/* free allocated memory */
	if (row != NULL)
		free(row);
	if (colno != NULL)
		free(colno);

	if (lp != NULL)
	{
		/* clean up such that all used memory by lpsolve is freed */
		delete_lp(lp);
	}

	return(ret);
}

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


