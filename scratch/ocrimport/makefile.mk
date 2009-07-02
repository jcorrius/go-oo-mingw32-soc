#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
#
# Copyright 2008 by Sun Microsystems, Inc.
#
# OpenOffice.org - a multi-platform office productivity suite
#
# $RCSfile: makefile.mk,v $
#
# $Revision: 1.10 $
#
# This file is part of OpenOffice.org.
#
# OpenOffice.org is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3
# only, as published by the Free Software Foundation.
#
# OpenOffice.org is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License version 3 for more details
# (a copy is included in the LICENSE file that accompanied this code).
#
# You should have received a copy of the GNU Lesser General Public License
# version 3 along with OpenOffice.org.  If not, see
# <http://www.openoffice.org/license.html>
# for a copy of the LGPLv3 License.
#
#*************************************************************************

PRJ=..$/..
PRJNAME=sdext
TARGET=OCRImport
GEN_HID=FALSE
EXTNAME=ocr

ENABLE_EXCEPTIONS=TRUE

# --- Settings ----------------------------------

.INCLUDE : rtlbootstrap.mk
.INCLUDE : settings.mk
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

.IF "$(ENABLE_OCRIMPORT)" == "NO"
@all:
	@echo "OCR Import build disabled."
.ELSE

DLLPRE=
common_build_zip=

# --- Files -------------------------------------

SLOFILES=										\
	$(SLO)$/MyProtocolHandler.obj				\
	$(SLO)$/MyJob.obj				\
	$(SLO)$/MyListener.obj		\
	$(SLO)$/WriterListener.obj					\
	$(SLO)$/CalcListener.obj			\
	$(SLO)$/ListenerHelper.obj	\
	$(SLO)$/exports.obj

# --- Library -----------------------------------

SHL1TARGET=		$(TARGET).uno

SHL1STDLIBS=	$(CPPUHELPERLIB)	\
				$(CPPULIB)			\
				$(SALLIB)
SHL1DEPN=
SHL1IMPLIB=		i$(SHL1TARGET)
SHL1LIBS=		$(SLB)$/$(TARGET).lib
SHL1DEF=		$(MISC)$/$(SHL1TARGET).def
SHL1VERSIONMAP=	exports.map
SHL1RPATH=      OXT
DEF1NAME=		$(SHL1TARGET)

ZIP1TARGET=		ocrimport
ZIP1DIR=		$(MISC)$/$(TARGET)
ZIP1EXT=		.oxt
ZIP1FLAGS=-r
ZIP1LIST=		*

DESCRIPTION:=$(ZIP1DIR)$/description.xml

COMPONENT_FILES=																			\
    $(ZIP1DIR)$/Jobs.xcu							\
    $(ZIP1DIR)$/ProtocolHandler.xcu   \
    $(ZIP1DIR)$/Addons.xcu   \
    $(ZIP1DIR)$/WriterWindowState.xcu   \
    $(ZIP1DIR)$/CalcWindowState.xcu


COMPONENT_BITMAPS=					\
	$(ZIP1DIR)$/logo_big.png	\
	$(ZIP1DIR)$/logo_small.png	

COMPONENT_MANIFEST= 							\
	$(ZIP1DIR)$/META-INF$/manifest.xml

COMPONENT_LIBRARY= 								\
	$(ZIP1DIR)$/$(TARGET).uno$(DLLPOST)

COMPONENT_HELP= 								\
	$(ZIP1DIR)$/help/component.txt

ZIP1DEPS=					\
	$(DESCRIPTION)			\
	$(COMPONENT_MANIFEST)	\
	$(COMPONENT_FILES)		\
	$(COMPONENT_BITMAPS)	\
	$(COMPONENT_LIBRARY)	\
	$(COMPONENT_HELP)

PLATFORMID:=$(RTL_OS:l)_$(RTL_ARCH:l)


# --- Targets ----------------------------------

.INCLUDE : target.mk

$(COMPONENT_MANIFEST) : $$(@:f)
	@-$(MKDIRHIER) $(@:d)
    +$(TYPE) $< | $(SED) "s/SHARED_EXTENSION/$(DLLPOST)/" > $@

$(COMPONENT_HELP) : help$/$$(@:f)
	@@-$(MKDIRHIER) $(@:d)
    $(COPY) $< $@

#$(COMPONENT_FILES) : $$(@:f)
#	@-$(MKDIRHIER) $(@:d)
#    +$(COPY) $< $@

$(COMPONENT_BITMAPS) : bitmaps$/$$(@:f)
	@-$(MKDIRHIER) $(@:d)
    +$(COPY) $< $@

$(COMPONENT_LIBRARY) : $(DLLDEST)$/$$(@:f)
	@-$(MKDIRHIER) $(@:d)
    +$(COPY) $< $@
