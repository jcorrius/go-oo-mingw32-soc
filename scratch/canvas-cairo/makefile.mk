#*************************************************************************
#
#   $RCSfile: makefile.mk,v $
#
#   $Revision: 1.8 $
#
#   last change: $Author: rodo $ $Date: 2005/08/10 15:12:03 $
#
#   The Contents of this file are made available subject to the terms of
#   either of the following licenses
#
#          - GNU Lesser General Public License Version 2.1
#          - Sun Industry Standards Source License Version 1.1
#
#   Sun Microsystems Inc., October, 2000
#
#   GNU Lesser General Public License Version 2.1
#   =============================================
#   Copyright 2000 by Sun Microsystems, Inc.
#   901 San Antonio Road, Palo Alto, CA 94303, USA
#
#   This library is free software; you can redistribute it and/or
#   modify it under the terms of the GNU Lesser General Public
#   License version 2.1, as published by the Free Software Foundation.
#
#   This library is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public
#   License along with this library; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#   MA  02111-1307  USA
#
#
#   Sun Industry Standards Source License Version 1.1
#   =================================================
#   The contents of this file are subject to the Sun Industry Standards
#   Source License Version 1.1 (the "License"); You may not use this file
#   except in compliance with the License. You may obtain a copy of the
#   License at http://www.openoffice.org/license.html.
#
#   Software provided under this License is provided on an "AS IS" basis,
#   WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
#   WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
#   MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
#   See the License for the specific provisions governing your rights and
#   obligations concerning the Software.
#
#   The Initial Developer of the Original Code is: Sun Microsystems, Inc.
#
#   Copyright: 2000 by Sun Microsystems, Inc.
#
#   All Rights Reserved.
#
#   Contributor(s): _______________________________________
#
#
#
#*************************************************************************

PRJ=..$/..

PRJNAME=canvas
TARGET=cairocanvas
ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------------

.INCLUDE :	settings.mk
DLLPRE =

# --- Nothing to do if we're compiling with --disable-cairo -----------
.IF "$(ENABLE_CAIRO)" != "TRUE"
@all:
	@echo "Building without cairo support..."
.ENDIF

# --- Common ----------------------------------------------------------

.IF "$(GUI)"=="UNX"

.IF "$(SYSTEM_CAIRO)" == "YES"
CFLAGS+=$(CAIRO_CFLAGS)
.ELSE
CFLAGS+=-I$(SOLARINCDIR)/cairo
.ENDIF

.IF "$(verbose)"!="" || "$(VERBOSE)"!=""
CDEFS+= -DVERBOSE
.ENDIF

# Disable optimization for SunCC SPARC
.IF "$(OS)$(CPU)"=="SOLARISS" && "$(COM)"!="GCC"
NOOPTFILES = $(SLO)$/cairo_canvashelper_texturefill.obj 
.ENDIF

SLOFILES =		$(SLO)$/cairo_spritecanvas.obj \
			$(SLO)$/cairo_linepolypolygon.obj \
			$(SLO)$/cairo_backbuffer.obj \
			$(SLO)$/cairo_bitmapbackbuffer.obj \
			$(SLO)$/cairo_canvasfont.obj \
			$(SLO)$/cairo_windowgraphicdevice.obj \
			$(SLO)$/cairo_canvasbitmap.obj \
			$(SLO)$/cairo_canvasbitmaphelper.obj \
			$(SLO)$/cairo_canvashelper.obj \
			$(SLO)$/cairo_canvashelper_texturefill.obj \
			$(SLO)$/cairo_textlayout.obj \
			$(SLO)$/cairo_canvascustomsprite.obj \
			$(SLO)$/cairo_cachedbitmap.obj \
			$(SLO)$/cairo_redrawmanager.obj \
			$(SLO)$/cairo_impltools.obj \
			$(SLO)$/cairo_parametricpolypolygon.obj \
			$(SLO)$/cairo_helper.obj


SHL1TARGET=$(TARGET).uno

SHL1STDLIBS= $(TOOLSLIB) $(CPPULIB) $(SALLIB) $(VCLLIB) $(COMPHELPERLIB) $(CPPUHELPERLIB) $(BASEGFXLIB) $(CANVASTOOLSLIB) $(GOODIESLIB) -L$(SOLARLIBDIR)

.IF "$(SYSTEM_CAIRO)" == "YES"
SHL1STDLIBS+= $(CAIRO_LIBS)
.ELSE
SHL1STDLIBS+= -lcairo -lpixman -lX11
.ENDIF

SHL1IMPLIB=i$(TARGET)
SHL1LIBS=$(SLB)$/$(TARGET).lib
SHL1DEF=$(MISC)$/$(SHL1TARGET).def

.IF "$(OS)"=="MACOSX"
.ELSE
SHL1VERSIONMAP=exports.map
.ENDIF 

DEF1NAME=$(SHL1TARGET)
DEF1EXPORTFILE=exports.dxp

.ENDIF

# ==========================================================================

.INCLUDE :	target.mk
