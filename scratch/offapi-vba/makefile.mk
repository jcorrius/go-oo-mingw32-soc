PRJ=..$/..$/..

PRJNAME=api

TARGET=vba
PACKAGE=org$/openoffice$/vba

# --- Settings -----------------------------------------------------
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

# ------------------------------------------------------------------------

IDLFILES=\
	XApplication.idl\
	XCollection.idl\
	XGlobals.idl\
	XRange.idl\
	XWorkbook.idl\
	XWorkbooks.idl\
	XWorksheet.idl\
	XFont.idl\

# ------------------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE :  $(PRJ)$/util$/target.pmk
