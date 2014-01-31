OS := $(shell uname)

ifeq ($(OS), Darwin)
	CC = clang
else
	CC = gcc
endif

CFLAGS = -ggdb3 -O0 -std=c99 -Wall
EXE = hdiff
SRC_FILES = main.c \
			LongList/LongList.c \
			ByteArray/ByteArray.c

OBJS = $(SRC_FILES:.c=.o)

$(EXE): $(OBJS) Makefile
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
	rm *.o
	rm -f LongList/*.o
	rm -f ByteArray/*.o

$(OBJS): Makefile

clean:
	rm -f core $(EXE) *.o
	rm -rf *.dSYM
	rm -f LongList/*.o
	rm -f ByteArray/*.o