.IF "$(OS)$(CPU)"=="WNTI"
 .IF "$(COM)"=="GCC"
    $(GNUCOPY) $(SOLARBINDIR)$/mingwm10.dll $(ZIP1DIR)
 .ELSE
	.IF "$(PACKMS)"!=""
		.IF "$(CCNUMVER)" <= "001399999999"
			$(GNUCOPY) $(PACKMS)$/msvcr71.dll $(ZIP1DIR)
			$(GNUCOPY) $(PACKMS)$/msvcp71.dll $(ZIP1DIR)
		.ELSE
			.IF "$(CCNUMVER)" <= "001499999999"
				$(GNUCOPY) $(PACKMS)$/msvcr80.dll $(ZIP1DIR)
				$(GNUCOPY) $(PACKMS)$/msvcp80.dll $(ZIP1DIR)
		    	$(GNUCOPY) $(PACKMS)$/msvcm80.dll $(ZIP1DIR)
				$(GNUCOPY) $(PACKMS)$/Microsoft.VC80.CRT.manifest $(ZIP1DIR)
			.ELSE
				$(GNUCOPY) $(PACKMS)$/msvcr90.dll $(ZIP1DIR)
				$(GNUCOPY) $(PACKMS)$/msvcp90.dll $(ZIP1DIR)
		    	$(GNUCOPY) $(PACKMS)$/msvcm90.dll $(ZIP1DIR)
			    $(GNUCOPY) $(PACKMS)$/Microsoft.VC90.CRT.manifest $(ZIP1DIR)
			.ENDIF
	    .ENDIF
	.ELSE        # "$(PACKMS)"!=""
		.IF "$(CCNUMVER)" <= "001399999999"
			$(GNUCOPY) $(SOLARBINDIR)$/msvcr71.dll $(ZIP1DIR)
			$(GNUCOPY) $(SOLARBINDIR)$/msvcp71.dll $(ZIP1DIR)
		.ELSE
			.IF "$(CCNUMVER)" <= "001499999999"
		    	$(GNUCOPY) $(SOLARBINDIR)$/msvcr80.dll $(ZIP1DIR)
			    $(GNUCOPY) $(SOLARBINDIR)$/msvcp80.dll $(ZIP1DIR)
				$(GNUCOPY) $(SOLARBINDIR)$/msvcm80.dll $(ZIP1DIR)
				$(GNUCOPY) $(SOLARBINDIR)$/Microsoft.VC80.CRT.manifest $(ZIP1DIR)
			.ELSE
	    		$(GNUCOPY) $(SOLARBINDIR)$/msvcr90.dll $(ZIP1DIR)
	    		$(GNUCOPY) $(SOLARBINDIR)$/msvcp90.dll $(ZIP1DIR)
		    	$(GNUCOPY) $(SOLARBINDIR)$/msvcm90.dll $(ZIP1DIR)
		    	$(GNUCOPY) $(SOLARBINDIR)$/Microsoft.VC90.CRT.manifest $(ZIP1DIR)
			.ENDIF
	    .ENDIF
	.ENDIF         # "$(PACKMS)"!=""
 .ENDIF	#"$(COM)"=="GCC"
.ENDIF


$(ZIP1DIR)/%.xcu : %.xcu
	@@-$(MKDIRHIER) $(@:d)
	$(GNUCOPY) $< $@

$(ZIP1DIR)$/%.xcs : %.xcs
	@@-$(MKDIRHIER) $(@:d)
	$(GNUCOPY) $< $@

# Temporary file that is used to replace some placeholders in description.xml.
DESCRIPTION_TMP:=$(ZIP1DIR)$/description.xml.tmp

.INCLUDE .IGNORE : $(ZIP1DIR)_lang_track.mk
.IF "$(LAST_WITH_LANG)"!="$(WITH_LANG)"
PHONYDESC=.PHONY
.ENDIF			# "$(LAST_WITH_LANG)"!="$(WITH_LANG)"
$(DESCRIPTION) $(PHONYDESC) : $$(@:f)
	@@-$(MKDIRHIER) $(@:d)
	$(PERL) $(SOLARENV)$/bin$/licinserter.pl description.xml registry/LICENSE_xxx $(DESCRIPTION_TMP)
	@echo LAST_WITH_LANG=$(WITH_LANG) > $(ZIP1DIR)_lang_track.mk
	$(TYPE) $(DESCRIPTION_TMP) | sed s/UPDATED_PLATFORM/$(PLATFORMID)/ > $@
	@@-$(RM) $(DESCRIPTION_TMP)


.ENDIF # "$(ENABLE_OCRIMPORT)" != "NO"
