CC = gcc 
CFLAGS = -ggdb3 -O0 -std=c99 -Wall -Werror
EXE = hdiff
SRC_FILES = hdiff.c
OBJS = $(SRC_FILES:.c=.o)

$(EXE): $(OBJS) Makefile
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

$(OBJS): Makefile

clean:
	rm -f core $(EXE) *.o
	rm -rf *.dSYM
