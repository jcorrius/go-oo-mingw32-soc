/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: MyProtocolHandler.cxx,v $
 * $Revision: 1.5 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/


#include "ListenerHelper.h"
#include "MyProtocolHandler.h"
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XController.hpp>
#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/frame/DispatchDescriptor.hpp>
#include <com/sun/star/text/XTextViewCursorSupplier.hpp>
#include <com/sun/star/text/XTextViewCursor.hpp>
#include <com/sun/star/text/XTextCursor.hpp>
#include <com/sun/star/text/XTextDocument.hpp>
#include <com/sun/star/sheet/XSpreadsheetView.hpp>
#include <com/sun/star/frame/ControlCommand.hpp>
#include <com/sun/star/frame/ControlEvent.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/frame/XControlNotificationListener.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/awt/XWindowPeer.hpp>
#include <com/sun/star/awt/WindowAttribute.hpp>
#include <com/sun/star/awt/XMessageBox.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/view/XSelectionSupplier.hpp>
#include <com/sun/star/system/XSystemShellExecute.hpp>
#include <com/sun/star/system/SystemShellExecuteFlags.hpp>
#include <com/sun/star/frame/XStorable.hpp>
#include <com/sun/star/container/XContainerQuery.hpp>
#include <com/sun/star/ui/dialogs/XFilePicker.hpp>
#include <com/sun/star/ui/dialogs/XFilterManager.hpp>
#include <com/sun/star/ui/dialogs/TemplateDescription.hpp>
#include <com/sun/star/ui/dialogs/ExecutableDialogResults.hpp>
 #include <com/sun/star/ui/dialogs/ExecutableDialogResults.hpp>

#include <osl/process.h>
#include <osl/file.hxx>
#include <rtl/ustring.hxx>
#include <rtl/ustrbuf.hxx>
#include <rtl/strbuf.hxx>
#include <rtl/byteseq.hxx>

using namespace com::sun::star::uno;
using namespace com::sun::star::frame;
using com::sun::star::lang::XMultiServiceFactory;
using com::sun::star::beans::PropertyValue;
using com::sun::star::util::URL;
using com::sun::star::text::XTextViewCursorSupplier;
using com::sun::star::text::XTextViewCursor;
using com::sun::star::text::XTextCursor;
using com::sun::star::sheet::XSpreadsheetView;
using com::sun::star::frame::XModel;
using com::sun::star::text::XTextRange;
using com::sun::star::text::XTextDocument;
using com::sun::star::beans::NamedValue;
using namespace com::sun::star::awt;
using com::sun::star::view::XSelectionSupplier;
using namespace com::sun::star::system;
using namespace com::sun::star::ui::dialogs;

ListenerHelper aListenerHelper;



/*void ShowMessageBox( const Reference< XToolkit >& rToolkit, 
               const Reference< XFrame >& rFrame, const ::rtl::OUString& aTitle, const ::rtl::OUString &aMsgText )
{
    if ( rFrame.is() && rToolkit.is() )
    {
        // describe window properties.
        WindowDescriptor                aDescriptor;
        aDescriptor.Type              = WindowClass_MODALTOP;
        aDescriptor.WindowServiceName = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "infobox" ));
        aDescriptor.ParentIndex       = -1;
        aDescriptor.Parent            = Reference< XWindowPeer >
						( rFrame->getContainerWindow(), UNO_QUERY );
        aDescriptor.Bounds            = Rectangle(300,200,300,200);
        aDescriptor.WindowAttributes  = WindowAttribute::BORDER | 
                                          WindowAttribute::MOVEABLE | WindowAttribute::CLOSEABLE;
 
        Reference< XWindowPeer > xPeer = rToolkit->createWindow( aDescriptor );
        if ( xPeer.is() )
        {
            Reference< XMessageBox > xMsgBox( xPeer, UNO_QUERY );
            if ( xMsgBox.is() )
            {
                xMsgBox->setCaptionText( aTitle );
                xMsgBox->setMessageText( aMsgText );
                xMsgBox->execute();
            }
        }
    }
}
*/


