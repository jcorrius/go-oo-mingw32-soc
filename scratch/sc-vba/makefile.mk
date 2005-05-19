PRJ=..$/..$/..

PRJNAME=sc
TARGET=vba

LIBTARGET=NO
ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  scpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  sc.mk
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

# --- Files --------------------------------------------------------

SLO1FILES =  \
        $(SLO)$/vbaapplication.obj \
        $(SLO)$/vbaglobals.obj \
        $(SLO)$/vbarange.obj \
        $(SLO)$/vbaworkbook.obj \
        $(SLO)$/vbaworkbooks.obj \
        $(SLO)$/vbaworksheet.obj

LIB1TARGET = \
	$(SLB)$/$(TARGET).lib
LIB1OBJFILES = \
	$(SLO1FILES)

# --- Tagets -------------------------------------------------------

.INCLUDE :  target.mk

