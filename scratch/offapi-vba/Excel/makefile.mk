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
	XlDirection.idl


# ------------------------------------------------------------------

.INCLUDE :  target.mk

