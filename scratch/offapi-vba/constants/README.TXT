VBA Constants

constant_types.rdb represents the state of constants from the latest MS Office version.
A new file has only to be created when a new version of MS Office 
(with extended constant definition) has to be supported.

This directory is not 'REALLY' taken into account in the normal build process. It just copies the  constant_types.rdb into $(UCR)

To build a new constant_types.rdb, which will be delivered in the ucr directory of the output
tree, do this:

Type 'dmake GEN_IDL' in this directory to create the idl files. Then uncomment the line
MY_IDLC: $(MYURDFILES)
in the makefile.mk in this directory and type 'dmake MY_IDLC' to build the
constant_types.rdb.

Remark: the following '.api' ( meta data ) files are used:
- access.api
- adodb.api
- dao.api
- excel.api
- msforms.api
- powerpoint.api
- stdole.api
- vba.api
- word.api
- ApiSymbols.dtd

NOTE: the above works best after removing the output ( e.g. unxlngxxx, winntxxx  ) directories due to the completely lame way this makefile is written
