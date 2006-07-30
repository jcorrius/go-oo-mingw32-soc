PRJ=..$/..$/..$/..

PRJNAME=api

TARGET=solver
PACKAGE=org$/openoffice$/sc$/solver

# --- Settings -----------------------------------------------------

.INCLUDE :  $(PRJ)$/util$/makefile.pmk

# ------------------------------------------------------------------------

IDLFILES= \
	Bound.idl \
	Equality.idl \
	Goal.idl \
	XLpAlgorithm.idl \
	XLpModel.idl
	
# ------------------------------------------------------------------

.INCLUDE :  target.mk
