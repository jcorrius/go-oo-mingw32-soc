SOURCES=GeneralTableSample.cs \
	SpreadsheetDocHelper.cs \
	SpreadsheetSample.cs \
	ViewSample.cs

all:	test

test:	SpreadsheetSample.exe SpreadsheetSample
	./SpreadsheetSample

SpreadsheetSample.exe: ${SOURCES}
	mcs SpreadsheetSample.cs SpreadsheetDocHelper.cs \
	    `pkg-config --libs mono-ooo-2.0`

clean:
	rm SpreadsheetSample.exe
