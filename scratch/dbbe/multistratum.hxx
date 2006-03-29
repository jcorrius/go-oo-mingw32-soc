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
#ifndef CONFIMGR_DBBE_MULTISTRATUM_HXX_
#define CONFIMGR_DBBE_MULTISTRATUM_HXX_

#ifndef CONFIMGR_DBBE_BASESTRATUM_HXX_
#include "basestratum.hxx"
#endif

#ifndef _CPPUHELPER_IMPLBASE1_HXX
#include <cppuhelper/implbase1.hxx>
#endif

#ifndef _COM_SUN_STAR_CONFIGURATION_BACKEND_XMULTILAYERSTRATUM_HPP_
#include <com/sun/star/configuration/backend/XMultiLayerStratum.hpp>
#endif

#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif // _RTL_USTRBUF_HXX_

namespace configmgr 
{ 
    namespace dbbe 
    {

        namespace css = com::sun::star ;
        namespace uno = css::uno ;
        namespace lang = css::lang ;
        namespace backend = css::configuration::backend ;
        
        typedef cppu::ImplInheritanceHelper1< BaseStratum,
                                              backend::XMultiLayerStratum
                                              > MultiStratumImplBase ;
        
        /**
           Implements the MultiLayerStratum service for local file access.
        */
        class MultiStratum : public MultiStratumImplBase 
        {
        public :
            /**
               Service constructor from a service factory.

               @param xContext   component context
            */
            MultiStratum(const uno::Reference<uno::XComponentContext>& xContext) ;

            /** Destructor */
            ~MultiStratum() ;


            // XMultiLayerStratum
            virtual uno::Sequence< rtl::OUString > SAL_CALL 
            listLayerIds( const rtl::OUString& aComponent, const rtl::OUString& aEntity ) 
                throw (backend::BackendAccessException, lang::IllegalArgumentException, uno::RuntimeException);
            
            virtual rtl::OUString SAL_CALL 
            getUpdateLayerId( const rtl::OUString& aComponent, const rtl::OUString& aEntity )
                throw (backend::BackendAccessException, lang::NoSupportException, 
                       lang::IllegalArgumentException, uno::RuntimeException);
            
            virtual uno::Reference< backend::XLayer > SAL_CALL
            getLayer( const rtl::OUString& aLayerId, const rtl::OUString& aTimestamp ) 
                throw (backend::BackendAccessException, lang::IllegalArgumentException, uno::RuntimeException);
            
            virtual uno::Sequence< uno::Reference< backend::XLayer > > SAL_CALL 
            getLayers( const uno::Sequence< rtl::OUString >& aLayerIds, const rtl::OUString& aTimestamp )
                throw (backend::BackendAccessException, lang::IllegalArgumentException, uno::RuntimeException);
            
            virtual uno::Sequence< uno::Reference< backend::XLayer > > SAL_CALL 
            getMultipleLayers( const uno::Sequence< rtl::OUString >& aLayerIds, const uno::Sequence< rtl::OUString >& aTimestamps ) 
                throw (backend::BackendAccessException, lang::IllegalArgumentException, uno::RuntimeException);
            
            virtual uno::Reference< backend::XUpdatableLayer > SAL_CALL 
            getUpdatableLayer( const rtl::OUString& aLayerId )
                throw (backend::BackendAccessException, lang::NoSupportException, 
                       lang::IllegalArgumentException, uno::RuntimeException);
            
        private:
            //
            //  FIXME
            //    some sort of replacement-- perhaps a <gasp> helper?
            //virtual void getLayerDirectories(rtl::OUString& aLayerUrl, rtl::OUString& aSubLayerUrl) const;
            virtual const ServiceImplementationInfo * getServiceInfoData() const;
        } ;
                
    } //namespace dbbe
}; //namespace configmgr


#endif
