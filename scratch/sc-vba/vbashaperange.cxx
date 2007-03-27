#include <com/sun/star/drawing/XShapeGrouper.hpp>
#include <com/sun/star/drawing/XDrawPage.hpp>

#include "vbashaperange.hxx"
#include "vbashape.hxx"

using namespace ::org::openoffice;
using namespace ::com::sun::star;

ScVbaShapeRange::ScVbaShapeRange( const uno::Reference< vba::XHelperInterface >& xParent, const uno::Reference< uno::XComponentContext >& xContext, uno::Reference< drawing::XShapes > xShapes, uno::Reference< drawing::XDrawPage > xDrawPage ) : ScVbaShapeRange_BASE( xParent, xContext ), m_xShapes( xShapes ), m_xDrawPage( xDrawPage ), m_nShapeGroupCount(0)
{
}

// Methods
void SAL_CALL 
ScVbaShapeRange::Select(  ) throw (uno::RuntimeException)
{
    uno::Reference< frame::XModel > xModel( getCurrentDocument(), uno::UNO_QUERY_THROW );
    uno::Reference< view::XSelectionSupplier > xSelectSupp( xModel->getCurrentController(), uno::UNO_QUERY_THROW );
    xSelectSupp->select( uno::makeAny( m_xShapes ) );
}

uno::Reference< msform::XShape > SAL_CALL 
ScVbaShapeRange::Group() throw (uno::RuntimeException)
{
    uno::Reference< drawing::XShapeGrouper > xShapeGrouper( m_xDrawPage, uno::UNO_QUERY_THROW );
    uno::Reference< drawing::XShapeGroup > xShapeGroup( xShapeGrouper->group( m_xShapes ), uno::UNO_QUERY_THROW );
    uno::Reference< drawing::XShape > xShape( xShapeGroup, uno::UNO_QUERY_THROW );
    return uno::Reference< msform::XShape >( new ScVbaShape( getParent(), mxContext, xShape ) );
}
sal_Int32 SAL_CALL 
ScVbaShapeRange::Count(  ) throw (uno::RuntimeException)
{
    uno::Reference< container::XIndexAccess > xIndexAccess( m_xShapes, uno::UNO_QUERY_THROW );
    return xIndexAccess->getCount();
}

rtl::OUString& 
ScVbaShapeRange::getServiceImplName()
{
	static rtl::OUString sImplName( RTL_CONSTASCII_USTRINGPARAM("ScVbaShapeRange") );
	return sImplName;
}

uno::Sequence< rtl::OUString > 
ScVbaShapeRange::getServiceNames()
{
	static uno::Sequence< rtl::OUString > aServiceNames;
	if ( aServiceNames.getLength() == 0 )
	{
		aServiceNames.realloc( 1 );
		aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("org.openoffice.msform.ShapeRange" ) );
	}
	return aServiceNames;
}
