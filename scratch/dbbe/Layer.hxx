#ifndef CONFIMGR_DBBE_LAYER_HXX_
#define CONFIMGR_DBBE_LAYER_HXX_

#include <db_cxx.h>

#include <Record.hxx>

#ifndef CONFIGMGR_MISC_PROPERTYSETHELPER_HXX
#include "propertysethelper.hxx"
#endif 

#ifndef _COM_SUN_STAR_CONFIGURATION_BACKEND_XUPDATABLELAYER_HPP_
#include <com/sun/star/configuration/backend/XUpdatableLayer.hpp>
#endif // _COM_SUN_STAR_CONFIGURATION_BACKEND_XUPDATABLELAYER_HPP_

#ifndef _COM_SUN_STAR_CONFIGURATION_BACKEND_XCOMPOSITELAYER_HPP_
#include <com/sun/star/configuration/backend/XCompositeLayer.hpp>
#endif // _COM_SUN_STAR_CONFIGURATION_BACKEND_XCOMPOSITELAYER_HPP_

#ifndef _COM_SUN_STAR_UTIL_XTIMESTAMPED_HPP_
#include <com/sun/star/util/XTimeStamped.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif // _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_

#ifndef _CPPUHELPER_IMPLBASE3_HXX_
#include <cppuhelper/implbase3.hxx>
#endif 
#ifndef _CPPUHELPER_IMPLBASE2_HXX_
#include <cppuhelper/implbase2.hxx>
#endif
#ifndef _CPPUHELPER_IMPLBASE1_HXX_
#include <cppuhelper/implbase1.hxx>
#endif

namespace configmgr { namespace dbbe {

namespace css = com::sun::star ;
namespace uno = css::uno ;
namespace lang = css::lang ;
namespace util = css::util ;
namespace backend = css::configuration::backend ;

//stolen verbatim from localbe
// provides properties for file layers
class LayerPropertyHelper : public apihelper::PropertySetHelper
{
protected:
    LayerPropertyHelper(){};
    virtual ~LayerPropertyHelper(){};

protected:
    virtual rtl::OUString const & getLayerUrl() const = 0;

protected:
    // cppu::OPropertySetHelper 
    virtual cppu::IPropertyArrayHelper * SAL_CALL newInfoHelper();

	virtual void SAL_CALL setFastPropertyValue_NoBroadcast( sal_Int32 nHandle, const uno::Any& rValue )
		    throw (uno::Exception);

    virtual void SAL_CALL getFastPropertyValue( uno::Any& rValue, sal_Int32 nHandle ) const;
};


/**
   Base implimentation of XLayer interfaces, just like localbe
 */
class BaseLayer
{
public:
    rtl::OString getKey() const;
    Db& getDb() const;
    static rtl::OUString getTimestamp(Db& aDatabase, rtl::OString aKey);

protected:
    /** 
        Constructor providing the Record to access and factory
        
        @param xFactory    service factory for XML parser
        @param aDatabase   BerkeleyDB that record is in
        @param aKey        the key containing Configuration Item
    */
    BaseLayer(
        const uno::Reference<lang::XMultiServiceFactory>& xFactory,
        Db& aDatabase,
        const rtl::OString aKey);

    /** Destructor */
    ~BaseLayer();
    
