CC=gcc
CFLAGS=-std=c99 -pedantic -Werror -Wall -Wextra -Wvla

SRC=src/main.c src/extract.c src/utils.c

all: epitar

epitar: $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

check:
	pytest -v tests/

leak: CFLAGS+=-g -fsanitize=address
leak: clean epitar check

clean:
	$(RM) epitar

.PHONY: all check leak clean