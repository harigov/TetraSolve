CC=g++
CXXOPTS=-c `wx-config --cxxflags` -I /usr/X11R6/include -arch i386 -arch ppc
LINK=g++
LINKOPTS=`wx-config --libs` -framework OpenGL -dynamiclib -arch i386 -arch ppc

foldbar.dylib:foldpanelbar.o foldpanelitem.o captionbar.o
	$(LINK) $(LINKOPTS) foldpanelbar.o foldpanelitem.o captionbar.o -o foldbar.dylib

captionbar.o:captionbar.cpp captionbar.h
	$(CC) $(CXXOPTS) captionbar.cpp -o captionbar.o

foldpanelbar.o:foldpanelbar.cpp foldpanelbar.h
	$(CC) $(CXXOPTS) foldpanelbar.cpp -o foldpanelbar.o

foldpanelitem.o:foldpanelitem.cpp foldpanelitem.h
	$(CC) $(CXXOPTS) foldpanelitem.cpp -o foldpanelitem.o

clean:
	rm -rf captionbar.o foldpanelbar.o foldpanelitem.o foldbar.dylib
