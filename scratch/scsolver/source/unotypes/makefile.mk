#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.1 $
#
#   last change: $Author: michael $ $Date: 2006/01/09 12:13:38 $
#
#   The Contents of this file are made available subject to
#   the terms of GNU Lesser General Public License Version 2.1.
#
#
#     GNU Lesser General Public License Version 2.1
#     =============================================
#     Copyright 2005 by Sun Microsystems, Inc.
#     901 San Antonio Road, Palo Alto, CA 94303, USA
#
#     This library is free software; you can redistribute it and/or
#     modify it under the terms of the GNU Lesser General Public
#     License version 2.1, as published by the Free Software Foundation.
#
#     This library is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#     Lesser General Public License for more details.
#
#     You should have received a copy of the GNU Lesser General Public
#     License along with this library; if not, write to the Free Software
#     Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#     MA  02111-1307  USA
#
#*************************************************************************
PRJ=..$/..

PRJNAME=	vcl
TARGET=		unotypes


# --- Settings -----------------------------------------------------

.INCLUDE :  svpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  sv.mk
.INCLUDE :  $(PRJ)$/util$/makefile2.pmk

# ------------------------------------------------------------------

CPPUMAKERFLAGS =

UNOUCROUT=$(OUT)$/inc

UNOTYPES= \
	com.sun.star.io.XStreamListener				\
	com.sun.star.awt.MouseEvent					\
	com.sun.star.awt.XWindowPeer 				\
	com.sun.star.awt.XGraphics 					\
	com.sun.star.awt.XToolkit 					\
	com.sun.star.awt.XExtendedToolkit			\
	com.sun.star.awt.XDisplayConnection 		\
	com.sun.star.awt.XTopWindow         		\
	com.sun.star.datatransfer.dnd.DNDConstants \
	com.sun.star.datatransfer.dnd.XDragGestureRecognizer \
	com.sun.star.datatransfer.dnd.XDragSource   \
	com.sun.star.datatransfer.dnd.XDropTarget   \
	com.sun.star.lang.XMultiServiceFactory 	    \
	com.sun.star.lang.XSingleServiceFactory 	\
	com.sun.star.lang.XTypeProvider				\
	com.sun.star.registry.XRegistryKey 			\
	com.sun.star.registry.XImplementationRegistration 			\
	com.sun.star.i18n.XBreakIterator			\
	com.sun.star.i18n.CharacterIteratorMode		\
	com.sun.star.i18n.WordType          		\
	com.sun.star.i18n.XCollator          		\
	com.sun.star.i18n.XLocaleData          		\
	com.sun.star.i18n.LocaleItem          		\
	com.sun.star.i18n.reservedWords        		\
	com.sun.star.i18n.XCharacterClassification	\
	com.sun.star.i18n.KCharacterType	\
	com.sun.star.i18n.TransliterationModules	\
	com.sun.star.i18n.XTransliteration      	\
	com.sun.star.uno.TypeClass					\
	com.sun.star.uno.RuntimeException			\
	com.sun.star.uno.XWeak						\
	com.sun.star.uno.XAggregation				\
	com.sun.star.uno.XNamingService				\
	com.sun.star.ucb.CommandAbortedException    \
	com.sun.star.datatransfer.clipboard.XFlushableClipboard \
	com.sun.star.accessibility.XAccessible \
	com.sun.star.accessibility.AccessibleRole \
	com.sun.star.accessibility.AccessibleEventObject \
	com.sun.star.bridge.XBridgeFactory				\
	com.sun.star.connection.XAcceptor				\
	com.sun.star.connection.XConnector				\
	com.sun.star.connection.XConnectionBroadcaster				\
	com.sun.star.util.Color \
	com.sun.star.util.XAtomServer					\
	com.sun.star.frame.XSessionManagerClient		\
	com.sun.star.frame.XSessionManagerListener

# --- Targets ------------------------------------------------------

.INCLUDE :	target.mk

