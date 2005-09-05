PRJ=..$/..$/..

PRJNAME=api

TARGET=vba
PACKAGE=org$/openoffice$/vba

# --- Settings -----------------------------------------------------
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

.IF "$(ENABLE_VBA)"!="YES"
dummy:
        @echo "not building vba..."
.ENDIF

# ------------------------------------------------------------------------

IDLFILES=\
	XApplication.idl\
	XCollection.idl\
	XGlobals.idl\
	XRange.idl\
	XWorkbook.idl\
	XWorkbooks.idl\
	XWorksheet.idl\
	XWorksheets.idl\
	XFont.idl\
	XRows.idl\
	XOutline.idl\
	XColumns.idl \
	MSFormReturnTypes.idl	

# ------------------------------------------------------------------

.INCLUDE :  target.mk
