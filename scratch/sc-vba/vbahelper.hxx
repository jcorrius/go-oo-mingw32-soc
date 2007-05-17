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
#ifndef SC_VBA_HELPER_HXX
#define SC_VBA_HELPER_HXX

#include <com/sun/star/drawing/XShape.hpp>
#include <com/sun/star/script/BasicErrorException.hpp>
#include <basic/sberrors.hxx>
#include <cppuhelper/implbase1.hxx>
#include <com/sun/star/frame/XModel.hpp>
#include <docsh.hxx>

namespace css = ::com::sun::star;

namespace org 
{
	namespace openoffice 
	{

		void dispatchRequests (css::uno::Reference< css::frame::XModel>& xModel,rtl::OUString & aUrl) ;
		void dispatchRequests (css::uno::Reference< css::frame::XModel>& xModel,rtl::OUString & aUrl, css::uno::Sequence< css::beans::PropertyValue >& sProps ) ;
		void implnCopy();
		void implnPaste();
		void implnCut();
		void implnPasteSpecial(sal_uInt16 nFlags,sal_uInt16 nFunction,sal_Bool bSkipEmpty, sal_Bool bTranspose);
		bool isRangeShortCut( const ::rtl::OUString& sParam );
		css::uno::Reference< css::frame::XModel >
			getCurrentDocument() throw (css::uno::RuntimeException);
		ScTabViewShell* getBestViewShell( css::uno::Reference< css::frame::XModel>& xModel ) ;
		ScDocShell* getDocShell( css::uno::Reference< css::frame::XModel>& xModel ) ;
		ScTabViewShell* getCurrentBestViewShell();
		SfxViewFrame* getCurrentViewFrame();
		sal_Int32 OORGBToXLRGB( sal_Int32 );
		sal_Int32 XLRGBToOORGB( sal_Int32 );
		css::uno::Any OORGBToXLRGB( const css::uno::Any& );
		css::uno::Any XLRGBToOORGB( const css::uno::Any& );
		// provide a NULL object that can be passed as variant so that 
		// the object when passed to IsNull will return true. aNULL 
		// contains an empty object reference
		const css::uno::Any& aNULL();
		void PrintOutHelper( const css::uno::Any& From, const css::uno::Any& To, const css::uno::Any& Copies, const css::uno::Any& Preview, const css::uno::Any& ActivePrinter, const css::uno::Any& PrintToFile, const css::uno::Any& Collate, const css::uno::Any& PrToFileName, css::uno::Reference< css::frame::XModel >& xModel, sal_Bool bSelection  );

		rtl::OUString getAnyAsString( const css::uno::Any& pvargItem ) throw ( css::uno::RuntimeException );

class Millimeter
{
//Factor to translate between points and hundredths of millimeters:
private:
	static const double factor;
    
	double m_nMillimeter;

public:
	Millimeter():m_nMillimeter(0) {}
    
	Millimeter(double mm):m_nMillimeter(mm) {}
    
	void set(double mm) { m_nMillimeter = mm; }
	void setInPoints(double points) 
	{ 
		m_nMillimeter = points * 0.352777778; 
		// 25.4mm / 72 
	}
      
	void setInHundredthsOfOneMillimeter(double hmm)
	{
		m_nMillimeter = hmm / 100;
	}
    
	double get()
	{
		return m_nMillimeter;
	}
	double getInHundredthsOfOneMillimeter()
	{
		return m_nMillimeter * 100;
	}
	double getInPoints()
	{
		return m_nMillimeter * 2.834645669; // 72 / 25.4mm
	}    

	static int getInHundredthsOfOneMillimeter(double points)
	{
		int mm = static_cast<int>(double(points * factor));
		return mm;
	}
    
	static double getInPoints(int _hmm)
	{
		float points = static_cast<float>(double(_hmm / factor));
		return points;
	}
};
class ShapeHelper
{
protected:
	css::uno::Reference< css::drawing::XShape > xShape;
public:
	ShapeHelper( const css::uno::Reference< css::drawing::XShape >& _xShape) throw (css::script::BasicErrorException ) : xShape( _xShape )
	{ 
		if( !xShape.is() ) 
			throw css::uno::RuntimeException( rtl::OUString::createFromAscii("No valid shape for helper"), css::uno::Reference< css::uno::XInterface >() );
	}
    
