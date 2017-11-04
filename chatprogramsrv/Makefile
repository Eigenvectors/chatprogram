OBJS = echosrv.o
#LIBS = -l
#PATH = -L
#HEAD = -I
CFLAGS = -Wall -g

echosrv : ${OBJS}
	cc ${CFLAGS} -o echosrv ${OBJS}

echosrv.o : unphead.h

.PHONY : clean

clean : 
	-rm -f echosrv ${OBJS}
