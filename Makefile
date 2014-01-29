OS := $(shell uname)

ifeq ($(OS), Darwin)
	CC = clang
else
	CC = gcc
endif

CFLAGS = -ggdb3 -O0 -std=c99 -Wall -Werror
EXE = hdiff
SRC_FILES = main.c \
			LongList/LongList.c

OBJS = $(SRC_FILES:.c=.o)

$(EXE): $(OBJS) Makefile
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
	rm *.o

$(OBJS): Makefile

clean:
	rm -f core $(EXE) *.o
	rm -rf *.dSYM
