PRJ=../..

SOURCE_PATH=$(PRJ)/source
NUMERIC_PATH=$(SOURCE_PATH)/numeric

CPPFLAGS=-I$(PRJ)/source/inc -DSCSOLVER_UNITTEST
CXXFLAGS=-Wall -O0 -g

TESTFILES = \
	matrix \
	polyeqnsolver \
	quadfitlinesearch

build: $(TESTFILES)

global.o: $(SOURCE_PATH)/tool/global.cxx
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c $<

matrix.o: $(NUMERIC_PATH)/matrix.cxx
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c $<

funcobj.o: $(NUMERIC_PATH)/funcobj.cxx
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c $<

matrix: matrix.o matrix_test.o global.o
	$(CXX) -o $@ matrix.o matrix_test.o global.o

matrix_test.o: $(NUMERIC_PATH)/matrix_test.cxx
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c $<

polyeqnsolver.o: $(NUMERIC_PATH)/polyeqnsolver.cxx
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c $<

polyeqnsolver_test.o: $(NUMERIC_PATH)/polyeqnsolver_test.cxx
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c $<

polyeqnsolver: polyeqnsolver.o polyeqnsolver_test.o matrix.o global.o
	$(CXX) -o $@ polyeqnsolver.o polyeqnsolver_test.o matrix.o global.o

quadfitlinesearch.o: $(NUMERIC_PATH)/quadfitlinesearch.cxx
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c $<

quadfitlinesearch_test.o: $(NUMERIC_PATH)/quadfitlinesearch_test.cxx
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c $<

quadfitlinesearch: quadfitlinesearch.o quadfitlinesearch_test.o matrix.o global.o funcobj.o
	$(CXX) -o $@ quadfitlinesearch.o quadfitlinesearch_test.o matrix.o global.o funcobj.o

clean:
	rm -f *.o $(TESTFILES)