oslFileError readLine( oslFileHandle pFile, ::rtl::OStringBuffer& line )
{
    OSL_PRECOND( line.getLength() == 0, "line buf not empty" );

    // TODO(P3): read larger chunks
    sal_Char aChar('\n');
    sal_uInt64 nBytesRead;
    oslFileError nRes;

    // skip garbage \r \n at start of line
    while( osl_File_E_None == (nRes=osl_readFile(pFile, &aChar, 1, &nBytesRead)) && 
           nBytesRead == 1 && 
           (aChar == '\n' || aChar == '\r') ) ;

    if( aChar != '\n' && aChar != '\r' )
        line.append( aChar );

    while( osl_File_E_None == (nRes=osl_readFile(pFile, &aChar, 1, &nBytesRead)) && 
           nBytesRead == 1 && aChar != '\n' && aChar != '\r' )
    {
        line.append( aChar );
    }
    
    return nRes;
}

void BaseDispatch::ShowMessageBox( const Reference< XFrame >& rFrame, const ::rtl::OUString& aTitle, const ::rtl::OUString& aMsgText )
{
	if ( !mxToolkit.is() )
		mxToolkit = Reference< XToolkit > ( mxMSF->createInstance(
			::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(
                                            "com.sun.star.awt.Toolkit" ))), UNO_QUERY );
    if ( rFrame.is() && mxToolkit.is() )
    {
        // describe window properties.
        WindowDescriptor                aDescriptor;
        aDescriptor.Type              = WindowClass_MODALTOP;
        aDescriptor.WindowServiceName = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "infobox" ));
        aDescriptor.ParentIndex       = -1;
        aDescriptor.Parent            = Reference< XWindowPeer >( rFrame->getContainerWindow(), UNO_QUERY );
        aDescriptor.Bounds            = Rectangle(0,0,300,200);
        aDescriptor.WindowAttributes  = WindowAttribute::BORDER | WindowAttribute::MOVEABLE | WindowAttribute::CLOSEABLE;

        Reference< XWindowPeer > xPeer = mxToolkit->createWindow( aDescriptor );
        if ( xPeer.is() )
        {
            Reference< XMessageBox > xMsgBox( xPeer, UNO_QUERY );
            if ( xMsgBox.is() )
            {
                xMsgBox->setCaptionText( aTitle );
                xMsgBox->setMessageText( aMsgText );
                xMsgBox->execute();
            }
        }
    }
}

void BaseDispatch::SendCommand( const com::sun::star::util::URL& aURL, const ::rtl::OUString& rCommand, const Sequence< NamedValue >& rArgs, sal_Bool bEnabled )
{
    Reference < XDispatch > xDispatch = 
            aListenerHelper.GetDispatch( mxFrame, aURL.Path );
    
    FeatureStateEvent aEvent;
    
    aEvent.FeatureURL = aURL;
    aEvent.Source     = xDispatch;
    aEvent.IsEnabled  = bEnabled;
    aEvent.Requery    = sal_False;

    ControlCommand aCtrlCmd;
    aCtrlCmd.Command   = rCommand;
    aCtrlCmd.Arguments = rArgs;
    
    aEvent.State <<= aCtrlCmd;
    aListenerHelper.Notify( mxFrame, aEvent.FeatureURL.Path, aEvent ); 
}

void BaseDispatch::SendCommandTo( const Reference< XStatusListener >& xControl, const URL& aURL, const ::rtl::OUString& rCommand, const Sequence< NamedValue >& rArgs, sal_Bool bEnabled )
{
    FeatureStateEvent aEvent;
    
    aEvent.FeatureURL = aURL;
    aEvent.Source     = (::com::sun::star::frame::XDispatch*) this;
    aEvent.IsEnabled  = bEnabled;
    aEvent.Requery    = sal_False;

    ControlCommand aCtrlCmd;
    aCtrlCmd.Command   = rCommand;
    aCtrlCmd.Arguments = rArgs;
    
    aEvent.State <<= aCtrlCmd;
    xControl->statusChanged( aEvent );
}

void SAL_CALL MyProtocolHandler::initialize( const Sequence< Any >& aArguments ) throw ( Exception, RuntimeException)
{
	Reference < XFrame > xFrame;
	if ( aArguments.getLength() )
	{
		// das erste Argument ist immer der Frame, da ein ProtocolHandler den braucht um Zugriff
		// auf den Context zu haben, in dem er aufgerufen wird
		aArguments[0] >>= xFrame;
		mxFrame = xFrame;
	}
}

