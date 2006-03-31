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
#include "multistratum.hxx"
#endif

#ifndef CONFIGMGR_API_FACTORY_HXX_
#include "confapifactory.hxx"
#endif // CONFIGMGR_API_FACTORY_HXX_

#ifndef CONFIGMGR_SERVICEINFOHELPER_HXX_
#include "serviceinfohelper.hxx"
#endif // CONFIGMGR_SERVICEINFOHELPER_HXX_

#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif // _RTL_USTRBUF_HXX_

#ifndef _RTL_STRBUF_HXX_
#include <rtl/strbuf.hxx>
#endif

#ifndef _COM_SUN_STAR_UNO_XCOMPONENTCONTEXT_HPP_
#include <com/sun/star/uno/XComponentContext.hpp>
#endif
#ifndef _COM_SUN_STAR_CONFIGURATION_BACKEND_INSUFFICIENTACCESSRIGHTSEXCEPTION_HPP_
#include <com/sun/star/configuration/backend/InsufficientAccessRightsException.hpp>
#endif

#include <db_cxx.h>

#include "Record.hxx"

#include "Layer.hxx"

namespace configmgr 
{ 
    namespace dbbe 
    {
        MultiStratum::MultiStratum(const uno::Reference<uno::XComponentContext>& xContext) 
            : MultiStratumImplBase(xContext)
        {
        }
        
        MultiStratum::~MultiStratum() 
        {
        }

        //
        //  note:
        //    as with localbe, aEntity is not important to us
        //
        uno::Sequence< rtl::OUString > SAL_CALL 
        MultiStratum::listLayerIds( const rtl::OUString& aComponent, 
                                    const rtl::OUString& aEntity ) 
            throw (backend::BackendAccessException, lang::IllegalArgumentException, uno::RuntimeException)
        {
            std::vector<rtl::OUString> aResult;

            rtl::OStringBuffer aBuf(aComponent.getLength() + getNamespace().getLength());
            aBuf.append(getNamespace());
            aBuf.append(rtl::OUStringToOString(aComponent, RTL_TEXTENCODING_UTF8));
            rtl::OString KeyString= aBuf.makeStringAndClear();
            Db& aDatabase= getDatabase();
            Dbt Key(const_cast<void*>(static_cast<const void*>(KeyString.getStr())), //kludge!!
                    KeyString.getLength());
            Dbt Data;
            int ret;
            ret= aDatabase.get(NULL, &Key, &Data, 0);
            switch (ret)
            {
                case 0: //success
                {
                    Record* pRecord= static_cast<Record*>(Data.get_data());
                    pRecord->unMarshal();
                    int swap= 0;  //dummy value to keep compiler from complaining
                    OSL_ASSERT(!aDatabase.get_byteswapped(&swap));
                    if (swap)
                        pRecord->bytesex();
                    std::vector<sal_Char*> aStrings= pRecord->listSubLayers();
                    std::vector<sal_Char*>::iterator it;
                    
                    for (it= aStrings.begin(); it != aStrings.end(); it++)
                        aResult.push_back(rtl::OUString::createFromAscii(*it));
                }
                break;
                
                case DB_NOTFOUND:
                    return uno::Sequence<rtl::OUString>();
                    break;
                    
                default: //something bad happened
                    rtl::OUStringBuffer errbuf;
                    errbuf.appendAscii("MultiStratum::listLayerIds: ");
                    errbuf.appendAscii("Error fetching Key: \"");
                    errbuf.appendAscii(KeyString.getStr());
                    errbuf.appendAscii("\" ");
                    errbuf.appendAscii("Error: ");
                    errbuf.append(static_cast<sal_Int32>(ret));
                    rtl::OUString const errmsg = errbuf.makeStringAndClear();
                    throw backend::BackendAccessException(errmsg,*this,uno::Any());
                    break;
            }
            return uno::Sequence<rtl::OUString>(&aResult.front(), 
                                                static_cast<sal_Int32>(aResult.size()));
        }

        rtl::OUString SAL_CALL 
        MultiStratum::getUpdateLayerId( const rtl::OUString& aComponent, 
                                        const rtl::OUString& aEntity )
            throw (backend::BackendAccessException, lang::NoSupportException, 
                   lang::IllegalArgumentException, uno::RuntimeException)
        {
            failReadonly();
            return aComponent;
        }

