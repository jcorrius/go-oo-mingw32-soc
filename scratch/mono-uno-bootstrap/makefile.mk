#************************************************************************
#
#  $RCSfile: $
#
#  $Revision: $
#
#  last change: $Author: $ $Date: $
#
#  The Contents of this file are made available subject to the terms of
#  either of the following licenses
#
#         - GNU Lesser General Public License Version 2.1
#         - Sun Industry Standards Source License Version 1.1
#
#  Sun Microsystems Inc., October, 2000
#
#  GNU Lesser General Public License Version 2.1
#  =============================================
#  Copyright 2000 by Sun Microsystems, Inc.
#  901 San Antonio Road, Palo Alto, CA 94303, USA
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License version 2.1, as published by the Free Software Foundation.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#  MA  02111-1307  USA
#
#
#  Sun Industry Standards Source License Version 1.1
#  =================================================
#  The contents of this file are subject to the Sun Industry Standards
#  Source License Version 1.1 (the "License"); You may not use this file
#  except in compliance with the License. You may obtain a copy of the
#  License at http://www.openoffice.org/license.html.
#
#  Software provided under this License is provided on an "AS IS" basis,
#  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
#  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
#  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
#  See the License for the specific provisions governing your rights and
#  obligations concerning the Software.
#
#  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
#
#  Copyright: 2000 by Sun Microsystems, Inc.
#
#  All Rights Reserved.
#
#  Contributor(s): _______________________________________
#
#
#***********************************************************************/

PRJ = ..$/..
PRJNAME = cli_ure

TARGET = cli_uno_glue
ENABLE_EXCEPTIONS=TRUE

.INCLUDE : settings.mk
.INCLUDE : $(PRJ)$/util$/makefile.pmk

.IF "$(USE_SHELL)"!="4nt"
ECHOQUOTE='
# emacs is a fool: '
.ELSE
ECHOQUOTE=
.ENDIF

.IF "$(ENABLE_MONO)" != "YES"
dummy:
     @echo "Mono binding disabled - skipping ..."
.ELSE

#!!! Always change version if code has changed. Provide a publisher
#policy assembly!!!
ASSEMBLY_VERSION="1.0.0.0"

ASSEMBLY_KEY="$(BIN)$/cliuno.snk"

ASSEMBLY_ATTRIBUTES = $(MISC)$/assembly_cppuhelper.cs

ALLTAR : \
	$(SHL1TARGETN) \
	$(ASSEMBLY_ATTRIBUTES) \
	$(BIN)$/cli_cppuhelper.dll

CSFILES = \
	managed_bootstrap.cs

ASSEMBLY_KEY_X=$(subst,\,\\ $(ASSEMBLY_KEY))

$(ASSEMBLY_ATTRIBUTES) .PHONY:
	$(GNUCOPY) -p assembly.cs $@
	+echo $(ECHOQUOTE) \
	[assembly:System.Reflection.AssemblyVersion( $(ASSEMBLY_VERSION) )] \
	[assembly:System.Reflection.AssemblyKeyFile($(ASSEMBLY_KEY_X))] $(ECHOQUOTE) \
	>> $@

EXTERNAL_DIR=$(PRJ)$/..$/external/cli

$(OUT)$/bin$/cli_types.dll : $(EXTERNAL_DIR)$/cli_types.dll
	+$(COPY) $< $@

$(BIN)$/cli_cppuhelper.dll : $(CSFILES) $(OUT)$/bin$/cli_types.dll
	+$(CSC) $(CSCFLAGS) \
		-target:library \
		-out:$@ \
		-reference:$(OUT)$/bin$/cli_types.dll \
		-reference:System.dll \
		$(CSFILES)
	@echo "If code has changed then provide a policy assembly and change the version!"

CFLAGS += `pkg-config --cflags mono`

SLOFILES= \
	$(SLO)$/native_glue.obj

SHL1OBJS = $(SLOFILES)

SHL1STDLIBS = \
	$(SALLIB) \
	$(CPPULIB) \
	$(CPPUHELPERLIB)

SHL1STDLIBS += `pkg-config --libs mono`

SHL1TARGET = $(TARGET)

.ENDIF

.INCLUDE : $(PRJ)$/util$/target.pmk
.INCLUDE : target.mk