Reference< XDispatch > SAL_CALL MyProtocolHandler::queryDispatch(	const URL& aURL, const ::rtl::OUString& sTargetFrameName, sal_Int32 nSearchFlags )
				throw( RuntimeException )
{
	Reference < XDispatch > xRet;
	if ( !mxFrame.is() )
		return 0;

	Reference < XController > xCtrl = mxFrame->getController();
	if ( xCtrl.is() && !aURL.Protocol.compareToAscii("vnd.demo.complextoolbarcontrols.demoaddon:") )
	{
		Reference < XTextViewCursorSupplier > xCursor( xCtrl, UNO_QUERY );
		Reference < XSpreadsheetView > xView( xCtrl, UNO_QUERY );
		if ( !xCursor.is() && !xView.is() )
			// ohne ein entsprechendes Dokument funktioniert der Handler nicht
			return xRet;

		if ( aURL.Path.equalsAscii("Command1" ) ||
			 aURL.Path.equalsAscii("Command2" ) ||
			 aURL.Path.equalsAscii("Command3" ) ||
			 aURL.Path.equalsAscii("Command4" ) ||
			 aURL.Path.equalsAscii("Command5" ) ||
			 aURL.Path.equalsAscii("Command6" ) ||
             aURL.Path.equalsAscii("Command7" ) )
		{
			xRet = aListenerHelper.GetDispatch( mxFrame, aURL.Path );
			if ( !xRet.is() )
			{
				xRet = xCursor.is() ? (BaseDispatch*) new WriterDispatch( mxMSF, mxFrame ) :
					(BaseDispatch*) new CalcDispatch( mxMSF, mxFrame );
				aListenerHelper.AddDispatch( xRet, mxFrame, aURL.Path );
			}
		}
	}

	return xRet;
}

Sequence < Reference< XDispatch > > SAL_CALL MyProtocolHandler::queryDispatches( const Sequence < DispatchDescriptor >& seqDescripts )
			throw( RuntimeException )
{
    sal_Int32 nCount = seqDescripts.getLength();
    Sequence < Reference < XDispatch > > lDispatcher( nCount );

    for( sal_Int32 i=0; i<nCount; ++i )
        lDispatcher[i] = queryDispatch( seqDescripts[i].FeatureURL, seqDescripts[i].FrameName, seqDescripts[i].SearchFlags );

    return lDispatcher;
}

::rtl::OUString MyProtocolHandler_getImplementationName ()
	throw (RuntimeException)
{
    return ::rtl::OUString::createFromAscii(MYPROTOCOLHANDLER_IMPLEMENTATIONNAME);
}

sal_Bool SAL_CALL MyProtocolHandler_supportsService( const ::rtl::OUString& ServiceName )
	throw (RuntimeException)
{
    return (
            ServiceName.equalsAscii(MYPROTOCOLHANDLER_SERVICENAME  ) ||
            ServiceName.equalsAscii("com.sun.star.frame.ProtocolHandler")
           );
}

Sequence< ::rtl::OUString > SAL_CALL MyProtocolHandler_getSupportedServiceNames(  )
	throw (RuntimeException)
{
	Sequence < ::rtl::OUString > aRet(1);
    aRet[0] = ::rtl::OUString::createFromAscii(MYPROTOCOLHANDLER_SERVICENAME);
    return aRet;
}

#undef SERVICE_NAME

Reference< XInterface > SAL_CALL MyProtocolHandler_createInstance( const Reference< XMultiServiceFactory > & rSMgr)
	throw( Exception )
{
	return (cppu::OWeakObject*) new MyProtocolHandler( rSMgr );
}

// XServiceInfo
::rtl::OUString SAL_CALL MyProtocolHandler::getImplementationName(  )
	throw (RuntimeException)
{
	return MyProtocolHandler_getImplementationName();
}

sal_Bool SAL_CALL MyProtocolHandler::supportsService( const ::rtl::OUString& rServiceName )
	throw (RuntimeException)
{
    return MyProtocolHandler_supportsService( rServiceName );
}

Sequence< ::rtl::OUString > SAL_CALL MyProtocolHandler::getSupportedServiceNames(  )
	throw (RuntimeException)
{
    return MyProtocolHandler_getSupportedServiceNames();
}

