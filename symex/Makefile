CC=gcc
COPTS=-c -arch i386 -arch ppc
LINK=gcc
LINKOPTS=-arch i386 -arch ppc -dynamiclib

symex.dylib:symex_list.o symex_parse.o symex_write.o
	$(LINK) $(LINKOPTS) symex_list.o symex_parse.o symex_write.o -o symex.dylib

symex_list.o:symex_list.c symex_list.h
	$(CC) $(COPTS) symex_list.c -o symex_list.o

symex_parse.o:symex_parse.c symex_parse.h
	$(CC) $(COPTS) symex_parse.c -o symex_parse.o

symex_write.o:symex_write.c symex_write.h
	$(CC) $(COPTS) symex_write.c -o symex_write.o

clean:
	rm symex_list.o symex_parse.o symex_write.o symex.dylib
