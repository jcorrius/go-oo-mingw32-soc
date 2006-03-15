#include "Layer.hxx"

#ifndef _COMPHELPER_SEQSTREAM_HXX
#include <comphelper/seqstream.hxx>
#endif // _COMPHELPER_SEQSTREAM_HXX

#ifndef _CONFIGMGR_OSLSTREAM_HXX_
#include "oslstream.hxx"
#endif // _CONFIGMGR_OSLSTREAM_HXX_

#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif // _RTL_USTRBUF_HXX_

#ifndef _OSL_TIME_H_
#include <osl/time.h>
#endif//_OSL_TIME_H_

#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif //_OSL_DIAGNOSE_H_

#ifndef _RTL_ALLOC_H_
#include <rtl/alloc.h>
#endif //_RTL_ALLOC_H_

#ifndef _COM_SUN_STAR_IO_XACTIVEDATASOURCE_HPP_
#include <com/sun/star/io/XActiveDataSource.hpp>
#endif
#ifndef _COM_SUN_STAR_IO_XACTIVEDATASINK_HPP_
#include <com/sun/star/io/XActiveDataSink.hpp>
#endif
#ifndef _COM_SUN_STAR_CONFIGURATION_BACKEND_BACKENDACCESSEXCEPTION_HPP_
#include <com/sun/star/configuration/backend/BackendAccessException.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYATTRIBUTE_HPP_
#include <com/sun/star/beans/PropertyAttribute.hpp>
#endif

