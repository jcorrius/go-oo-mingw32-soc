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


#ifndef _SOLVER_HXX_
#define _SOLVER_HXX_


#include <cppuhelper/implbase5.hxx>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/frame/XNotifyingDispatch.hpp>
#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>

#include <org/openoffice/sc/XSolver.hpp>

#include "type.hxx"
#include <vector>
#include <memory>

#define	SERVICE_NAME		"org.openoffice.sc.Solver"
#define	IMPLEMENTATION_NAME	"scsolver.SolverImpl"

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

namespace com { namespace sun { namespace star { 
	
	namespace frame
	{
		class XDispatch;
		class XDispatchDescriptor;
		class XDispatchResultListener;
		class XStatusListener;
	}

	namespace sheet 
	{	
		class XSpreadsheet;
	}

}}}

namespace scsolver {


class CalcInterface;
	
class SolverDialog;

//--------------------------------------------------------------------------
// SolverImpl

class SolverImpl : public ::cppu::WeakImplHelper5<
        ::org::openoffice::sc::XSolver, lang::XServiceInfo, lang::XInitialization,
        frame::XDispatchProvider, frame::XNotifyingDispatch >
{
public:

    SolverImpl( Reference< uno::XComponentContext > const & );
    ~SolverImpl();

	//--------------------------------------------------------------------------
	// UNO Component Interface Methods

    // XInitialization
    virtual void SAL_CALL initialize( const Sequence< Any >& aArgs )
        throw ( Exception );

    // XServiceInfo
    virtual rtl::OUString SAL_CALL getImplementationName()
        throw ( RuntimeException );
    virtual sal_Bool SAL_CALL supportsService( rtl::OUString const & )
        throw ( RuntimeException );
    virtual Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames()
        throw ( RuntimeException );

    // XDispatchProvider
    virtual Reference< frame::XDispatch > SAL_CALL queryDispatch( 
        const util::URL&, const ::rtl::OUString &, sal_Int32 )
        throw ( RuntimeException );
        
    virtual Sequence< Reference< frame::XDispatch > > SAL_CALL queryDispatches(
        const Sequence< frame::DispatchDescriptor >& )
        throw ( RuntimeException );
        
    // XDispatch
    virtual void SAL_CALL dispatch( const util::URL&, const Sequence< beans::PropertyValue >& )
        throw( RuntimeException );
        
    virtual void SAL_CALL addStatusListener( const Reference< frame::XStatusListener >&, 
		const util::URL& ) throw ( RuntimeException );
        
    virtual void SAL_CALL removeStatusListener( 
		const Reference< frame::XStatusListener >&, const util::URL& )
        throw ( RuntimeException );

    // XNotifyingDispatch
    virtual void SAL_CALL dispatchWithNotification( 
        const util::URL&, const Sequence< beans::PropertyValue >&, 
		const Reference< frame::XDispatchResultListener >& )
        throw ( RuntimeException );

    // XSolver
    virtual void SAL_CALL executeDialog() throw ( RuntimeException );

	// The "get-pointer" methods
	CalcInterface* getCalcInterface() const;	
	SolverDialog* getMainDialog();

	sal_Bool solveModel();
	
private:

	::std::auto_ptr<SolverDialog> m_pDlg;
	::std::auto_ptr<CalcInterface> m_pCalc;

};


}



#endif //_SOLVER_HXX_