    /**
       Describes the contents of a particular Record to a handler.
       
       @param xHandler     handler to describe the data to
       @param aDb          Database
       @param aKey         the key containing Configuration Item
       @throws com::sun::star::configuration::backend::MalformedDataException
                  if the file contains invalid data.
       @throws com::sun::star::lang::NullPointerException
                  if pContext is NULL.
       @throws com::sun::star::lang::WrappedTargetException
       if an error occurs while accessing the data.
    */
    void readData(backend::XLayer * pContext,
                  const uno::Reference<backend::XLayerHandler>& xHandler,
                  Db& aDb,
                  rtl::OString aKey)
        throw (backend::MalformedDataException, lang::NullPointerException, 
               lang::WrappedTargetException, uno::RuntimeException);
    /**
       Returns an object that can be used to write a layer.
    */
    uno::Reference<backend::XLayerHandler> createLayerWriter();

private :
    /** Service factory */
    uno::Reference<lang::XMultiServiceFactory> const mFactory ;
    /** Database reference */
    Db& mDb;
    /** Key of Configuration Item being accessed */
    rtl::OString mKey;
    /** XLayer implementation used for readData */
    uno::Reference<backend::XLayer> mLayerReader ;
};

/**
   XLayer (read-only)
   and XTimeStamped
 */
class Layer : public BaseLayer, 
              public cppu::ImplInheritanceHelper2<LayerPropertyHelper,
              backend::XLayer, util::XTimeStamped>
{
public:
    /**
       Constructor providing the database and record number 
       describing the item to access.
       
       @param xFactory   service factory used to access canned services
       @param aDatabase  database reference
       @param aKey       key of config item
     */
    Layer(
        const uno::Reference<lang::XMultiServiceFactory>& xFactory,
        Db& aDatabase,
        rtl::OString aKey);
    
    /** Descructor */
    ~Layer();
    
    // XLayer
    virtual void SAL_CALL readData(
        const uno::Reference<backend::XLayerHandler>& xHandler) 
        throw (backend::MalformedDataException, lang::NullPointerException, 
               lang::WrappedTargetException, uno::RuntimeException);
    
    // XTimeStamped
    virtual rtl::OUString SAL_CALL getTimestamp() 
        throw (uno::RuntimeException);
};

/**
   Implemenation of XUpdatableLayer
   and
   XTimeStamped
*/
class UpdatableLayer: public BaseLayer,
                      public cppu::ImplInheritanceHelper2<LayerPropertyHelper,
                      backend::XUpdatableLayer, 
                      util::XTimeStamped>
{
public:
    /**
       Constructor providing the database and record number 
       describing the item to access.
       
       @param xFactory   service factory used to access canned services
       @param aDatabase  database reference
       @param aKey       key of config item
     */
    UpdatableLayer(
        const uno::Reference<lang::XMultiServiceFactory>& xFactory,
        Db& aDatabase,
        rtl::OString aKey);
    
    /** Descructor */
    ~UpdatableLayer(void);
    
    // XLayer
    virtual void SAL_CALL readData(
        const uno::Reference<backend::XLayerHandler>& xHandler) 
        throw (backend::MalformedDataException, lang::NullPointerException, 
               lang::WrappedTargetException, uno::RuntimeException);

    // XUpdatableLayer
    virtual void SAL_CALL replaceWith(
        const uno::Reference<backend::XLayer>& aNewLayer) 
        throw (backend::MalformedDataException, lang::NullPointerException, 
               lang::WrappedTargetException, uno::RuntimeException);

    // XTimeStamped
    virtual rtl::OUString SAL_CALL getTimestamp() 
        throw (uno::RuntimeException);

private:
    /** XLayerHandler implementation for getWriteHandler */
    uno::Reference<backend::XLayerHandler> mLayerWriter ;
};                       

/**
   Implementation of the XCompositeLayer
   interface for a local file access.
   The read data is accessible through a canned implementation of
   an XML parser.
   The layer is defined by the URL of the file containing its 
   contents, and that file will be either read or updated by 
   the access to the handlers.
   The timestamp is refreshed on each read operation only.
  */
class BaseCompositeLayer : public BaseLayer
{
public:
    typedef std::vector<rtl::OString> SubLayerKeys;
protected:
    /**
       Constructor providing the base directory and the
       file subpath describing the file to access.
       An resource directory provides the location
       of sublayers of the component.
       
       @param xFactory   service factory used to access canned services
       @param aDatabase  BerkelyDB that record is in
       @param aKey       the key containing the Configuration Item
    */
    BaseCompositeLayer(
        const uno::Reference<lang::XMultiServiceFactory>& xFactory,
        Db& aDatabase,
        const rtl::OString aKey);
    
    // XCompositeLayer helpers
    uno::Sequence<rtl::OUString> SAL_CALL listSubLayerIds()
        throw (lang::WrappedTargetException, uno::RuntimeException)
        { return mSubLayerIds; }
    
    void SAL_CALL readSubLayerData(
        backend::XCompositeLayer * pContext,
        const uno::Reference<backend::XLayerHandler>& xHandler,
        const rtl::OUString& aSubLayerId) 
        throw (backend::MalformedDataException, lang::NullPointerException, 
               lang::WrappedTargetException, lang::IllegalArgumentException,
               uno::RuntimeException);
#if 0 
    //I don't think we should use this.
    /**
       Fills the list of available sublayers.
       
       @param aParentKeys    Config Items holding potential Sublayers
       @param aComponent     component subpath
    */
    void fillSubLayerLists(const SubLayerKeys& aParentKeys,
                           const rtl::OUString& aComponent) ;
#endif

