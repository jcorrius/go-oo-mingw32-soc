EXTERNAL_WARNINGS_NOT_ERRORS := TRUE

PRJ=..

PRJNAME=scsolver
TARGET=scsolver

# --- Settings -----------------------------------------------------------

.INCLUDE :	settings.mk

# --- Allgemein ----------------------------------------------------------

DLLPRE=
LIB1TARGET= $(SLB)$/$(TARGET).uno.lib
LIB1FILES=	$(SLB)$/numeric.lib	\
		$(SLB)$/service.lib	\
		$(SLB)$/tool.lib	\
		$(SLB)$/ui.lib

SHL1TARGET= $(TARGET).uno
SHL1IMPLIB= i$(SHL1TARGET)
SHL1DEPN=	makefile.mk
SHL1VERSIONMAP=$(PRJNAME).map
SHL1DEF=$(MISC)$/$(SHL1TARGET).def
DEF1NAME=$(SHL1TARGET)
SHL1LIBS=$(LIB1TARGET)
SHL1STDLIBS= \
	$(CPPUHELPERLIB) \
	$(CPPULIB) \
	$(SALLIB) \
	$(TOOLSLIB) \
	$(LPSOLVELIB)

RESLIB1NAME=scsolver
RESLIB1SRSFILES= $(SRS)/scsolver.srs

EXTPACKFILES= \
	$(BIN)/$(SHL1TARGET).oxt \
	$(BIN)/$(SHL1TARGET).rdb \
	$(LB)/$(SHL1TARGET).so \
	../ext/translation \
	../ext/META-INF \
	../ext/description.xml \
	../ext/*.xcu

EXTPACKPATH= \
	-p ../ext \
	-p $(BIN) \
	-p $(LB)

# --- Targets ------------------------------------------------------------

.INCLUDE :	target.mk

ALLTAR:
	$(REGCOMP) -register -wop -r $(BIN)/$(SHL1TARGET).rdb -c $(LB)/$(SHL1TARGET).so
	python ./packExtension.py $(EXTPACKPATH) $(EXTPACKFILES)
