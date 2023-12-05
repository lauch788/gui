.POSIX:
.SUFFIXES:

CC = gcc -Wall -pedantic
CFLAGS = -O2 -ggdb3
LIBS = -lSDL2 -lm

SRC =\
	src/main

gui_test: $(SRC:=.o)
	$(CC) -o $@ $(CFLAGS) $^ $(LIBS)

src/main.o: src/main.c src/ease.h

$(SRC:=.o):
	$(CC) -c -o $@ $(CFLAGS) $(@:.o=.c)

clean:
	rm -f $(SRC:=.o) gui_test

.PHONY: clean
