PRJ=..$/..$/..

PRJNAME=api

TARGET=vba
PACKAGE=org$/openoffice$/vba

# --- Settings -----------------------------------------------------
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

# ------------------------------------------------------------------------

IDLFILES=\
	OptionalAny.idl\
	XApplication.idl\
	XGlobals.idl\
	XRange.idl\
	XWorkbook.idl\
	XWorksheet.idl\

# ------------------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE :  $(PRJ)$/util$/target.pmk
