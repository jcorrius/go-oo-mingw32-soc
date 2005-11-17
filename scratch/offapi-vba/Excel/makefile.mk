PRJ=..$/..$/..$/..

PRJNAME=api

TARGET=excel
PACKAGE=org$/openoffice$/vba$/Excel

# --- Settings -----------------------------------------------------
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

.IF "$(ENABLE_VBA)"!="YES"
dummy:
        @echo "not building vba..."
.ENDIF

# ------------------------------------------------------------------------

IDLFILES=\
	XlCutCopyMode.idl \
	XlFindLookIn.idl\
	Constants.idl \
	XlCalculation.idl \
	XlCellType.idl

# ------------------------------------------------------------------

.INCLUDE :  target.mk

