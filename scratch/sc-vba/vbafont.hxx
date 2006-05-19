#ifndef SC_VBA_FONT_HXX
#define SC_VBA_FONT_HXX

#include <cppuhelper/implbase1.hxx>

#include <org/openoffice/vba/XFont.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include "vbahelper.hxx"
#include "vbapalette.hxx"

class ScTableSheetsObj;

typedef  ::cppu::WeakImplHelper1< oo::vba::XFont > ScVbaFont_BASE;

class ScVbaFont : public ScVbaFont_BASE 
{
    css::uno::Reference< css::beans::XPropertySet > mxFont;
    ScVbaPalette mPalette;
public:
	ScVbaFont( const ScVbaPalette& dPalette, css::uno::Reference< css::beans::XPropertySet > xPropertySet ) throw ( css::uno::RuntimeException ) : mPalette( dPalette ), mxFont( xPropertySet, css::uno::UNO_QUERY_THROW ){}
	virtual ~ScVbaFont() {}

	// Attributes
	virtual void SAL_CALL setBold( sal_Bool bValue ) throw (css::uno::RuntimeException);
	virtual sal_Bool SAL_CALL getBold() throw (css::uno::RuntimeException);
	virtual void SAL_CALL setUnderline( const css::uno::Any& aValue ) throw (css::uno::RuntimeException);
	virtual css::uno::Any SAL_CALL getUnderline() throw (css::uno::RuntimeException);
	virtual void SAL_CALL setStrikethrough( sal_Bool bValue ) throw (css::uno::RuntimeException);
	virtual sal_Bool SAL_CALL getStrikethrough() throw (css::uno::RuntimeException);
	virtual void SAL_CALL setShadow( sal_Bool bValue ) throw (css::uno::RuntimeException);
	virtual sal_Bool SAL_CALL getShadow() throw (css::uno::RuntimeException);
	virtual void SAL_CALL setItalic( sal_Bool bValue ) throw (css::uno::RuntimeException);
	virtual sal_Bool SAL_CALL getItalic() throw (css::uno::RuntimeException);
	virtual void SAL_CALL setName( const ::rtl::OUString &rString ) throw (css::uno::RuntimeException);
	virtual ::rtl::OUString SAL_CALL getName() throw (css::uno::RuntimeException);
	virtual void SAL_CALL setSubscript( sal_Bool bValue ) throw (css::uno::RuntimeException);
	virtual sal_Bool SAL_CALL getSubscript() throw (css::uno::RuntimeException);
	virtual void SAL_CALL setSuperscript( sal_Bool bValue ) throw (css::uno::RuntimeException);
	virtual sal_Bool SAL_CALL getSuperscript() throw (css::uno::RuntimeException);
	virtual void SAL_CALL setStandardFontSize( float dValue ) throw (css::uno::RuntimeException);
	virtual float SAL_CALL getStandardFontSize() throw (css::uno::RuntimeException);
	virtual void SAL_CALL setStandardFont( const ::rtl::OUString &rString ) throw (css::uno::RuntimeException);
	virtual ::rtl::OUString SAL_CALL getStandardFont() throw (css::uno::RuntimeException);
        virtual void SAL_CALL setColorIndex( sal_Int32 bValue ) throw (css::uno::RuntimeException);
        virtual sal_Int32 SAL_CALL getColorIndex() throw (css::uno::RuntimeException);
        virtual void SAL_CALL setSize( float fValue ) throw (css::uno::RuntimeException);
        virtual float SAL_CALL getSize() throw (css::uno::RuntimeException);
        virtual void SAL_CALL setFontStyle( const ::rtl::OUString &rString ) throw (css::uno::RuntimeException);
        virtual ::rtl::OUString SAL_CALL getFontStyle() throw (css::uno::RuntimeException);

};



#endif /* SC_VBA_FONT_HXX */

