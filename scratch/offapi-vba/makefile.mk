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
	XOutline.idl\
	MSFormReturnTypes.idl\
	XInterior.idl\
	XWorksheetFunction.idl\
	XlPasteSpecialOperation.idl\
	XlPasteType.idl\
	XlBuiltInDialog.idl\
	XVBAToOOEventDescGen.idl\
	XWindow.idl\
	XChartObject.idl\
	XChartObjects.idl\
	XChart.idl\
	XSeriesCollection.idl\
	XSeries.idl\
	XDialog.idl \
	XDialogs.idl 

# ------------------------------------------------------------------

.INCLUDE :  target.mk
