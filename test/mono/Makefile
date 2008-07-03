SOURCES=GeneralTableSample.cs \
	SpreadsheetDocHelper.cs \
	SpreadsheetSample.cs \
	ViewSample.cs

all:	test

test:	SpreadsheetSample.exe
	mono SpreadsheetSample.exe

SpreadsheetSample.exe: ${SOURCES}
	mcs SpreadsheetSample.cs SpreadsheetDocHelper.cs \
	    `pkg-config --libs mono-openoffice`

clean:
	rm -f SpreadsheetSample.exe
