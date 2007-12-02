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

#include <cppuhelper/implbase6.hxx>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/frame/XNotifyingDispatch.hpp>
#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XLocalizable.hpp>

#include <com/sun/star/ui/dialogs/XExecutableDialog.hpp>

#include "tool/global.hxx"
#include "type.hxx"
#include <vector>
#include <memory>

#ifdef SCSOLVER_UNO_COMPONENT
class ResMgr;
#else
#include <tools/resmgr.hxx>
#endif

#define	SERVICE_NAME		"org.go-oo.CalcSolver"
#define	IMPLEMENTATION_NAME	"scsolver.SolverImpl"

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
class OptionData;
class SolverDialog;
class StringResMgr;

//--------------------------------------------------------------------------
// SolverImpl

class SolverImpl : public ::cppu::WeakImplHelper6<
    ::com::sun::star::ui::dialogs::XExecutableDialog,
    ::com::sun::star::lang::XServiceInfo, 
    ::com::sun::star::lang::XInitialization,
    ::com::sun::star::frame::XDispatchProvider,
    ::com::sun::star::frame::XNotifyingDispatch,
    ::com::sun::star::lang::XLocalizable >
{
public:

    SolverImpl(const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& xContext);
    ~SolverImpl();

    //--------------------------------------------------------------------------
    // UNO Component Interface Methods

    // XInitialization
    virtual void SAL_CALL initialize( const ::com::sun::star::uno::Sequence< Any >& aArgs )
        throw (::com::sun::star::uno::Exception);

    // XServiceInfo
    virtual rtl::OUString SAL_CALL getImplementationName()
        throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL supportsService( rtl::OUString const & )
        throw (::com::sun::star::uno::RuntimeException);
    virtual Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames()
        throw (::com::sun::star::uno::RuntimeException);

    // XDispatchProvider
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch > SAL_CALL queryDispatch( 
        const ::com::sun::star::util::URL&, const ::rtl::OUString &, sal_Int32 )
        throw (::com::sun::star::uno::RuntimeException);
        
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch > > SAL_CALL 
        queryDispatches(const ::com::sun::star::uno::Sequence< ::com::sun::star::frame::DispatchDescriptor >& )
        throw (::com::sun::star::uno::RuntimeException);
        
    // XDispatch
    virtual void SAL_CALL dispatch( const ::com::sun::star::util::URL&, 
                                    const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& )
        throw(::com::sun::star::uno::RuntimeException);
        
    virtual void SAL_CALL addStatusListener( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener >&, 
		const ::com::sun::star::util::URL& ) 
        throw (::com::sun::star::uno::RuntimeException);
        
    virtual void SAL_CALL removeStatusListener( 
		const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener >&, const ::com::sun::star::util::URL& )
        throw (::com::sun::star::uno::RuntimeException);

    // XNotifyingDispatch
    virtual void SAL_CALL dispatchWithNotification( 
        const ::com::sun::star::util::URL&, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >&, 
		const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchResultListener >& )
        throw (::com::sun::star::uno::RuntimeException);

    // XExecutableDialog
	virtual void SAL_CALL setTitle( const ::rtl::OUString& aTitle )
			throw (::com::sun::star::uno::RuntimeException);
	virtual sal_Int16 SAL_CALL execute()
			throw (::com::sun::star::uno::RuntimeException);

	// XLocalizable
    virtual void SAL_CALL setLocale( const com::sun::star::lang::Locale& eLocale )
		throw(::com::sun::star::uno::RuntimeException);
    virtual com::sun::star::lang::Locale SAL_CALL getLocale()
		throw(::com::sun::star::uno::RuntimeException);

	// The "get-pointer" methods
	CalcInterface* getCalcInterface() const;
	OptionData* getOptionData() const;
	SolverDialog* getMainDialog();

	sal_Bool solveModel();
    rtl::OUString getResStr( int resid );

private:

#ifndef SCSOLVER_UNO_COMPONENT
    ResMgr* getResMgr();
    ResMgr *m_pResMgr;
#endif

    void initLocale();

    com::sun::star::lang::Locale    m_eLocale;
    ::std::auto_ptr<SolverDialog>   m_pDlg;
    ::std::auto_ptr<CalcInterface>  m_pCalc;
    ::std::auto_ptr<OptionData>     m_pOption;
    ::std::auto_ptr<StringResMgr>   m_pStringResMgr;
};


}



#endif //_SOLVER_HXX_