	double getHeight()
	{
        	return  Millimeter::getInPoints(xShape->getSize().Height);
    	}


    	void setHeight(double _fheight) throw ( css::script::BasicErrorException )
	{
		try
		{
			css::awt::Size aSize = xShape->getSize();
			aSize.Height = Millimeter::getInHundredthsOfOneMillimeter(_fheight);
			xShape->setSize(aSize);
		}
		catch ( css::uno::Exception& /*e*/)
		{
			throw css::script::BasicErrorException( rtl::OUString(), css::uno::Reference< css::uno::XInterface >(), SbERR_METHOD_FAILED, rtl::OUString() );
    		}
	}
    
    
	double getWidth()
	{ 
		return Millimeter::getInPoints(xShape->getSize().Width);
    	}

	void setWidth(double _fWidth) throw ( css::script::BasicErrorException )
	{
		try
		{
			css::awt::Size aSize = xShape->getSize();
			aSize.Width = Millimeter::getInHundredthsOfOneMillimeter(_fWidth);
			xShape->setSize(aSize);
		}
		catch (css::uno::Exception& /*e*/)
		{
			throw css::script::BasicErrorException( rtl::OUString(), css::uno::Reference< css::uno::XInterface >(), SbERR_METHOD_FAILED, rtl::OUString() );
		}
	}
    
    
	double getLeft()
	{
		return Millimeter::getInPoints(xShape->getPosition().X);
	}

    
	void setLeft(double _fLeft)
	{
		css::awt::Point aPoint = xShape->getPosition();
		aPoint.X = Millimeter::getInHundredthsOfOneMillimeter(_fLeft);
		xShape->setPosition(aPoint);
	}
    

	double getTop()
	{
        	return Millimeter::getInPoints(xShape->getPosition().Y);
	}

    
	void setTop(double _fTop)
	{
		css::awt::Point aPoint = xShape->getPosition();
		aPoint.Y = Millimeter::getInHundredthsOfOneMillimeter(_fTop);
		xShape->setPosition(aPoint);
	}
    
};

class ContainerUtilities
{

public: 
	static rtl::OUString getUniqueName( const css::uno::Sequence< ::rtl::OUString >&  _slist, const rtl::OUString& _sElementName, const ::rtl::OUString& _sSuffixSeparator);
	static rtl::OUString getUniqueName( const css::uno::Sequence< rtl::OUString >& _slist, const rtl::OUString _sElementName, const rtl::OUString& _sSuffixSeparator, sal_Int32 _nStartSuffix );

	static sal_Int32 FieldInList( const css::uno::Sequence< rtl::OUString >& SearchList, const rtl::OUString& SearchString );
};

// really just a a place holder to ease the porting pain
class DebugHelper
{
public:
	static void exception( const rtl::OUString&  DetailedMessage, const css::uno::Exception& ex,  int err, const rtl::OUString& /*additionalArgument*/ ) throw( css::script::BasicErrorException )
	{
		// #TODO #FIXME ( do we want to support additionalArg here )
		throw css::script::BasicErrorException( DetailedMessage.concat( rtl::OUString::createFromAscii(" ") ).concat( ex.Message ), css::uno::Reference< css::uno::XInterface >(), err, rtl::OUString() );
	}

	static void exception( int err,  const rtl::OUString& additionalArgument ) throw( css::script::BasicErrorException )
	{
		exception( rtl::OUString(), css::uno::Exception(), err, additionalArgument );
	}

	static void exception( css::uno::Exception& ex ) throw( css::script::BasicErrorException )
	{
		exception( rtl::OUString(), ex, SbERR_INTERNAL_ERROR, rtl::OUString() );
	}
};
	} // openoffice
} // org

namespace oo = org::openoffice;

namespace oo = org::openoffice;

#ifdef DEBUG
#  define SC_VBA_FIXME(a) OSL_TRACE( a )
#  define SC_VBA_STUB() SC_VBA_FIXME(( "%s - stubbed\n", __FUNCTION__ ))
#else
#  define SC_VBA_FIXME(a)
#  define SC_VBA_STUB()
#endif

#endif