void SAL_CALL BaseDispatch::dispatch( const URL& aURL, const Sequence < PropertyValue >& lArgs ) throw (RuntimeException)
{
	/* Its neccessary to hold this object alive, till this method finish.
	   May the outside dispatch cache (implemented by the menu/toolbar!)
	   forget this instance during de-/activation of frames (focus!).

		E.g. An open db beamer in combination with the My-Dialog
		can force such strange situation :-(
	 */
	Reference< XInterface > xSelfHold(static_cast< XDispatch* >(this), UNO_QUERY);

	if ( !aURL.Protocol.compareToAscii("vnd.demo.complextoolbarcontrols.demoaddon:") )
	{
		if ( !aURL.Path.compareToAscii("Command1" ) )
		{
		
		        
		        
		        
		         Reference< XFilePicker > rFilePicker = Reference< XFilePicker >

( mxMSF->createInstance(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.ui.dialogs.FilePicker" ))), UNO_QUERY );


Reference< XFilterManager > rFilterManager (rFilePicker, UNO_QUERY);

Reference<com::sun::star::lang::XInitialization > rInitialize (rFilePicker, UNO_QUERY);

Sequence < Any > info(1);


// Don't forget to add : #include <com/sun/star/ui/dialogs/TemplateDescription.hpp>

// Don't forget to add "com.sun.star.ui.dialogs.TemplateDescription \" in the makefile

info[0] <<= (short) TemplateDescription::FILEOPEN_SIMPLE;

rInitialize-> initialize(info);


rFilterManager->appendFilter(::rtl::OUString::createFromAscii("Texts"),

::rtl::OUString::createFromAscii("*.txt"));

rFilterManager->appendFilter(::rtl::OUString::createFromAscii("Docs OpenOffice"),

::rtl::OUString::createFromAscii("*.sxw;*.sxc"));

rFilterManager->setCurrentFilter(::rtl::OUString::createFromAscii("Docs OpenOffice"));

short result=rFilePicker->execute();


// Don't forget to add : #include <com/sun/star/ui/dialogs/ExecutableDialogResults.hpp>

// Don't forget to add "com.sun.star.ui.dialogs/ExecutableDialogResults \" in the makefile

Reference< XToolkit >rToolkit = Reference< XToolkit >(mxMSF->createInstance(

::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM(

"com.sun.star.awt.Toolkit" ))), UNO_QUERY );


            Reference< XInterface > Desktop = mxMSF->createInstance(::rtl::OUString::createFromAscii( "com.sun.star.frame.Desktop" ));

Reference<com::sun::star::frame::XDesktop > rDesktop(Desktop,UNO_QUERY);
Reference< XFrame > rFrame=rDesktop->getCurrentFrame();


if (result == ExecutableDialogResults::OK)

ShowMessageBox( rFrame,::rtl::OUString::createFromAscii("Result") ,rFilePicker->getFiles()[0] );



            

            Reference< XComponentLoader > rComponentLoader (Desktop, UNO_QUERY);
            
            
      /****
      
      SOME TEST ON URLS
        ::rtl::OUString sAbsoluteDocUrl, sWorkingDir;
        
             
            osl_getProcessWorkingDir(&sWorkingDir.pData);
            
            ShowMessageBox( rFrame,::rtl::OUString::createFromAscii("Result") ,sWorkingDir);

          
        
          osl::FileBase::getAbsoluteFileURL( ::rtl::OUString::createFromAscii("file:///Users/jon/"), ::rtl::OUString::createFromAscii("index.html"), sAbsoluteDocUrl);
         */   
            
                Reference<com::sun::star::lang::XComponent > xHTMLComponent = rComponentLoader->loadComponentFromURL(
       ::rtl::OUString::createFromAscii("file:///Users/jon/index.html"), ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("_blank") ), 0,
        Sequence < ::com::sun::star::beans::PropertyValue >());
        
        
        

            Reference <com::sun::star::lang::XComponent > xWriterComponent = rComponentLoader->loadComponentFromURL(
               ::rtl::OUString::createFromAscii("private:factory/swriter"),
               ::rtl::OUString::createFromAscii("_blank"),
               0,
               Sequence < ::com::sun::star::beans::PropertyValue >());
            
            Reference < XTextDocument > xTextDocument (xWriterComponent,UNO_QUERY);
            Reference<com::sun::star::text::XText> xText = xTextDocument->getText();
            xText->setString(::rtl::OUString::createFromAscii("Hello"));
            
            
 
            rtl::OUString FileURL;
            osl::FileBase::getFileURLFromSystemPath(
              rtl::OUString::createFromAscii("/bin/cat"),
              FileURL
            );
            
            rtl::OUString AppArgs = rtl::OUString::createFromAscii("/Users/jon/out.txt");
            
          
          
            
          
             oslProcess    aProcess;
    oslFileHandle pIn  = NULL;
    oslFileHandle pOut = NULL;
    oslFileHandle pErr = NULL;
        rtl_uString** ppEnv = NULL;
    sal_uInt32 nEnv = 0;
    
    const oslProcessError eErr = 
        osl_executeProcess_WithRedirectedIO(FileURL.pData, 
                                            &AppArgs.pData, 
                                            1, 
                                            osl_Process_SEARCHPATH|osl_Process_HIDDEN, 
                                            osl_getCurrentSecurity(), 
                                            0, ppEnv, nEnv, 
                                            &aProcess, &pIn, &pOut, &pErr);
                                            
                                            



        
        ::rtl::OStringBuffer line;
        ::rtl::OString content;
        ::rtl::OString lineBreak = rtl::OUStringToOString(rtl::OUString::createFromAscii(" "), RTL_TEXTENCODING_ASCII_US );
        
        bool bRet=true;
         while( osl_File_E_None == readLine(pOut, line) && line.getLength() ) {
            if(bRet) {
             content = line.makeStringAndClear();
             bRet = false;
            } else {
             content = content.concat(lineBreak);
             content = content.concat(line.makeStringAndClear());
            }
         }
         xText->setString(rtl::OStringToOUString(content, RTL_TEXTENCODING_ASCII_US));
            
        }

	}
}

