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
	XlCreator.idl \
	XlAutoFillType.idl \
	XlCutCopyMode.idl \
	XlFindLookIn.idl\
	Constants.idl \
	XlCalculation.idl \
	XlCellType.idl \
	XlCommentDisplayMode.idl \
	XlLookAt.idl \
	XlSearchOrder.idl \
	XlSortOrder.idl \
	XlSortMethod.idl \
	XlYesNoGuess.idl \
	XlSortOrientation.idl \
	XlDirection.idl \
	XlSortDataOption.idl \
	XlDeleteShiftDirection.idl


# ------------------------------------------------------------------

.INCLUDE :  target.mk

