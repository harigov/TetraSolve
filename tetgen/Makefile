CC=g++
CXXOPTS=-c -arch i386 -arch ppc
LINK=g++
LINKOPTS=-arch i386 -arch ppc -dynamiclib

tetgen.dylib:predicates.o tetgen.o
	$(LINK) $(LINKOPTS) predicates.o tetgen.o -o tetgen.dylib

predicates.o:predicates.cxx tetgen.h
	$(CC) $(CXXOPTS) predicates.cxx -o predicates.o

tetgen.o:tetgen.cxx tetgen.h
	$(CC) $(CXXOPTS) tetgen.cxx -o tetgen.o

clean:
	rm -rf tetgen.o predicates.o tetgen.dylib
