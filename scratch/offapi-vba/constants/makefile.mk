#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile$
#
#   $Revision$
#
#   last change: $Author$ $Date$
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
TARGET=constants
PRJ=..
PRJNAME=oovbaapi

# --- Settings -----------------------------------------------------

.INCLUDE : settings.mk

# symbol files of Microsoft API
MYSYMFILES = access.api vba.api adodb.api msforms.api word.api dao.api powerpoint.api excel.api stdole.api

# ------------------------------------------------------------------

MY_GEN_IDL_PATH=$(MISC)$/idl
MY_GEN_UCR_PATH=$(OUT)$/ucr$/constant
MYCLASSPATH:=$(SOLARBINDIR)$/xalan.jar$(PATH_SEPERATOR)$(SOLARBINDIR)$/xercesImpl.jar$(PATH_SEPERATOR)$(SOLARBINDIR)$/xml-apis.jar$(PATH_SEPERATOR)$(SOLARBINDIR)$/serializer.jar

MYDONEFILES += $(foreach,i,$(MYSYMFILES) $(MISC)$/$(i:b).done)

# --- Targets ------------------------------------------------------

ALLTAR: $(UCR)$/constant_types.rdb 

GEN_IDL: $(MY_GEN_IDL_PATH) $(MY_GEN_UCR_PATH) $(MYDONEFILES) 

$(UCR)$/constant_types.rdb : constant_types.rdb
	+ $(GNUCOPY) -f constant_types.rdb $(UCR)$/constant_types.rdb

$(MISC)$/%.done : %.api
    @echo Parsing $?
    +java -cp $(MYCLASSPATH) -Xbootclasspath/p:$(MYCLASSPATH)\
    org.apache.xalan.xslt.Process -v -xsl oovbaconsts.xsl -param IDL_DIRECTORY $(MISC)$/idl/ -in $?
    $(TOUCH) $@

$(MY_GEN_UCR_PATH)$/%.urd: $(MY_GEN_IDL_PATH)$/%.idl
    $(IDLC) -C -O $(MY_GEN_UCR_PATH) -I $(MY_GEN_IDL_PATH)$(PATH_SEPERATOR)$(SOLARIDLDIR) $?
	$(REGMERGE) constant_types.rdb /UCR $@

$(MY_GEN_IDL_PATH) : 
    $(MKDIR) $@

$(MY_GEN_UCR_PATH) :
    $(MKDIR) $(OUT)$/ucr
    $(MKDIR) $@

MYIDLFILES=$(shell ls $(MY_GEN_IDL_PATH))
MYURDFILES=$(foreach,i,$(MYIDLFILES) $(MY_GEN_UCR_PATH)$/$(i:b).urd)

#MY_IDLC: $(MYURDFILES)

CLEAN:
    @$(RM) $(MY_GEN_IDL_PATH)
    @$(RM) $(MY_GEN_UCR_PATH)
    @$(RM) $(foreach,i,$(MYSYMFILES) $(MISC)$/$(i:b).done)
    @$(RM) $(UCR)$/constant_types.rdb

