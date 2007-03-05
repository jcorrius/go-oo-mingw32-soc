PRJ=..$/..$/..

PRJNAME=oovbapi

TARGET=msforms
PACKAGE=org$/openoffice$/msforms

# --- Settings -----------------------------------------------------
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

.IF "$(ENABLE_VBA)"!="YES"
dummy:
        @echo "not building vba..."
.ENDIF

# ------------------------------------------------------------------------

IDLFILES=\
	MSFormReturnTypes.idl \
	XComboBox.idl \
	XButton.idl \
	XControl.idl \
	XLabel.idl \
	XTextBox.idl \
	XRadioButton.idl \
	XListBox.idl 

# ------------------------------------------------------------------

.INCLUDE :  target.mk
