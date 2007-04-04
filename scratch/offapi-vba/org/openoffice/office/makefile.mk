PRJ=..$/..$/..

PRJNAME=oovapi

TARGET=office
PACKAGE=org$/openoffice$/Office

# --- Settings -----------------------------------------------------
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

.IF "$(ENABLE_VBA)"!="YES"
dummy:
        @echo "not building vba..."
.ENDIF

# ------------------------------------------------------------------------


CONST_IDLFILES=\
	MsoArrowheadStyle.idl \
	MsoLineDashStyle.idl \
	MsoScaleFrom.idl \
	MsoShapeType.idl \
	MsoZOrderCmd.idl \


IDLFILES+=$(CONST_IDLFILES)
# ------------------------------------------------------------------

.INCLUDE :  target.mk
