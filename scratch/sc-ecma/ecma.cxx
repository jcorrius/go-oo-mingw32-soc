#include <sal/config.h>
#include <stdio.h>
#include <sfx2/docfile.hxx>

#include "global.hxx"
#include "docpool.hxx" 
#include "patattr.hxx"
#include "document.hxx"
#include "cell.hxx"
#include "biff.hxx"
#include <tools/stream.hxx>
#include <com/sun/star/embed/ElementModes.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <comphelper/processfactory.hxx>
#include <unotools/streamwrap.hxx>
#include <comphelper/processfactory.hxx>
#include <parsebase.hxx>

#define SCHEMA_OFFICEDOCUMENT "http://schemas.microsoft.com/office/2006/relationships/officeDocument"

using namespace com::sun::star;

namespace {
	inline rtl::OUString getBase (const rtl::OUString &rPath)
	{
		return rtl::OUString (rPath.copy (0, rPath.lastIndexOf('/')));
	}
	inline rtl::OUString getSuffix (const rtl::OUString &rPath)
	{
		return rtl::OUString (rPath.copy (rPath.lastIndexOf('/') + 1));
	}
};

class ScECMAParseRel : public ScECMAParseBase
{
	struct Rel {
		rtl::OUString m_aId;
		rtl::OUString m_aType;	
		rtl::OUString m_aTarget;
	};
	std::list<Rel> m_aRels;
  public:
	ScECMAParseRel() :
		ScECMAParseBase(),
		m_aRels()
	{
	}
	virtual ~ScECMAParseRel() {}

	rtl::OUString getDocument()
	{
		std::list<Rel>::const_iterator iter;
		for (iter = m_aRels.begin(); iter != m_aRels.end(); iter++)
		{
			if (iter->m_aType.equalsAscii(SCHEMA_OFFICEDOCUMENT))
				return iter->m_aTarget;
		}
		return rtl::OUString();
	}
	rtl::OUString getTargetById(const rtl::OUString &rId)
	{
		std::list<Rel>::const_iterator iter;
		for (iter = m_aRels.begin(); iter != m_aRels.end(); iter++)
		{
			if (iter->m_aId == rId)
				return iter->m_aTarget;
		}
		return rtl::OUString();
	}
	// rStreamPath is the stream for which to read the rels
	FltError parseRels(const css::uno::Reference<css::lang::XMultiServiceFactory> &xServiceFactory,
					   const css::uno::Reference<css::embed::XStorage> &xStorage,
					   const rtl::OUString &rStreamPath)
	{
		return doParse(xServiceFactory, xStorage,
					   getBase (rStreamPath) + rtl::OUString::createFromAscii("/_rels/")
					   + getSuffix (rStreamPath) + rtl::OUString::createFromAscii(".rels"));
	}
	virtual void SAL_CALL startElement(const ::rtl::OUString& aName,
							  const css::uno::Reference< css::xml::sax::XAttributeList > & xAttribs)
		throw( css::xml::sax::SAXException, css::uno::RuntimeException )
	{
		fprintf (stderr, "startElement '%s'\n", rtl::OUStringToOString(aName, RTL_TEXTENCODING_UTF8).getStr());
		if (aName.equalsAscii ("Relationship"))
		{
			Rel aRel;
			aRel.m_aId = xAttribs->getValueByName(rtl::OUString::createFromAscii("Id"));
			aRel.m_aType = xAttribs->getValueByName(rtl::OUString::createFromAscii("Type"));
			aRel.m_aTarget = xAttribs->getValueByName(rtl::OUString::createFromAscii("Target"));
			m_aRels.push_back (aRel);
		}
	}
};

class ScECMAParseContentTypes : public ScECMAParseBase
{
	struct Override {
		rtl::OUString m_aPartName;
		rtl::OUString m_aType;	
	};
	struct Default {
		rtl::OUString m_aExtension;
		rtl::OUString m_aType;	
	};
	std::list<Default> m_aDefaults;
	std::list<Override> m_aOverrides;
  public:
	ScECMAParseContentTypes() :
		ScECMAParseBase(),
		m_aDefaults(),
		m_aOverrides()
	{
	}
	virtual ~ScECMAParseContentTypes() {}
	rtl::OUString getType (const rtl::OUString &aPath)
	{ // FIXME: implement me
		return rtl::OUString();
	}
	virtual void SAL_CALL startElement (const ::rtl::OUString& aName,
										const css::uno::Reference< css::xml::sax::XAttributeList > & xAttribs)
		throw( css::xml::sax::SAXException, css::uno::RuntimeException )
	{
		fprintf (stderr, "startElement '%s'\n", rtl::OUStringToOString(aName, RTL_TEXTENCODING_UTF8).getStr());
	}
};

