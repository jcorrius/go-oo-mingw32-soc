PRJ=..

PRJNAME=scsolver
TARGET=scsolver

# --- Settings -----------------------------------------------------------

.INCLUDE :	settings.mk

# --- Allgemein ----------------------------------------------------------

LIB1TARGET= $(SLB)$/$(TARGET).uno.lib
LIB1FILES=	$(SLB)$/numeric.lib	\
		$(SLB)$/service.lib		\
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
	$(SALLIB)

# --- Targets ------------------------------------------------------------

.INCLUDE :	target.mk
