PRJ=..$/..
PRJNAME=scsolver
TARGET=ui
ENABLE_EXCEPTIONS=TRUE

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

# --- Files --------------------------------------------------------

SLOFILES =  \
        $(SLO)$/basedlg.obj \
        $(SLO)$/baselistener.obj \
        $(SLO)$/dialog.obj \
        $(SLO)$/global.obj \
        $(SLO)$/listener.obj \
        $(SLO)$/lpbuilder.obj \
        $(SLO)$/msgdlg.obj \
        $(SLO)$/solvemodel.obj \
        $(SLO)$/solver.obj \
        $(SLO)$/unoglobal.obj \
        $(SLO)$/unohelper.obj \
        $(SLO)$/xcalc.obj

# --- Tagets -------------------------------------------------------

.INCLUDE :  target.mk