void SAL_CALL BaseDispatch::addStatusListener( const Reference< XStatusListener >& xControl, const URL& aURL ) throw (RuntimeException)
{
	if ( aURL.Protocol.equalsAscii("vnd.demo.complextoolbarcontrols.demoaddon:") )
	{
		if ( aURL.Path.equalsAscii("Command1" ) )
        {
			// just enable this command
		    ::com::sun::star::frame::FeatureStateEvent aEvent;
			aEvent.FeatureURL = aURL;
		    aEvent.Source = (::com::sun::star::frame::XDispatch*) this;
			aEvent.IsEnabled = mbButtonEnabled;
			aEvent.Requery = sal_False;
			aEvent.State <<= Any();
			xControl->statusChanged( aEvent );
        }
        else if ( aURL.Path.equalsAscii("Command2" ) )
		{
			// just enable this command
		    ::com::sun::star::frame::FeatureStateEvent aEvent;
			aEvent.FeatureURL = aURL;
		    aEvent.Source = (::com::sun::star::frame::XDispatch*) this;
			aEvent.IsEnabled = sal_True;
			aEvent.Requery = sal_False;
			aEvent.State <<= Any();
			xControl->statusChanged( aEvent );
		}
		else if ( aURL.Path.equalsAscii("Command3" ) )
		{
            // A toggle dropdown box is normally used for a group of commands
            // where the user can select the last issued command easily.
            // E.g. a typical command group would be "Insert shape"
            Sequence< NamedValue > aArgs( 1 );
            
            // send command to set context menu content
            Sequence< rtl::OUString > aContextMenu( 3 );
            aContextMenu[0] = rtl::OUString::createFromAscii( "Command 1" );
            aContextMenu[1] = rtl::OUString::createFromAscii( "Command 2" );
            aContextMenu[2] = rtl::OUString::createFromAscii( "Command 3" );
            
            aArgs[0].Name = rtl::OUString::createFromAscii( "List" );
            aArgs[0].Value <<= aContextMenu;
            SendCommandTo( xControl, aURL, rtl::OUString::createFromAscii( "SetList" ), aArgs, sal_True );
            
            // send command to check item on pos=0
            aArgs[0].Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Pos" ));
            aArgs[0].Value <<= sal_Int32( 0 );            
            SendCommandTo( xControl, aURL, ::rtl::OUString::createFromAscii( "CheckItemPos" ), aArgs, sal_True );
        }
        else if ( aURL.Path.equalsAscii("Command4" ) )
        {
            // A dropdown box is normally used for a group of dependent modes, where
            // the user can only select one. The modes cannot be combined.
            // E.g. a typical group would be left,right,center,block.
            Sequence< NamedValue > aArgs( 1 );
            
            // send command to set context menu content
            Sequence< rtl::OUString > aContextMenu( 2 );
            aContextMenu[0] = rtl::OUString::createFromAscii( "Button Enabled" );
            aContextMenu[1] = rtl::OUString::createFromAscii( "Button Disabled" );
            
            aArgs[0].Name = rtl::OUString::createFromAscii( "List" );
            aArgs[0].Value <<= aContextMenu;
            SendCommandTo( xControl, aURL, rtl::OUString::createFromAscii( "SetList" ), aArgs, sal_True );            

            // set position according to enable/disable state of button
            sal_Int32 nPos( mbButtonEnabled ? 0 : 1 );
            
            // send command to check item on pos=0
            aArgs[0].Name = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Pos" ));
            aArgs[0].Value <<= nPos;
            SendCommandTo( xControl, aURL, ::rtl::OUString::createFromAscii( "CheckItemPos" ), aArgs, sal_True );
        }
        else if ( aURL.Path.equalsAscii("Command5" ) )
        {
            // A spin button
            Sequence< NamedValue > aArgs( 5 );
            
            // send command to initialize spin button
            aArgs[0].Name = rtl::OUString::createFromAscii( "Value" );
            aArgs[0].Value <<= double( 0.0 );
            aArgs[1].Name = rtl::OUString::createFromAscii( "UpperLimit" );
            aArgs[1].Value <<= double( 10.0 );
            aArgs[2].Name = rtl::OUString::createFromAscii( "LowerLimit" );
            aArgs[2].Value <<= double( 0.0 );
            aArgs[3].Name = rtl::OUString::createFromAscii( "Step" );
            aArgs[3].Value <<= double( 0.1 );
            aArgs[4].Name = rtl::OUString::createFromAscii( "OutputFormat" );
            aArgs[4].Value <<= rtl::OUString::createFromAscii( "%.2f cm" );

            SendCommandTo( xControl, aURL, rtl::OUString::createFromAscii( "SetValues" ), aArgs, sal_True );
        }
        else if ( aURL.Path.equalsAscii("Command7" ) )
        {
            // A dropdown box is normally used for a group of commands
            // where the user can select one of a defined set.
            Sequence< NamedValue > aArgs( 1 );
            
            // send command to set context menu content
            Sequence< rtl::OUString > aList( 10 );
            aList[0] = rtl::OUString::createFromAscii( "White" );
            aList[1] = rtl::OUString::createFromAscii( "Black" );
            aList[2] = rtl::OUString::createFromAscii( "Red" );
            aList[3] = rtl::OUString::createFromAscii( "Blue" );
            aList[4] = rtl::OUString::createFromAscii( "Green" );
            aList[5] = rtl::OUString::createFromAscii( "Grey" );
            aList[6] = rtl::OUString::createFromAscii( "Yellow" );
            aList[7] = rtl::OUString::createFromAscii( "Orange" );
            aList[8] = rtl::OUString::createFromAscii( "Brown" );
            aList[9] = rtl::OUString::createFromAscii( "Pink" );
            
            aArgs[0].Name = rtl::OUString::createFromAscii( "List" );
            aArgs[0].Value <<= aList;
            SendCommandTo( xControl, aURL, rtl::OUString::createFromAscii( "SetList" ), aArgs, sal_True );
        }

		aListenerHelper.AddListener( mxFrame, xControl, aURL.Path );
	}
}