class ScECMAParseSheet : public ScECMAParseBase
{
	css::uno::Reference<css::lang::XMultiServiceFactory> m_xServiceFactory;
	css::uno::Reference<css::embed::XStorage> m_xStorage;
	ScDocument *m_pDoc;
	SCTAB m_nTab;

	rtl::OUString m_aBasePath;
	uno::Reference<ScECMAParseRel> m_xRels;

 	// Cell reading
	rtl::OUString m_aContent;
	ScAddress     m_aAddr;
	rtl::OUString m_aType;
  public:
	ScECMAParseSheet(const css::uno::Reference<css::lang::XMultiServiceFactory> &xServiceFactory,
					 const css::uno::Reference<css::embed::XStorage> &xStorage,
					 ScDocument *pDoc,
					 SCTAB nTab) :
			ScECMAParseBase(),
			m_xServiceFactory(xServiceFactory),
			m_xStorage(xStorage),
			m_pDoc(pDoc),
			m_nTab(nTab),
			m_xRels(),
			m_aContent(),
			m_aAddr(0, 0, nTab)
	{
	}
	virtual ~ScECMAParseSheet() {}
	void parseSheet (const rtl::OUString &rStreamPath)
	{
		doParse (m_xServiceFactory, m_xStorage, rStreamPath);
	}
	virtual void SAL_CALL startElement (const ::rtl::OUString& aName,
										const css::uno::Reference< css::xml::sax::XAttributeList > & xAttribs)
		throw( css::xml::sax::SAXException, css::uno::RuntimeException )
	{
		fprintf (stderr, "startElement '%s'\n", rtl::OUStringToOString(aName, RTL_TEXTENCODING_UTF8).getStr());
		if (aName.equalsAscii("c"))
		{
			m_aAddr.Parse(xAttribs->getValueByName(rtl::OUString::createFromAscii("r")));
			m_aType = xAttribs->getValueByName(rtl::OUString::createFromAscii("t"));
			if (!m_aType.getLength()) // nice & slow
				m_aType = rtl::OUString::createFromAscii("n");
		}
	}
	virtual void SAL_CALL endElement(const ::rtl::OUString& aName)
		throw( css::xml::sax::SAXException, css::uno::RuntimeException )
	{
		if (aName.equalsAscii("c"))
            m_pDoc->PutCell( m_aAddr, new ScStringCell( m_aContent ), (BOOL) TRUE );
	}
	virtual void SAL_CALL characters(const ::rtl::OUString& aChars)
		throw( css::xml::sax::SAXException, css::uno::RuntimeException )
	{
		m_aContent = aChars;
	}
};

class ScECMAParseWorkbook : public ScECMAParseBase
{
	css::uno::Reference<css::lang::XMultiServiceFactory> m_xServiceFactory;
	css::uno::Reference<css::embed::XStorage> m_xStorage;
	ScDocument *m_pDoc;

	rtl::OUString m_aBasePath;
	uno::Reference<ScECMAParseRel> m_xRels;
  public:
	ScECMAParseWorkbook(const css::uno::Reference<css::lang::XMultiServiceFactory> &xServiceFactory,
						const css::uno::Reference<css::embed::XStorage> &xStorage,
						ScDocument *pDoc) :
			ScECMAParseBase(),
			m_xServiceFactory(xServiceFactory),
			m_xStorage(xStorage),
			m_pDoc(pDoc),
			m_aBasePath(),
			m_xRels(new ScECMAParseRel())
	{
	}
	virtual ~ScECMAParseWorkbook() {}

	FltError parseWorkbook(const rtl::OUString &rStreamPath)
	{
		FltError nErr;
		nErr = m_xRels->parseRels (m_xServiceFactory, m_xStorage, rStreamPath);
		if (nErr != eERR_OK)
			return nErr;
		m_aBasePath = getBase (rStreamPath) + rtl::OUString::createFromAscii ("/");
		return doParse (m_xServiceFactory, m_xStorage, rStreamPath);
	}

