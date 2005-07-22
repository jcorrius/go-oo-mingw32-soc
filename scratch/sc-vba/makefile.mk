PRJ=../..$/..

PRJNAME=
TARGET=vbaobj
NO_BSYMBOLIC=	TRUE
ENABLE_EXCEPTIONS=TRUE
COMP1TYPELIST=$(TARGET)
COMPRDB=$(SOLARBINDIR)$/types.rdb

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk
.INCLUDE :  sv.mk
DLLPRE =

# ------------------------------------------------------------------

SLOFILES= \
		$(SLO)$/service.obj \
		$(SLO)$/vbaglobals.obj \
		$(SLO)$/vbaworkbook.obj \
		$(SLO)$/vbaworksheets.obj \
		$(SLO)$/vbaapplication.obj \
		$(SLO)$/vbarange.obj \
		$(SLO)$/vbaworkbooks.obj \
		$(SLO)$/vbacolumns.obj \
		$(SLO)$/vbaworksheet.obj \
		$(SLO)$/vbarows.obj \
		$(SLO)$/vbafont.obj

SHL1TARGET= $(TARGET)$(UPD)$(DLLPOSTFIX).uno
SHL1IMPLIB=	i$(TARGET)

SHL1VERSIONMAP=$(TARGET).map
SHL1DEF=$(MISC)$/$(SHL1TARGET).def
DEF1NAME=$(SHL1TARGET)

SHL1STDLIBS= \
		$(CPPUHELPERLIB) \
		$(CPPULIB) \
		$(TOOLSLIB) \
		$(SALLIB)

SHL1DEPN=
SHL1LIBS=$(SLB)$/$(TARGET).lib

# --- Targets ------------------------------------------------------

.INCLUDE :	target.mk
