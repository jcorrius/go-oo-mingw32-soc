EXTERNAL_WARNINGS_NOT_ERRORS := TRUE

PRJ=..$/..
PRJNAME=scsolver
TARGET=tool
ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

# --- Files --------------------------------------------------------

SLOFILES =  \
        $(SLO)$/timer.obj \
        $(SLO)$/global.obj

# --- Tagets -------------------------------------------------------

.INCLUDE :  target.mk

