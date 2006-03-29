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

#ifndef CONFIMGR_DBBE_BASESTRATUM_HXX_
#define CONFIMGR_DBBE_BASESTRATUM_HXX_

#ifndef _COM_SUN_STAR_CONFIGURATION_BACKEND_XBACKENDENTITIES_HPP_
#include <com/sun/star/configuration/backend/XBackendEntities.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XINITIALIZATION_HPP_
#include <com/sun/star/lang/XInitialization.hpp>
#endif
#ifndef _COM_SUN_STAR_UNO_XCOMPONENTCONTEXT_HPP_
#include <com/sun/star/uno/XComponentContext.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif

#ifndef _COM_SUN_STAR_CONFIGURATION_BACKEND_XUPDATABLELAYER_HPP_
#include <com/sun/star/configuration/backend/XUpdatableLayer.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif

#ifndef _COM_SUN_STAR_CONFIGURATION_INVALIDBOOTSTRAPFILEEXCEPTION_HPP_
#include <com/sun/star/configuration/InvalidBootstrapFileException.hpp>
#endif

#ifndef _COM_SUN_STAR_CONFIGURATION_BACKEND_CANNOTCONNECTEXCEPTION_HPP_
#include <com/sun/star/configuration/backend/CannotConnectException.hpp>
#endif

#ifndef _CPPUHELPER_COMPBASE3_HXX_
#include <cppuhelper/compbase3.hxx>
#endif 

#ifndef _SALHELPER_SINGLETONREF_HXX_
#include <salhelper/singletonref.hxx>
#endif

#ifndef _RTL_STRBUF_HXX_
#include <rtl/strbuf.hxx>
#endif

#include <db_cxx.h>

#include "factory.hxx"

namespace configmgr 
{ 
    //this is done in localbe, and we don't know any better:
    struct ServiceImplementationInfo;
    
    namespace dbbe 
    {
        struct StratumMutexHolder
        {
            osl::Mutex mMutex;
        };

        namespace css = com::sun::star ;
        namespace uno = css::uno ;
        namespace lang = css::lang ;
        namespace backend = css::configuration::backend ;

        typedef cppu::WeakComponentImplHelper3<lang::XInitialization,
                                               backend::XBackendEntities, 
                                               lang::XServiceInfo>  BaseStratumImpl;
        
        class BaseStratum : protected StratumMutexHolder, public BaseStratumImpl
        {
        protected:
            /**
               Service constructor from a service factory.
               
               @param xContext   component context
            */
            BaseStratum(const uno::Reference<uno::XComponentContext>& xContext) ;
            
            /** Destructor */
            ~BaseStratum() ;

        public:
            // XInitialize
            virtual void SAL_CALL 
            initialize( const uno::Sequence<uno::Any>& aParameters) 
                throw (uno::RuntimeException, uno::Exception,
                       css::configuration::InvalidBootstrapFileException,
                       backend::CannotConnectException,
                       backend::BackendSetupException);

        
            // XBackendEntities
            virtual rtl::OUString SAL_CALL 
            getOwnerEntity(  ) 
                throw (uno::RuntimeException);
            
            virtual rtl::OUString SAL_CALL 
            getAdminEntity(  ) 
                throw (uno::RuntimeException);
            
            virtual sal_Bool SAL_CALL
            supportsEntity( const rtl::OUString& aEntity ) 
                throw (backend::BackendAccessException, uno::RuntimeException);
            
            virtual sal_Bool SAL_CALL 
            isEqualEntity( const rtl::OUString& aEntity, const rtl::OUString& aOtherEntity ) 
                throw (backend::BackendAccessException, lang::IllegalArgumentException, uno::RuntimeException);
            
            // XServiceInfo
            virtual rtl::OUString SAL_CALL 
            getImplementationName(  ) 
                throw (uno::RuntimeException);
    
            virtual sal_Bool SAL_CALL 
            supportsService( const rtl::OUString& aServiceName ) 
                throw (uno::RuntimeException) ;
            
            virtual uno::Sequence<rtl::OUString> SAL_CALL 
            getSupportedServiceNames(  ) 
                throw (uno::RuntimeException) ;
            
        protected:           
            const rtl::OString& getNamespace() const;

            Db& getDatabase() const;

            uno::Reference<lang::XMultiServiceFactory> getFactory() const;

            /// raise a NoSupportException for an attempt to update this layer
            void failReadonly();
            
            /**
               Tells if a file is more recent than a given date.
               The date is formatted YYYYMMDDhhmmssZ.
               
               @param aSubpath       relative URL of the component to check
               @param aTimestamp     timestamp to check against
               @return   sal_True if the file is more recent, sal_False otherwise
            */
            sal_Bool isMoreRecent(const rtl::OString& aKey,
                                  const rtl::OUString& aTimestamp) ;


            /**
               Fetches Key from database and tells if the record has sublayers
               
               @param  aKey           database Key
               @param  numSubLayers   the number of sublayers is written here
               @returns               true if the record is found, false otherwise
             */
            sal_Bool hasSubLayers(const rtl::OString& aKey, sal_uInt32 &numSubLayers)
                throw(backend::BackendAccessException);

        private:
            virtual const ServiceImplementationInfo * getServiceInfoData() const = 0;
            
            /** Service factory */
            uno::Reference<lang::XMultiServiceFactory> mFactory ;

            /** Database factory */
            // should be static??
            salhelper::SingletonRef<DbFactory> mDbFactory;
            
            /** Namespace of our strata */
            rtl::OString mStrataNamespace ;
            
            /** Database out strata is in */
            Db* mpDatabase;
            
        };

        /** 
            convert a Component to a key
            
            @param aNamespace the namespace the component
            @param aComponent the component
            @param aSubLayer  a sublayer of the component

            @return key string of the component and sublayer
         */
        rtl::OString ComponentToKey(const rtl::OString &aNamespace, 
                                    const rtl::OString &aComponent, 
                                    const rtl::OString &aSubLayer);

        /** 
            convert a Component to a key
            
            @param aNamespace the namespace the component
            @param aComponent the component
            @param aSubLayer  a sublayer of the component
            
            @return key string of the component and sublayer
        */
        rtl::OString ComponentToKey(const rtl::OString &aNamespace, 
                                    const rtl::OUString &aComponent, 
                                    const rtl::OString &aSubLayer);
       
        
    } //namespace dbbe 
}; //namespace configmgr

#endif//CONFIMGR_DBBE_BASESTRATUM_HXX_
