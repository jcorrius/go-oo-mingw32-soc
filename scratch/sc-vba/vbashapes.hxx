#ifndef SC_VBA_SHAPES_HXX
#define SC_VBA_SHAPES_HXX

#include <com/sun/star/drawing/XShapes.hpp>
#include <com/sun/star/drawing/XDrawPage.hpp>
#include <com/sun/star/container/XIndexAccess.hpp>
#include <org/openoffice/msforms/XShapes.hpp>

#include "vbahelperinterface.hxx"

#include "vbacollectionimpl.hxx"

typedef CollTestImplHelper< oo::msforms::XShapes > ScVbaShapes_BASE;

class ScVbaShapes : public ScVbaShapes_BASE
{
private: 
    css::uno::Reference< css::drawing::XShapes > m_xShapes;
    css::uno::Reference< css::drawing::XDrawPage > m_xDrawPage;
protected:
    virtual css::uno::Any getItemByStringIndex( const rtl::OUString& sIndex ) throw (css::uno::RuntimeException);
	virtual rtl::OUString& getServiceImplName();
	virtual css::uno::Sequence<rtl::OUString> getServiceNames();
    virtual css::uno::Reference< css::drawing::XShape > getByName( rtl::OUString sIndex ) throw (css::uno::RuntimeException);
    virtual css::uno::Reference< css::drawing::XShapes > getEmptyShapesCollection() throw (css::uno::RuntimeException);
    virtual css::uno::Reference< css::drawing::XShapes > getShapeByName( rtl::OUString sIndex ) throw (css::uno::RuntimeException);
    virtual css::uno::Reference< css::drawing::XShapes > getShapeByIndex( sal_Int32  nIndex ) throw (css::uno::RuntimeException);
    virtual css::uno::Reference< css::drawing::XShapes > getShapesByNames( css::uno::Sequence< rtl::OUString > sArray ) throw (css::uno::RuntimeException);
    virtual css::uno::Reference< css::drawing::XShapes > getShapesByIndex( css::uno::Sequence< sal_Int16 > nArray ) throw (css::uno::RuntimeException);
public:
    ScVbaShapes( const css::uno::Reference< oo::vba::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::container::XIndexAccess > xShapes );
    //XEnumerationAccess
    virtual css::uno::Type SAL_CALL getElementType() throw (css::uno::RuntimeException);
    virtual css::uno::Reference< css::container::XEnumeration > SAL_CALL createEnumeration() throw (css::uno::RuntimeException);

    // ScVbaCollectionBaseImpl
    virtual css::uno::Any createCollectionObject( const css::uno::Any& aSource );
    // Methods
    virtual css::uno::Any SAL_CALL Item( const css::uno::Any& NameOrIndex ) throw (css::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL Count() throw (css::uno::RuntimeException);
    virtual void SAL_CALL SelectAll() throw (css::uno::RuntimeException);
    virtual css::uno::Reference< oo::msforms::XShapeRange > SAL_CALL Range( const css::uno::Any& shapes ) throw (css::uno::RuntimeException);
};

#endif//SC_VBA_SHAPES_HXX
