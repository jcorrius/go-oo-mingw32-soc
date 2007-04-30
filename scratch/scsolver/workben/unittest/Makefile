PRJ=../..

SOURCE_PATH=$(PRJ)/source
NUMERIC_PATH=$(SOURCE_PATH)/numeric

CPPFLAGS=-I$(PRJ)/source/inc
CXXFLAGS=-Wall -Os

TESTFILES = \
	matrix

build: $(TESTFILES)

global.o: $(SOURCE_PATH)/tool/global.cxx
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c $<

matrix.o: $(NUMERIC_PATH)/matrix.cxx
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c $<

matrix_test.o: $(NUMERIC_PATH)/matrix_test.cxx
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c $<

matrix: matrix.o matrix_test.o global.o
	$(CXX) -o $@ matrix.o matrix_test.o global.o

clean:
	rm -f *.o $(TESTFILES)

