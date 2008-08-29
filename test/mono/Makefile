SOURCES=GeneralTableSample.cs \
	SpreadsheetDocHelper.cs \
	SpreadsheetSample.cs \
	ViewSample.cs

all:	test

test:	SpreadsheetSample.exe
	mono --debug SpreadsheetSample.exe

SpreadsheetSample.exe: ${SOURCES}
	mcs -debug SpreadsheetSample.cs SpreadsheetDocHelper.cs \
	    `pkg-config --libs mono-ooo-3.0`

clean:
	rm -f SpreadsheetSample.exe
