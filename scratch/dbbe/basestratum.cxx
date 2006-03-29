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

#include "basestratum.hxx"

#include "serviceinfohelper.hxx"

#include "Layer.hxx"

#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif

#include <com/sun/star/lang/NoSupportException.hpp>


namespace configmgr 
{ 
    namespace dbbe 
    {
        BaseStratum::BaseStratum(const uno::Reference<uno::XComponentContext>& xContext) 
            : BaseStratumImpl(mMutex),
              mFactory(xContext->getServiceManager(),uno::UNO_QUERY),
              mDbFactory()
        {
            mpDatabase= NULL;
        }
    
        BaseStratum::~BaseStratum()
        {
        }

        const rtl::OString& BaseStratum::getNamespace() const
        {
            return mStrataNamespace;
        }

        Db& BaseStratum::getDatabase() const
        {
            OSL_ASSERT(mpDatabase);
            return *mpDatabase;
        }

        uno::Reference<lang::XMultiServiceFactory> BaseStratum::getFactory() const
        {
            return mFactory;
        }

    
        void SAL_CALL BaseStratum::initialize(const uno::Sequence<uno::Any>& aParameters) 
            throw (uno::RuntimeException, uno::Exception,
                   css::configuration::InvalidBootstrapFileException,
                   backend::CannotConnectException,
                   backend::BackendSetupException)
        {
            if (aParameters.getLength() == 0) 
            {
                throw lang::IllegalArgumentException(
                    rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(
                                      "No parameters provided to Stratum")),
                    *this, 0) ;
            }
        