    /**
       You give it a Parent Key, and it will populate
       the list of SubLayerIds and SubLayerKeys
       
       @param aParentKey   ConfigItem that is the parent of the sublayers
     */
    void findSubLayers(const rtl::OString& aParentKey);

private :
    /** List of the sublayerIds... */
    uno::Sequence<rtl::OUString> mSubLayerIds;
    /** .. and the corresponding Keys */
    SubLayerKeys mSubLayerKeys;
};

/**
  Implementation of the XCompositeLayer
  interface for a database access.
  The read data is accessible through a canned implementation of
  an XML parser.
  The layer is defined by the Key of the item containing its 
  contents, and that file will be either read or updated by 
  the access to the handlers.
  The timestamp is refreshed on each read operation only.
  */
class CompositeLayer : public BaseCompositeLayer
                     , public cppu::WeakImplHelper1< backend::XCompositeLayer> 
{
public :
    /**
      Constructor providing the base directory and the
      file subpath describing the file to access.
      An resource directory provides the location
      of sublayers of the component.

      @param xFactory   service factory used to access canned services
      @param aDatabase  reference to the database the key is in
      @param aKey       Key of the Parent for which their may be sublayers
      */
    CompositeLayer(
        const uno::Reference<lang::XMultiServiceFactory>& xFactory,
        Db& aDatabase,
        const rtl::OUString& aKey);
    /** Destructor */
    ~CompositeLayer(void) ;
    // XLayer
    virtual void SAL_CALL readData(
            const uno::Reference<backend::XLayerHandler>& xHandler) 
        throw (backend::MalformedDataException, lang::NullPointerException, 
               lang::WrappedTargetException, uno::RuntimeException);

    // XCompositeLayer
    virtual uno::Sequence<rtl::OUString> SAL_CALL listSubLayerIds()
        throw (lang::WrappedTargetException, uno::RuntimeException)
    { return BaseCompositeLayer::listSubLayerIds() ; }
 
    virtual void SAL_CALL readSubLayerData(
            const uno::Reference<backend::XLayerHandler>& xHandler,
            const rtl::OUString& aSubLayerId) 
        throw (backend::MalformedDataException, lang::NullPointerException, 
               lang::WrappedTargetException, lang::IllegalArgumentException,
               uno::RuntimeException);
};

/**
  Implementation of the XUpdatableLayer and XCompositeLayer
  interfaces for a database access.
  The read data is accessible through a canned implementation of
  an XML parser, and the write data is defined through a canned
  implementation of an XML writer.
  The layer is defined by the key containing its 
  contents, and that file will be either read or updated by 
  the access to the handlers.

  The timestamp is refreshed on each read operation only.
  */
class UpdatableCompositeLayer : public BaseCompositeLayer
                              , public cppu::ImplInheritanceHelper3
                                <LayerPropertyHelper,
                                backend::XUpdatableLayer,
                                backend::XCompositeLayer,
                                util::XTimeStamped> 
{
public :
    /**
      Constructor providing the base directory and the
      file subpath describing the file to access.
      An resource directory provides the location
      of sublayers of the component.

      @param xFactory   service factory used to access canned services
      @param aDatabase  Database that the Config Item is in
      @param aKey       Key of the Parent for which there may be sublayers
      */
    UpdatableCompositeLayer(
            const uno::Reference<lang::XMultiServiceFactory>& xFactory,
            Db& aDatabase,
            const rtl::OString& aKey);
    /** Destructor */
    ~UpdatableCompositeLayer(void) ;
    // XLayer
    virtual void SAL_CALL readData(
            const uno::Reference<backend::XLayerHandler>& xHandler) 
        throw (backend::MalformedDataException, lang::NullPointerException, 
               lang::WrappedTargetException, uno::RuntimeException);

    // XUpdatableLayer
    virtual void SAL_CALL replaceWith(
            const uno::Reference<backend::XLayer>& aNewLayer) 
        throw (backend::MalformedDataException, lang::NullPointerException, 
               lang::WrappedTargetException, uno::RuntimeException);

    // XCompositeLayer
    virtual uno::Sequence<rtl::OUString> SAL_CALL listSubLayerIds()
        throw (lang::WrappedTargetException, uno::RuntimeException)
    { return BaseCompositeLayer::listSubLayerIds() ; }
 
    virtual void SAL_CALL readSubLayerData(
            const uno::Reference<backend::XLayerHandler>& xHandler,
            const rtl::OUString& aSubLayerId) 
        throw (backend::MalformedDataException, lang::NullPointerException, 
               lang::WrappedTargetException, lang::IllegalArgumentException,
               uno::RuntimeException);

    // XTimeStamped
    virtual rtl::OUString SAL_CALL getTimestamp() 
        throw (uno::RuntimeException);

private :
    /** XLayerHandler implementation for getWriteHandler */
    uno::Reference<backend::XLayerHandler> mLayerWriter ;
};

}}; //configmgr::dbbe

#endif //CONFIMGR_DBBE_LAYER_HXX_
