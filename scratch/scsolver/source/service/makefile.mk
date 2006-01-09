PRJ=..$/..
PRJNAME=scsolver
TARGET=service
ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

# --- Files --------------------------------------------------------

SLOFILES =  \
        $(SLO)$/service.obj

# --- Tagets -------------------------------------------------------

.INCLUDE :  target.mk

