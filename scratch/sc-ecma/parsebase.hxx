#ifndef SC_ECMA_PARSEBASE_HXX
#define SC_ECMA_PARSEBASE_HXX

#include "filter.hxx"
#include "scerrors.hxx"
#include <cppuhelper/implbase1.hxx>
#include <com/sun/star/xml/sax/XDocumentHandler.hpp>

#define css ::com::sun::star

class ScPathRefHolder;

class ScECMAParseBase : public ::cppu::WeakImplHelper1<
	css::xml::sax::XDocumentHandler >
{
  private:
	ScPathRefHolder *m_pPath;
  public:
	ScECMAParseBase() : m_pPath(NULL) {}
	virtual ~ScECMAParseBase();

	FltError doParse(const css::uno::Reference<css::lang::XMultiServiceFactory> &xServiceFactory,
					 const css::uno::Reference<css::embed::XStorage> &xStorage,
					 const rtl::OUString &rStreamPath);

	// css::xml::sax::XDocumentHandler
	virtual void SAL_CALL startDocument(void)
		throw( css::xml::sax::SAXException, css::uno::RuntimeException ) {}
	virtual void SAL_CALL endDocument(void)
		throw( css::xml::sax::SAXException, css::uno::RuntimeException ) {}
	virtual void SAL_CALL startElement(const ::rtl::OUString& aName,
									   const css::uno::Reference< css::xml::sax::XAttributeList > & xAttribs)
		throw( css::xml::sax::SAXException, css::uno::RuntimeException );
	virtual void SAL_CALL endElement(const ::rtl::OUString& aName)
		throw( css::xml::sax::SAXException, css::uno::RuntimeException ) {}
	virtual void SAL_CALL characters(const ::rtl::OUString& aChars)
		throw( css::xml::sax::SAXException, css::uno::RuntimeException ) {}
	virtual void SAL_CALL ignorableWhitespace(const ::rtl::OUString& aWhitespaces)
		throw( css::xml::sax::SAXException, css::uno::RuntimeException ) {}
	virtual void SAL_CALL processingInstruction(const ::rtl::OUString& aTarget,
												const ::rtl::OUString& aData)
		throw( css::xml::sax::SAXException, css::uno::RuntimeException ) {}
	virtual void SAL_CALL setDocumentLocator(const css::uno::Reference< css::xml::sax::XLocator > & xLocator)
		throw( css::xml::sax::SAXException, css::uno::RuntimeException ) {}
};

#endif // SC_ECMA_PARSEBASE_HXX
