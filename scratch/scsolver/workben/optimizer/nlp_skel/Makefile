
SOURCE_PATH=../../../source
NUMERIC_PATH=$(SOURCE_PATH)/numeric

CPPFLAGS=-I../../../source/inc
CXXFLAGS=-Wall -Os

OBJFILES= \
	myoptimizer.o \
	main.o \
	funcobj.o \
	myfunctor.o \
	nlpbase.o \
	nlpmodel.o \
	exception.o \
	global.o

EXEC=skel

$(EXEC): $(OBJFILES)
	$(CXX) -o $@ $(OBJFILES)

main.o: main.cxx
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

funcobj.o: $(NUMERIC_PATH)/funcobj.cxx
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

myfunctor.o: myfunctor.cxx
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

myoptimizer.o: myoptimizer.cxx
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

nlpbase.o: $(NUMERIC_PATH)/nlpbase.cxx
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

nlpmodel.o: $(NUMERIC_PATH)/nlpmodel.cxx
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

exception.o: $(NUMERIC_PATH)/exception.cxx
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

global.o: $(SOURCE_PATH)/tool/global.cxx
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -f $(OBJFILES) $(EXEC)