	virtual void SAL_CALL startElement (const ::rtl::OUString& aName,
										const css::uno::Reference< css::xml::sax::XAttributeList > & xAttribs)
		throw( css::xml::sax::SAXException, css::uno::RuntimeException )
	{
		// FIXME: yes - a validation needed: workbook/sheets/sheet
		if (aName.equalsAscii("sheet"))
		{
			// <sheet name="Sheet1" tabId="1" r:id="rId1"/>
			sal_Int32 nTabId = xAttribs->getValueByName(rtl::OUString::createFromAscii("tabId")).toInt32();
			// FIXME: hideous lack of namespace handling / hardcoding ! ...
			rtl::OUString aTarget = m_xRels->getTargetById (
				xAttribs->getValueByName(rtl::OUString::createFromAscii("r:id")));
			rtl::OUString aName = m_xRels->getTargetById (
				xAttribs->getValueByName(rtl::OUString::createFromAscii("name")));
			// FIXME: why have a 'name' here ? ... is that the user-visible name ?
			fprintf (stderr, "Sheet %d '%s'\n", nTabId,
				 rtl::OUStringToOString(m_aBasePath + aTarget, RTL_TEXTENCODING_UTF8).getStr());
			// FIXME: WTF do we have to deal with an 'nTabId' instead of a pointer ?
			m_pDoc->InsertTab (nTabId, aName);
			SCTAB nTab;
//			if (!m_pDoc->GetTable (aName, nTab))
//				fprintf (stderr, "Failed to create sheet\n");
//			else
			{
				uno::Reference<ScECMAParseSheet> xSheet = new
						ScECMAParseSheet(m_xServiceFactory, m_xStorage, m_pDoc, nTabId);
				xSheet->parseSheet (m_aBasePath + aTarget);
			}
		}
	}
};

static void
recursiveDump (uno::Reference < embed::XStorage > xStorage, int nDepth = 0)
{
	uno::Reference< container::XNameAccess > xName ( xStorage, uno::UNO_QUERY );
	if (!xName.is())
		return;
	uno::Sequence<rtl::OUString> aElements = xName->getElementNames();
	for (int i = 0; i < aElements.getLength(); i++)
	{
		bool isDir = xStorage->isStorageElement(aElements[i]);
		for (int j = 0; j < nDepth; j++)
				fprintf (stderr, "\t");
		fprintf (stderr, "Item '%s' (%s)\n",
				 rtl::OUStringToOString (aElements[i], RTL_TEXTENCODING_UTF8).getStr(),
				 isDir ? "storage" : "stream");
		if (isDir)
		{
			uno::Reference < embed::XStorage > xSubStor;
			xSubStor = xStorage->openStorageElement (aElements[i],
													 embed::ElementModes::READ);
			recursiveDump (xSubStor, nDepth+1);
		}
	}
}

FltError ScImportECMA( SfxMedium &rMedium, ScDocument *pDoc )
{
	fprintf (stderr, "ScImportECMA\n");

	uno::Reference < embed::XStorage > xStorage;

    FltError eRet = eERR_OK;

	xStorage = rMedium.GetStorage();
	if ( !xStorage.is() )
	{
		fprintf (stderr, "No storage!\n");
		return eERR_OPEN;
	}

	uno::Reference<lang::XMultiServiceFactory> xServiceFactory =
			::comphelper::getProcessServiceFactory();

	// Common doc bits
	uno::Reference<ScECMAParseContentTypes> xTypes = new ScECMAParseContentTypes();
	xTypes->doParse(xServiceFactory, xStorage,
					rtl::OUString::createFromAscii("[Content_Types].xml"));
	// Dump the contents: FIXME: dump the types from Content_Types.xml etc.
	recursiveDump (xStorage);

	uno::Reference<ScECMAParseRel> xRootRel = new ScECMAParseRel();
	xRootRel->parseRels (xServiceFactory, xStorage,
						 rtl::OUString::createFromAscii(""));
	
	// XL specific foo
	fprintf (stderr, "The doc path is: '%s'\n",
			 rtl::OUStringToOString(xRootRel->getDocument(), RTL_TEXTENCODING_UTF8).getStr());

	uno::Reference<ScECMAParseWorkbook> xWorkbook = new ScECMAParseWorkbook(xServiceFactory, xStorage, pDoc);
	xWorkbook->parseWorkbook (xRootRel->getDocument());

    pDoc->CalcAfterLoad();

    return eRet;
}