            //the argument is a file and a namespace of the form
            // /foo/bar/baz.db::namespace
            for (sal_Int32 i = 0 ; i < aParameters.getLength() ; ++ i) 
            {
                rtl::OUString param;
                if (aParameters [i] >>= param )
                { 
                    static const rtl::OUString kSeperator(RTL_CONSTASCII_USTRINGPARAM("::"));
                    sal_Int32 separator= param.match(kSeperator);
                    if (separator != -1)
                    {                   
                        mpDatabase= mDbFactory->getDb(param.copy(0, separator - 1));
                        mStrataNamespace= rtl::OUStringToOString(param.copy(
                                                                     separator + kSeperator.getLength()),
                                                                 RTL_TEXTENCODING_UTF8);
                        if (mStrataNamespace.getLength() == 0) 
                        {
                            throw lang::IllegalArgumentException(
                                rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(
                                                  "No namespace in argument")),
                                *this, 0) ;
                        }
                    }
                    else
                    {//not present in string
                        throw lang::IllegalArgumentException(
                            rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(
                                              "No namespace in argument")),
                            *this, 0) ;                    
                    }
                    break; 
                }            
                //FIXME:
                //
                //  there's some checkOptionalArg stuff in localbe, but we ignore that for now
                //
            }
        }
    
        //
        //  One wonders if it would be better to specify the ownerentity
        //  as /path/to/database.db::namespace
        //
        rtl::OUString SAL_CALL BaseStratum::getOwnerEntity() 
            throw (uno::RuntimeException) 
        {
            return rtl::OStringToOUString(mStrataNamespace, RTL_TEXTENCODING_UTF8);
        }

        rtl::OUString SAL_CALL BaseStratum::getAdminEntity() 
            throw (uno::RuntimeException) 
        {
            return rtl::OUString();
        }


        //  FIXME?
        //   the implication of the localbe version of this is that we can
        //   specify a layer without a stratadataurl, which we can't do
        //   the same sort of thing
        //
        sal_Bool SAL_CALL BaseStratum::supportsEntity( const rtl::OUString& aEntity )
            throw (backend::BackendAccessException, uno::RuntimeException) 
        {
            if(mStrataNamespace.getLength() == 0)
            {
                return false;
            }
            if (aEntity.getLength() == 0)
            {
                return false;
            }
            return aEntity.equals(rtl::OStringToOUString(mStrataNamespace, RTL_TEXTENCODING_UTF8));
        } 

        sal_Bool SAL_CALL BaseStratum::isEqualEntity(const rtl::OUString& aEntity, 
                                                     const rtl::OUString& aOtherEntity) 
            throw (backend::BackendAccessException, lang::IllegalArgumentException, uno::RuntimeException)
        {
            if (aEntity.getLength() == 0)
            {
                rtl::OUString const sMsg(RTL_CONSTASCII_USTRINGPARAM(
                                             "LocalSingleBackend - Invalid empty entity."));
            
                throw lang::IllegalArgumentException(sMsg, *this, 1);
            }
            if (aOtherEntity.getLength() == 0)
            {
                rtl::OUString const sMsg(RTL_CONSTASCII_USTRINGPARAM(
                                             "LocalSingleBackend - Invalid empty entity."));
            
                throw lang::IllegalArgumentException(sMsg, *this, 2);
            }
            return aEntity == aOtherEntity;
        }
 
        rtl::OUString SAL_CALL BaseStratum::getImplementationName() 
            throw (uno::RuntimeException) 
        {
            return ServiceInfoHelper(getServiceInfoData()).getImplementationName() ;
        }

        sal_Bool SAL_CALL BaseStratum::supportsService(const rtl::OUString& aServiceName) 
            throw (uno::RuntimeException) 
        {
            return  ServiceInfoHelper(getServiceInfoData()).supportsService(aServiceName);
        }

        uno::Sequence<rtl::OUString> SAL_CALL BaseStratum::getSupportedServiceNames() 
            throw (uno::RuntimeException) 
        {
            return ServiceInfoHelper(getServiceInfoData()).getSupportedServiceNames() ;
        }
    
        void BaseStratum::failReadonly() 
        {
            rtl::OUStringBuffer aMessage;
            aMessage.appendAscii("Configurations - ")
                .appendAscii("Cannot get update access to layer: ")
                .appendAscii("Local file-based stratum at ")
                .appendAscii(this->getNamespace().getStr())
                .appendAscii(" is readonly.");
            throw lang::NoSupportException(aMessage.makeStringAndClear(),*this);
        }
    
        sal_Bool BaseStratum::isMoreRecent(const rtl::OString& aKey,
                                           const rtl::OUString& aTimestamp)
        {
            return !aKey.getLength() || !BaseLayer::getTimestamp(getDatabase(), aKey).equals(aTimestamp);
        }
    
        rtl::OString ComponentToKey(const rtl::OString &aNamespace, 
                                    const rtl::OString &aComponent,
                                    const rtl::OString &aSubLayer)
        {
            //FIXME const part in too many places
            static const rtl::OString kSeperator(RTL_CONSTASCII_STRINGPARAM("::"));
            rtl::OStringBuffer aKey(aNamespace.getLength() + kSeperator.getLength() +
                                    aComponent.getLength() + kSeperator.getLength() +
                                    aSubLayer.getLength());
            aKey.append(aNamespace);
            aKey.append(kSeperator);
            aKey.append(aComponent);
            aKey.append(kSeperator);
            aKey.append(aSubLayer);
            return aKey.makeStringAndClear();
        }

        rtl::OString ComponentToKey(const rtl::OString &aNamespace, 
                                    const rtl::OUString &aComponent, 
                                    const rtl::OString &aSubLayer)
        {
            return ComponentToKey(aNamespace, 
                                  rtl::OUStringToOString(aComponent, RTL_TEXTENCODING_UTF8),
                                  aSubLayer);
        }

        sal_Bool BaseStratum::hasSubLayers(const rtl::OString& aKey, sal_uInt32 &numSubLayers) 
            throw(backend::BackendAccessException)
        {
            Db& aDatabase= getDatabase();
            Dbt Key(const_cast<void*>(static_cast<const void*>(aKey.getStr())), //kludge!!
                    aKey.getLength());
            Dbt Data;
            Data.set_flags(DB_DBT_PARTIAL);
            Data.set_dlen(sizeof(Record));
            Data.set_doff(0);

            int ret;
            ret= aDatabase.get(NULL, &Key, &Data, 0);
            switch (ret)
            {
                case 0: //success
                {
                    Record* pRecord= static_cast<Record*>(Data.get_data());
                    int swap= 0;  //dummy value to keep compiler from complaining
                    OSL_ASSERT(!aDatabase.get_byteswapped(&swap));
                    if (swap)
                        pRecord->bytesex();
                    numSubLayers= pRecord->numSubLayers;
                }
                break;
                
                case DB_NOTFOUND:
                    return false;
                    break;
                                       
                default: //something bad happened
                    rtl::OUStringBuffer errbuf;
                    errbuf.appendAscii("SingleStratum::getLayer: ");
                    errbuf.appendAscii("Error fetching Key: \"");
                    errbuf.appendAscii(aKey.getStr());
                    errbuf.appendAscii("\" ");
                    errbuf.appendAscii("Error: ");
                    errbuf.append(static_cast<sal_Int32>(ret));
                    rtl::OUString const errmsg = errbuf.makeStringAndClear();
                    throw backend::BackendAccessException(errmsg,*this,uno::Any());
                    break;
            }
            return true;
        }
    
    }; //namespace dbbe
}; //namespace configmgr
