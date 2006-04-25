#include <stdio.h>
#include <list>
#include <com/sun/star/xml/sax/XParser.hpp>
#include <com/sun/star/xml/sax/InputSource.hpp>
#include <com/sun/star/xml/sax/SAXParseException.hpp>
#include <com/sun/star/embed/ElementModes.hpp>
#include <com/sun/star/embed/XStorage.hpp>
#include <parsebase.hxx>
#include <unotools/streamwrap.hxx>

using namespace com::sun::star;

class ScPathRefHolder
{
	std::list< uno::Reference<embed::XStorage> > m_aUgly;
	uno::Reference<io::XStream> m_xRef;
  public:
	ScPathRefHolder(uno::Reference<embed::XStorage> xStorage,
					rtl::OUString aPath, long nOpenMode)
	{
		rtl::OUString aTok;
		sal_Int32 nIdx = 0;
		uno::Reference<io::XStream> xStream;
		if (aPath[0] == sal_Unicode('/'))
			nIdx++;
		do
		{
			m_aUgly.push_back (xStorage);
			aTok = aPath.getToken (0, sal_Unicode('/'), nIdx);
//			fprintf (stderr, "Tok '%s' (%d)\n",
//					 rtl::OUStringToOString (aTok, RTL_TEXTENCODING_UTF8).getStr(),
//					 nIdx);
			if (nIdx >= 0)
				xStorage = xStorage->openStorageElement (aTok, nOpenMode);
			else
				m_xRef = xStorage->openStreamElement (aTok, nOpenMode);
		}
		while (nIdx >= 0);
	}
	uno::Reference<io::XStream> get()
	{
		return m_xRef;
	}
};

ScECMAParseBase::~ScECMAParseBase()
{
	if (m_pPath)
		delete m_pPath;
}

FltError
ScECMAParseBase::doParse(const uno::Reference<lang::XMultiServiceFactory> &xServiceFactory,
						 const uno::Reference<embed::XStorage> &xStorage,
						 const rtl::OUString &rStreamPath)
{
	// get parser
	uno::Reference<xml::sax::XParser> xParser (
		xServiceFactory->createInstance(
			rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.xml.sax.Parser" )) ),
		uno::UNO_QUERY);
	if (!xParser.is())
		return eERR_OPEN;

	xParser->setDocumentHandler (this);

	xml::sax::InputSource aParserInput;

	m_pPath = new ScPathRefHolder(xStorage, rStreamPath,
								  embed::ElementModes::READ);
						   
	uno::Reference<io::XStream> xDocStream = m_pPath->get();
	if (!xDocStream.is())
		fprintf (stderr, "Foo\n");
	else
		aParserInput.aInputStream = xDocStream->getInputStream();

	aParserInput.sSystemId = rtl::OUString(); // hmm ?

	try
	{
		fprintf (stderr, "Do Parse\n");
		xParser->parseStream( aParserInput );
	}
	catch( xml::sax::SAXParseException& r )
	{
		fprintf (stderr, "SAX parse exception '%s'\n",
				 rtl::OUStringToOString (r.Message, RTL_TEXTENCODING_UTF8).getStr());
	}
#if 0
	catch( packages::WrongPasswordException& )
	{
	}
	catch( packages::zip::ZipIOException& )
	{
	}
	catch( uno::Exception& )
	{
	}
#endif
	return eERR_OK; 
}

void SAL_CALL
ScECMAParseBase::startElement(const ::rtl::OUString& aName,
							  const css::uno::Reference< css::xml::sax::XAttributeList > & xAttribs)
		throw( css::xml::sax::SAXException, css::uno::RuntimeException )
{
	fprintf (stderr, "startElement '%s'\n", rtl::OUStringToOString(aName, RTL_TEXTENCODING_UTF8).getStr());
}
