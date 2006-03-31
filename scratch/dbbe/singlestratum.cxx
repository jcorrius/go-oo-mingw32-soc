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

#include "singlestratum.hxx"

#include "Layer.hxx"

#include <db_cxx.h>

#ifndef CONFIGMGR_API_FACTORY_HXX_
#include "confapifactory.hxx"
#endif // CONFIGMGR_API_FACTORY_HXX_

#ifndef CONFIGMGR_SERVICEINFOHELPER_HXX_
#include "serviceinfohelper.hxx"
#endif // CONFIGMGR_SERVICEINFOHELPER_HXX_

#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif // _RTL_USTRBUF_HXX_

#ifndef _COM_SUN_STAR_UNO_XCOMPONENTCONTEXT_HPP_
#include <com/sun/star/uno/XComponentContext.hpp>
#endif
#ifndef _COM_SUN_STAR_CONFIGURATION_BACKEND_INSUFFICIENTACCESSRIGHTSEXCEPTION_HPP_
#include <com/sun/star/configuration/backend/InsufficientAccessRightsException.hpp>
#endif

namespace configmgr 
{ 
    namespace dbbe 
    {  
        SingleStratum::SingleStratum(const uno::Reference<uno::XComponentContext>& xContext) 
            :SingleStratumImplBase(xContext)
        {
        }
            
        SingleStratum::~SingleStratum()
        {
        }
        
        // XSingleLayerStratum
        uno::Reference<backend::XLayer> SAL_CALL 
        SingleStratum::getLayer( const rtl::OUString& aComponent, const rtl::OUString& aTimestamp )
            throw (backend::BackendAccessException,
                   lang::IllegalArgumentException,
                   uno::RuntimeException) 
        {
             uno::Reference<backend::XLayer> xResult;
             if (aComponent.getLength() == 0)
             {
                 throw lang::IllegalArgumentException(
                     rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(
                                       "SingleStratum:getLayer - no LayerId specified")),
                     *this, 0) ;
             }
             
            const rtl::OString aKey= ComponentToKey(getNamespace(), 
                                                    aComponent, 
                                                    rtl::OString());
            Db& aDatabase= getDatabase();
            sal_uInt32 numSubLayers= 0;
            if (hasSubLayers(aKey, numSubLayers))
            {
                if (numSubLayers)
                {
                    xResult.set(new CompositeLayer(getFactory(), getDatabase(), aKey));
                }
                else
                {
                    xResult.set(new Layer(getFactory(), getDatabase(), aKey));
                }
            }
                else
            {
                //If I understand correctly, if you give me
                //a Layer that doesn't exist, I give you an empty layer
                xResult.set(new Layer(getFactory(), getDatabase(), rtl::OString()));
            }
            return xResult;
        }
        
        uno::Reference<backend::XUpdatableLayer> SAL_CALL
        SingleStratum::getUpdatableLayer( const rtl::OUString& aLayerId ) 
            throw (backend::BackendAccessException, 
                   lang::IllegalArgumentException,
                   lang::NoSupportException,
                   uno::RuntimeException)
        {
            uno::Reference <backend::XUpdatableLayer> xResult;
            if (aLayerId.getLength() == 0)
            {
                throw lang::IllegalArgumentException(
                    rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(
                                      "MultiStratum:getLayer - no LayerId specified")),
                    *this, 0) ;
            } 
            u_int32_t open_flags;
            int ret= getDatabase().get_open_flags(&open_flags);
            OSL_ASSERT(!ret);
            if (ret || DB_RDONLY)
            {
                failReadonly();
                return NULL;
            }
            else
            {
                Db& aDatabase= getDatabase();
                sal_uInt32 numSubLayers= 0;
                rtl::OString aKey= rtl::OUStringToOString(aLayerId, RTL_TEXTENCODING_UTF8);
                if (hasSubLayers(aKey, numSubLayers))
                {
                    if (numSubLayers)
                    {
                        xResult.set(new UpdatableCompositeLayer(getFactory(), getDatabase(), aKey));
                    }
                    else
                    {
                        xResult.set(new UpdatableLayer(getFactory(), getDatabase(), aKey));
                    }
                }
                else
                {
                    //If I understand correctly, if you give me
                    //a Layer that doesn't exist, I give you an empty layer
                    xResult.set(new UpdatableLayer(getFactory(), getDatabase(), rtl::OString()));
                }
            }
            return xResult;
        }

        static const sal_Char * const kSingleStratumImplementation = 
        "com.sun.star.comp.configuration.backend.DbSingleStratum" ;
        static const sal_Char * const kBackendService = 
        "com.sun.star.configuration.backend.SingleStratum" ;
        static const sal_Char * const kLocalService = 
        "com.sun.star.configuration.backend.DbSingleStratum" ;
        static AsciiServiceName kServiceNames [] = { kLocalService, 0, kBackendService, 0 } ;
        static const ServiceImplementationInfo kSingleStratumServiceInfo = { kSingleStratumImplementation, kServiceNames, kServiceNames + 2 } ;

        const ServiceRegistrationInfo *getSingleStratumServiceInfo() 
        { 
            return getRegistrationInfo(&kSingleStratumServiceInfo) ; 
        }
        
    }; //namespace dbbe
}; //namespace configmr