        uno::Sequence< uno::Reference< backend::XLayer > > SAL_CALL 
        MultiStratum::getLayers( const uno::Sequence< rtl::OUString >& aLayerIds, 
                                      const rtl::OUString& aTimestamp )
            throw (backend::BackendAccessException, lang::IllegalArgumentException, 
                   uno::RuntimeException)
        {
            sal_Int32 const nLayers = aLayerIds.getLength();
            uno::Sequence< uno::Reference< backend::XLayer > > aResult(nLayers);
            for (sal_Int32 ix=0; ix<nLayers; ++ix)
            {
                aResult[ix] = getLayer(aLayerIds[ix],aTimestamp);
            }
            return aResult;
        }


        uno::Sequence< uno::Reference< backend::XLayer > > SAL_CALL
        MultiStratum::getMultipleLayers( const uno::Sequence< rtl::OUString >& aLayerIds, 
                                         const uno::Sequence< rtl::OUString >& aTimestamps ) 
            throw (backend::BackendAccessException, lang::IllegalArgumentException, 
                   uno::RuntimeException)
        {
            if (aLayerIds.getLength() != aTimestamps.getLength()) {
                throw lang::IllegalArgumentException(
                    rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(
                                      "LocalStratum::getMultipleLayers(): Timestamp count does not match layer count")),
                    *this, 0) ;
            }
            sal_Int32 const nLayers = aLayerIds.getLength();
            uno::Sequence< uno::Reference< backend::XLayer > > aResult(nLayers);
            for (sal_Int32 ix=0; ix<nLayers; ++ix)
            {
                aResult[ix] = getLayer(aLayerIds[ix],aTimestamps[ix]);
            }
            return aResult;
        }
        
        uno::Reference< backend::XUpdatableLayer > SAL_CALL 
        MultiStratum::getUpdatableLayer( const rtl::OUString& aLayerId )
            throw (backend::BackendAccessException, lang::IllegalArgumentException,
                   lang::NoSupportException, uno::RuntimeException)
        {
            failReadonly();
            return 0;
        }
        

        //FIXME:
        //  can we set this to anything we want?  What is with the comp.configuration bit??
        static const sal_Char * const kMultiStratumImplementation = 
        "com.sun.star.comp.configuration.backend.DbMultiStratum" ;
        
        static const sal_Char * const kBackendService = 
        "com.sun.star.configuration.backend.MultiStratum" ;
        
        //FIXME
        //  can we set this to anything we want??
        static const sal_Char * const kLocalService = 
        "com.sun.star.configuration.backend.DbMultiStratum" ;
        
        static AsciiServiceName kServiceNames [] = { kLocalService, 0, kBackendService, 0 } ;
        static const ServiceImplementationInfo kMultiStratumServiceInfo   = { kMultiStratumImplementation  , kServiceNames, kServiceNames + 2 } ;
        
        const ServiceImplementationInfo * MultiStratum::getServiceInfoData() const
        {
            return &kMultiStratumServiceInfo;
        }

        //cribbed from localbe
        const ServiceRegistrationInfo *getMultiStratumServiceInfo() 
        { 
            return getRegistrationInfo(&kMultiStratumServiceInfo) ; 
        }

        uno::Reference<uno::XInterface> SAL_CALL 
        instantiateMultiStratum(const CreationContext& xContext) 
        {
            return *new MultiStratum(xContext) ;
        }


    uno::Reference< backend::XLayer > SAL_CALL
    MultiStratum::getLayer( const rtl::OUString& aLayerId, const rtl::OUString& aTimestamp ) 
        throw (backend::BackendAccessException, lang::IllegalArgumentException, uno::RuntimeException)
    {
        uno::Reference<backend::XLayer> xResult;
        if (aLayerId.getLength() == 0)
        {
            throw lang::IllegalArgumentException(
                rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(
                                  "MultiStratum:getLayer - no LayerId specified")),
                    *this, 0) ;
            }
            
            rtl::OString LayerId= rtl::OUStringToOString(aLayerId, RTL_TEXTENCODING_UTF8);
            
            if (!isMoreRecent(LayerId, aTimestamp))
            {
                //should be xResult.set(NULL)??
                return NULL;
            }
            xResult.set(new Layer(getFactory(), getDatabase(), LayerId));               
            return xResult;
    }
        
    }; //namespace dbbe
}; //namespace configmgr