void SAL_CALL BaseDispatch::removeStatusListener( const Reference< XStatusListener >& xControl, const URL& aURL ) throw (RuntimeException)
{
	aListenerHelper.RemoveListener( mxFrame, xControl, aURL.Path );
}

void SAL_CALL BaseDispatch::controlEvent( const ControlEvent& Event ) throw (RuntimeException)
{
    if ( Event.aURL.Protocol.equalsAscii("vnd.demo.complextoolbarcontrols.demoaddon:" ))
	{
        if ( Event.aURL.Path.equalsAscii( "Command2" ))
        {
            // We get notifications whenever the text inside the combobox has been changed.
            // We store the new text into a member.
            if ( Event.Event.equalsAscii( "TextChanged" ))
            {
                rtl::OUString aNewText;
                sal_Bool      bHasText( sal_False );
                for ( sal_Int32 i = 0; i < Event.aInformation.getLength(); i++ )
                {
                    if ( Event.aInformation[i].Name.equalsAsciiL( "Text", 4 ))
                    {
                        bHasText = Event.aInformation[i].Value >>= aNewText;
                        break;
                    }
                }
                
                if ( bHasText )
                    maComboBoxText = aNewText;
            }
        }
    }
}
 
BaseDispatch::BaseDispatch( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > &rxMSF,
        const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& xFrame, const ::rtl::OUString& rServiceName )
        : mxMSF( rxMSF )
		, mxFrame( xFrame )
        , msDocService( rServiceName )
        , mbButtonEnabled( sal_True )
{
}


BaseDispatch::~BaseDispatch()
{
	mxFrame.clear();
	mxMSF.clear();
}
