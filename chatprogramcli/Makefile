OBJS = echocli.o
#LIBS = -l
#PATH = -L
#HEAD = -I
CFLAGS = -Wall -g

echocli : echocli.o
	cc ${CFLAGS} -o echocli ${OBJS}

echocli.o : unphead.h

.PHONY : clean

clean : 
	-rm -f echocli ${OBJS}