namespace configmgr { namespace dbbe {

//Lifted directly from localfilelayer
//Should possibly modularize better
#define PROPNAME( name ) rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( name ) )
#define PROPTYPE( type ) getCppuType( static_cast< type const *>( 0 ) )

// cppu::OPropertySetHelper 
cppu::IPropertyArrayHelper * SAL_CALL LayerPropertyHelper::newInfoHelper()
{
    using com::sun::star::beans::Property;
    using namespace com::sun::star::beans::PropertyAttribute;

    Property properties[] = {};

    return new cppu::OPropertyArrayHelper(properties, sizeof(properties)/sizeof(properties[0]));
}

#define MESSAGE( text ) rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "ERROR: Layer Properties: " text ) )

void SAL_CALL LayerPropertyHelper::setFastPropertyValue_NoBroadcast( sal_Int32 nHandle, const uno::Any& /*rValue*/ )
		    throw (uno::Exception)
{
    namespace beans = com::sun::star::beans;
    
    OSL_ENSURE(false, "Error: trying to set an UNKNOWN property");
    throw beans::UnknownPropertyException(MESSAGE("Trying to set an unknown property"),*this);
}

void SAL_CALL LayerPropertyHelper::getFastPropertyValue( uno::Any& rValue, sal_Int32 nHandle ) const
{
    OSL_ENSURE(false, "Error: trying to get an UNKNOWN property");
}
//end lift


//Lifted from localbe.  Should be inlined itself?
static inline void readEmptyLayer(const uno::Reference<backend::XLayerHandler>& xHandler)
{
    OSL_ASSERT(xHandler.is());
    xHandler->startLayer();
    xHandler->endLayer();
}


BaseLayer::BaseLayer(
    const uno::Reference<lang::XMultiServiceFactory>& xFactory,
    Db& aDatabase,
    rtl::OString aKey)
    :mFactory(xFactory),
     mDb(aDatabase)
{
    mKey= aKey;
    static const rtl::OUString kXMLLayerParser(RTL_CONSTASCII_USTRINGPARAM(
                                                   "com.sun.star.configuration.backend.xml.LayerParser")) ;
    
    mLayerReader = uno::Reference<backend::XLayer>::query(
        mFactory->createInstance(kXMLLayerParser)) ;
    
}

BaseLayer::~BaseLayer()
{
}


void BaseLayer::readData(backend::XLayer * pContext,
                         const uno::Reference<backend::XLayerHandler>& xHandler,
                         Db& aDb,
                         rtl::OString aKey)
    throw (backend::MalformedDataException, lang::NullPointerException, 
           lang::WrappedTargetException, uno::RuntimeException)
{
    if (!xHandler.is())
    {
        rtl::OUString const sMessage(RTL_CONSTASCII_USTRINGPARAM(
            "BaseLayer - Cannot readData: Handler is NULL."));

        throw  lang::NullPointerException(sMessage,pContext);
    }
    
    Dbt data;
    Dbt key((void*)aKey.getStr(), aKey.getLength());
    
    int ret;
    //  We use a non-elegant error method because we cannot be sure
    //  that berkeleydb was compiled with exceptions enabled.
    try
    {
        ret= aDb.get(NULL, &key, &data, 0); 
    } catch(DbException &dbe)
    {
        ret= dbe.get_errno();
    }
    switch(ret)
    {
        using namespace comphelper;
        
        case 0: //found
        {
            Record* pRecord= static_cast<Record*>(data.get_data());
            pRecord->unMarshall();
            ByteSequence BS((const sal_Int8*)pRecord->pBlob, pRecord->blobSize);
            uno::Reference<io::XActiveDataSink> xAS(mLayerReader, uno::UNO_QUERY_THROW);
            uno::Reference<io::XInputStream> xStream(new SequenceInputStream(BS));
            xAS->setInputStream(xStream);
            mLayerReader->readData(xHandler);
            //FIXME: leak?  reentrancy?
        }
        break;
            
        case DB_NOTFOUND:
            readEmptyLayer(xHandler);
            break;
            
        default: //some other error
            rtl::OUStringBuffer sMsg(80);            
            sMsg.appendAscii("dbbe Layer got code ");
            sMsg.append(static_cast<sal_Int64>(ret));
            sMsg.appendAscii(" when getting record \"");
            sMsg.appendAscii(aKey.getStr());
            sMsg.appendAscii("\"\n");
            throw backend::BackendAccessException(sMsg.makeStringAndClear(), pContext, uno::Any());
            break;
            
    }
}

rtl::OString BaseLayer::getKey(void) const
{
    return mKey;
}

Db& BaseLayer::getDb(void) const
{
    return mDb;
}

rtl::OUString BaseLayer::getTimestamp(Db& aDatabase, rtl::OString aKey)
{   
    Dbt data;
    Dbt key(const_cast<sal_Char*>(aKey.getStr()), aKey.getLength());
    rtl::OUString retCode;

    key.set_flags(DB_DBT_PARTIAL);
    key.set_dlen(sizeof(Record));
    key.set_doff(0);

    int ret;
    //  We use a non-elegant error method because we cannot be sure
    //  that berkeleydb was compiled with exceptions enabled.
    try
    {
        ret= aDatabase.get(NULL, &key, &data, 0); 
    } catch(DbException &dbe)
    {
        ret= dbe.get_errno();
    }
    if (!ret)
    {
        //
        //  It is said that timestamps are up to our discretion on how they are
        //  created.  We choose to concatinate 64-bit unix epoch time and 
        //  64bit blob size
        //
        //  FIXME:
        //    leaks, reentrancy
        rtl::OUStringBuffer timestamp(50);
        
        Record* pRecord= static_cast<Record*>(data.get_data());
        timestamp.append(pRecord->date);
        timestamp.append(static_cast<sal_Int64>(pRecord->blobSize));
        retCode= timestamp.makeStringAndClear();
    }
    else
    {
        rtl::OUStringBuffer sMsg(80);            
        sMsg.appendAscii("dbbe Layer got code ");
        sMsg.append(static_cast<sal_Int64>(ret));
        sMsg.appendAscii(" when getting record \"");
        sMsg.appendAscii(aKey.getStr());
        sMsg.appendAscii("\"\n");
        
        //compiles, but will it work correctly?
        throw backend::BackendAccessException(sMsg.makeStringAndClear(), NULL, uno::Any());
    }
    return retCode;
}

uno::Reference<backend::XLayerHandler> BaseLayer::createLayerWriter()
{
    static const rtl::OUString kXMLLayerWriter(RTL_CONSTASCII_USTRINGPARAM(
                                                   "com.sun.star.configuration.backend.xml.LayerWriter")) ;
    
    uno::Reference< uno::XInterface > xWriter = mFactory->createInstance(kXMLLayerWriter);
    
    return uno::Reference<backend::XLayerHandler>(xWriter,uno::UNO_REF_QUERY_THROW) ;
}

//----------------------------------------------------
Layer::Layer(
    const uno::Reference<lang::XMultiServiceFactory>& xFactory,
    Db& aDatabase,
    const rtl::OString aKey)
    :BaseLayer(xFactory, aDatabase, aKey)
{
}

Layer::~Layer()
{
}

void SAL_CALL Layer::readData(
    const uno::Reference<backend::XLayerHandler>& xHandler) 
    throw ( backend::MalformedDataException,
            lang::NullPointerException,
            lang::WrappedTargetException, 
            uno::RuntimeException)
{
    BaseLayer::readData(this, xHandler, getDb(), getKey());
}

rtl::OUString Layer::getTimestamp() 
    throw (uno::RuntimeException)
{
    rtl::OUString sStamp = BaseLayer::getTimestamp(getDb(), getKey());
    
    return sStamp;
}

//----------------------------------------------------
UpdatableLayer::UpdatableLayer(
    const uno::Reference<lang::XMultiServiceFactory>& xFactory,
    Db& aDatabase,
    const rtl::OString aKey)
    :BaseLayer(xFactory, aDatabase, aKey)
{
}

UpdatableLayer::~UpdatableLayer()
{
}

void SAL_CALL UpdatableLayer::readData(
    const uno::Reference<backend::XLayerHandler>& xHandler) 
    throw ( backend::MalformedDataException,
            lang::NullPointerException,
            lang::WrappedTargetException, 
            uno::RuntimeException)
{
    BaseLayer::readData(this, xHandler, getDb(), getKey());
}

rtl::OUString UpdatableLayer::getTimestamp() 
    throw (uno::RuntimeException)
{
    rtl::OUString sStamp = BaseLayer::getTimestamp(getDb(), getKey());
    
    return sStamp;
}


void SAL_CALL UpdatableLayer::replaceWith(
    const uno::Reference<backend::XLayer>& aNewLayer) 
    throw ( backend::MalformedDataException,
            lang::NullPointerException,
            lang::WrappedTargetException, 
            uno::RuntimeException)
{
    using comphelper::OSequenceOutputStream;

    if (!aNewLayer.is())
    {
        rtl::OUString const sMessage(RTL_CONSTASCII_USTRINGPARAM(
                                         "LocalFileLayer - Cannot replaceWith: Replacement layer is NULL."));

        throw  lang::NullPointerException(sMessage, *this);
    }
    OSL_ENSURE( !uno::Reference<backend::XCompositeLayer>::query(aNewLayer).is(),
                "Warning: correct updates with composite layers are not implemented");

    uno::Reference<io::XActiveDataSource> xAS(mLayerWriter, uno::UNO_QUERY_THROW);
    

    uno::Sequence<sal_Int8> BS;
    OSequenceOutputStream* pStream= new OSequenceOutputStream(BS);

    uno::Reference<io::XOutputStream> xStream( pStream );
        
    xAS->setOutputStream(xStream);

    aNewLayer->readData(mLayerWriter) ;

    pStream->closeOutput();
 
    Db& aDatabase= getDb();
    Record aRecord;
    TimeValue timeval;
    size_t aSize;

    osl_getSystemTime(&timeval);
    aRecord.date= timeval.Seconds;
    aRecord.blobSize= BS.getLength();
    aRecord.pBlob= (sal_Char*)BS.getArray();
    aRecord.numSubLayers= 0;
    aRecord.pSubLayers= NULL;
    Record* pRecord= aRecord.Marshall(aSize);
    rtl::OString KeyName= getKey();
    Dbt Key(const_cast<void*>(static_cast<const void*>(KeyName.getStr())), //kludge! 
            KeyName.getLength());
    Dbt Data(pRecord, aSize);
    int ret;
    //  We use a non-elegant error method because we cannot be sure
    //  that berkeleydb was compiled with exceptions enabled.
    try
    {
        // The berkelydb documentation is ambiguous about 
        // inserting keys of the same name when duplicates
        // are not enabled.  To be sure, we simply delete
        // the key first
        ret= aDatabase.del(NULL, &Key, 0);
        if (!ret)
        {
            ret= aDatabase.put(NULL, &Key, &Data, 0);
            if (!ret)
                ret= aDatabase.sync(0);
        }
    } catch(DbException &dbe)
    {
        ret= dbe.get_errno();
    }
    if (ret)
    {
        rtl::OUStringBuffer sMsg(80);            
        sMsg.appendAscii("dbbe Layer got code ");
        sMsg.append(static_cast<sal_Int64>(ret));
        sMsg.appendAscii(" when putting/deleting record \"");
        sMsg.appendAscii(KeyName.getStr());
        sMsg.appendAscii("\"\n");        
        //compiles, but will it work correctly?
        throw backend::BackendAccessException(sMsg.makeStringAndClear(), NULL, uno::Any());
    }
    rtl_freeMemory(pRecord);

    // clear the output stream
    xStream.clear();
    xAS->setOutputStream(xStream);
}

//------------------------------------------------------------------------------

BaseCompositeLayer::BaseCompositeLayer(
    const uno::Reference<lang::XMultiServiceFactory>& xFactory,
    Db& aDatabase,
    const rtl::OString aKey): BaseLayer(xFactory, aDatabase, aKey)
{
}

void BaseCompositeLayer::readSubLayerData(
    backend::XCompositeLayer * pContext,
    const uno::Reference<backend::XLayerHandler>& xHandler,
    const rtl::OUString& aSubLayerId) 
    throw (backend::MalformedDataException, lang::NullPointerException, 
           lang::WrappedTargetException, lang::IllegalArgumentException,
           uno::RuntimeException)
{
   if (!xHandler.is())
    {
        rtl::OUString const sMessage(RTL_CONSTASCII_USTRINGPARAM(
            "BaseCompositeLayer - Cannot readSubLayerData: Handler is NULL."));

        throw  lang::NullPointerException(sMessage,pContext);
    }

    sal_Int32 i ; 
    
    for (i = 0 ; i < mSubLayerIds.getLength() ; ++ i) 
    {
        if (mSubLayerIds[i].equals(aSubLayerId)) 
        {
            break;
        }
    }
    if (i == mSubLayerIds.getLength()) 
    { 
        rtl::OUStringBuffer message ;

        message.appendAscii("Sublayer Id '").append(aSubLayerId) ;
        message.appendAscii("' is unknown") ;
        throw lang::IllegalArgumentException(message.makeStringAndClear(),
                                             pContext, 2) ;
    }
    if (mSubLayerKeys[i].getLength() != 0)
        BaseLayer::readData(pContext, xHandler, getDb(), mSubLayerKeys[i]) ;
    else
        readEmptyLayer(xHandler);
}

#if 0
//FIXME
// documentation
// headers
// prototype
//
//  Note that this method assumes that keys without sublayers are of the form:
//   namespace::config_item::
static rtl::OUString getSubLayerName(const rtl::OString Key)
{
    static const rtl::OUString kSeperator(RTL_CONSTASCII_USTRINGPARAM("::"));
    rtl::OUString OUKey= OStringToOUString(Key, RTL_TEXTENCODING_UTF8);
    sal_Int32 lastIndex= OUKey.lastIndexOf(kSeperator);
    rtl::OUString ret= OUKey.copy(lastIndex + kSeperator.getLength());
}
#endif

void BaseCompositeLayer::findSubLayers(const rtl::OString& aParentKey)
{
    Db& aDb= getDb();
    Dbt Key(const_cast<void*>(static_cast<const void*>(aParentKey.getStr())),
            aParentKey.getLength());
    Dbt Data;
    
    int ret;
    try
    {
        ret= aDb.get(NULL, &Key, &Data, 0);
    }
    catch (DbException &dbe)
    {
        ret= dbe.get_errno();
    }
    switch (ret)
    {
        case 0: //found
        {
            Record *pRecord= static_cast<Record*>(Data.get_data());
            OSL_ASSERT(pRecord);
            pRecord->unMarshall();
            const int numSubLayers= pRecord->numSubLayers;
            mSubLayerKeys.resize(numSubLayers);
            mSubLayerIds.realloc(numSubLayers);
            sal_Char* pString= pRecord->pSubLayers;
            for (int i= 0; i < numSubLayers; i++)
            {
                mSubLayerIds[i]= rtl::OUString::createFromAscii(pString);

                //test to see if it is there
                Dbt Key2(pString, strlen(pString));
                Key2.set_flags(DB_DBT_PARTIAL);
                Key2.set_doff(0);
                Key2.set_dlen(0); 
                Dbt Data2;
                int ret2;
                try
                {
                    ret2= aDb.get(NULL, &Key2, &Data2, 0);
                }
                catch (DbException &dbe)
                {
                    ret2= dbe.get_errno();
                }
                switch (ret2)
                {
                    case 0: //found
                        mSubLayerKeys[i]= rtl::OString(pString);
                        break;
                        
                    case DB_NOTFOUND:
                        //This is lifted from localbe and is bizzare to me
                        //shouldn't we just set it to the empty string?
                        //why test if length is zero??
                        OSL_ASSERT(mSubLayerKeys[i].getLength() == 0);
                        break;
                        
                    default: //some other error
                        OSL_ASSERT(0);
                        break;
                }
                pString+= strlen(pString) + 2; //push us over the null
            }
        }
        break;
            
        case DB_NOTFOUND:
            //I'm pretty sure this should never happen
            OSL_ASSERT(0);
            break;
            
        default: //some sort of error
            OSL_ASSERT(0);
            break;
    }
}

//
//  FIXME:
//   I stopped 3/4 of the way through this method b/c 
//     1) the semantics of sublayers is different, in dbbe, in that we know them by looking at the parent
//     2) exactly how this method is used by the constructor isn't totally clear at this moment
//
#if 0
void BaseCompositeLayer::fillSubLayerLists(const SubLayerKeys& aParentKeys,
                                           const rtl::OUString& aComponent)
{
    SubLayerKeys::size_type const nSubLayerCount= aParentKeys.size();
    mSubLayerIds.realloc(nSublayerCount);
    mSubLayerKeys.resize(nSublayerCount);
    
    Db& aDb= getDb();

    for (SubLayerKeys::size_type i = 0; i < nSublayerCount; ++i)
    {
        // In the original version, that is I pass in /foo/bar/baz.xcu and get back baz.xcu
        //   or perhaps foo/bar/baz and get back baz
        // In our version, you pass in namespace::config_item::sublayer and get back sublayer
        mSubLayerIds[i] = getSubLayerName(aParentKeys[i]);
        
        Dbt Key(aParentKeys[i].getStr(), aParentKeys[i].getLength());
        Key.set_flags(DB_DBT_PARTIAL);
        Key.set_dlen= 1;
        Key.set_doff= 0;
        Dbt Data;
        
        int ret;
        try
        {
            aDb.get(NULL, &Key, &Data, 0);
        }
        catch (DbException &dbe)
        {
            ret= dbe.get_errno();
        }
        switch (ret)
        {
            case 0: //record found
                rtl::OUString subLayerKey
                mSubLayerKeys[i]= 
                break;
                
            case DB_NOTFOUND:
                break;
                
            default:
                break;               
        }
        
        /// old stuff below
        
        // Let's check whether the sublayer exists for the
        // particular component.
        rtl::OUString subLayerFile(aSublayerDirectories[i] + aComponent) ;
        if (FileHelper::fileExists(subLayerFile))
        {
            mSubLayerFiles[i] =  subLayerFile;
        }
        else
            OSL_ASSERT(mSubLayerFiles[i].getLength() == 0);
    }
}
#endif


//-----------------------------------------------------------------
CompositeLayer::CompositeLayer(
    const uno::Reference<lang::XMultiServiceFactory>& xFactory,
    Db& aDatabase,
    const rtl::OUString& aKey)
    : BaseCompositeLayer(xFactory, aDatabase, rtl::OString())
{
}

CompositeLayer::~CompositeLayer(void)
{
}

void SAL_CALL CompositeLayer::readData(
    const uno::Reference<backend::XLayerHandler>& xHandler) 
    throw (backend::MalformedDataException, lang::NullPointerException, 
           lang::WrappedTargetException, uno::RuntimeException)
{
    if (!xHandler.is())
    {
        rtl::OUString const sMessage(RTL_CONSTASCII_USTRINGPARAM(
                                         "CompositeLayer - Cannot readData: Handler is NULL."));
        
        throw  lang::NullPointerException(sMessage,*this);
    }
    
    readEmptyLayer(xHandler) ; 
}

void SAL_CALL CompositeLayer::readSubLayerData(
    const uno::Reference<backend::XLayerHandler>& xHandler,
    const rtl::OUString& aSubLayerId) 
    throw (backend::MalformedDataException, lang::NullPointerException, 
           lang::WrappedTargetException, lang::IllegalArgumentException,
           uno::RuntimeException)
{
    return BaseCompositeLayer::readSubLayerData(this,xHandler,aSubLayerId);
}

//----------------------------------------------------
UpdatableCompositeLayer::UpdatableCompositeLayer(
    const uno::Reference<lang::XMultiServiceFactory>& xFactory,
    Db& aDatabase,
    const rtl::OString& aKey)
    :BaseCompositeLayer(xFactory,aDatabase,aKey),
     mLayerWriter(createLayerWriter())
{
}

UpdatableCompositeLayer::~UpdatableCompositeLayer(void)
{
}

void SAL_CALL UpdatableCompositeLayer::readData(
    const uno::Reference<backend::XLayerHandler>& xHandler) 
    throw (backend::MalformedDataException, lang::NullPointerException, 
           lang::WrappedTargetException, uno::RuntimeException)
{
   BaseLayer::readData(static_cast<backend::XCompositeLayer*>(this),xHandler, getDb(), getKey());
}

void SAL_CALL UpdatableCompositeLayer::replaceWith(
    const uno::Reference<backend::XLayer>& aNewLayer) 
    throw (backend::MalformedDataException, lang::NullPointerException, 
           lang::WrappedTargetException, uno::RuntimeException)
{
//copied from UpdatableLayer, which appears to be
// the way localbe does it
    using comphelper::OSequenceOutputStream;
    
    if (!aNewLayer.is())
    {
        rtl::OUString const sMessage(RTL_CONSTASCII_USTRINGPARAM(
                                         "LocalFileLayer - Cannot replaceWith: Replacement layer is NULL."));

        throw  lang::NullPointerException(sMessage, *this);
    }
    OSL_ENSURE( !uno::Reference<backend::XCompositeLayer>::query(aNewLayer).is(),
                "Warning: correct updates with composite layers are not implemented");

    uno::Reference<io::XActiveDataSource> xAS(mLayerWriter, uno::UNO_QUERY_THROW);
    

    uno::Sequence<sal_Int8> BS;
    OSequenceOutputStream* pStream= new OSequenceOutputStream(BS);

    uno::Reference<io::XOutputStream> xStream( pStream );
        
    xAS->setOutputStream(xStream);

    aNewLayer->readData(mLayerWriter) ;

    pStream->closeOutput();
 
    Db& aDatabase= getDb();
    Record aRecord;
    TimeValue timeval;
    size_t aSize;

    osl_getSystemTime(&timeval);
    aRecord.date= timeval.Seconds;
    aRecord.blobSize= BS.getLength();
    aRecord.pBlob= (sal_Char*)BS.getArray();
    aRecord.numSubLayers= 0;
    aRecord.pSubLayers= NULL;
    Record* pRecord= aRecord.Marshall(aSize);
    rtl::OString KeyName= getKey();
    Dbt Key(const_cast<void*>(static_cast<const void*>(KeyName.getStr())), //kludge! 
            KeyName.getLength());
    Dbt Data(pRecord, aSize);
    int ret;
    //  We use a non-elegant error method because we cannot be sure
    //  that berkeleydb was compiled with exceptions enabled.
    try
    {
        // The berkelydb documentation is ambiguous about 
        // inserting keys of the same name when duplicates
        // are not enabled.  To be sure, we simply delete
        // the key first
        ret= aDatabase.del(NULL, &Key, 0);
        if (!ret)
        {
            ret= aDatabase.put(NULL, &Key, &Data, 0);
            if (!ret)
                ret= aDatabase.sync(0);
        }
    } catch(DbException &dbe)
    {
        ret= dbe.get_errno();
    }
    if (ret)
    {
        rtl::OUStringBuffer sMsg(80);            
        sMsg.appendAscii("dbbe Layer got code ");
        sMsg.append(static_cast<sal_Int64>(ret));
        sMsg.appendAscii(" when putting/deleting record \"");
        sMsg.appendAscii(KeyName.getStr());
        sMsg.appendAscii("\"\n");        
        //compiles, but will it work correctly?
        throw backend::BackendAccessException(sMsg.makeStringAndClear(), NULL, uno::Any());
    }
    rtl_freeMemory(pRecord);

    // clear the output stream
    xStream.clear();
    xAS->setOutputStream(xStream);
}

void SAL_CALL UpdatableCompositeLayer::readSubLayerData(
    const uno::Reference<backend::XLayerHandler>& xHandler,
    const rtl::OUString& aSubLayerId) 
    throw (backend::MalformedDataException, lang::NullPointerException, 
           lang::WrappedTargetException, lang::IllegalArgumentException,
           uno::RuntimeException)
{
    return BaseCompositeLayer::readSubLayerData(this,xHandler,aSubLayerId);
}

rtl::OUString SAL_CALL UpdatableCompositeLayer::getTimestamp() 
    throw (uno::RuntimeException)
{
    return BaseLayer::getTimestamp(getDb(), getKey());
}


}}; //configmgr::dbbe namespace
