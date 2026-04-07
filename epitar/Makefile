CC=gcc
CFLAGS=-std=c99 -pedantic -Werror -Wall -Wextra -Wvla
LDFLAGS=

SRC=src/main.c src/extract.c src/parse_opts.c src/tar_utils.c src/extract_utils.c src/archive.c src/archive_utils.c

OBJ=$(SRC:.c=.o)
UNIT_OBJ=tests/test_utils.o src/parse_opts.o src/tar_utils.o src/extract_utils.o

all: epitar

epitar: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

check:
	pytest -v tests/

tests/test_utils: $(UNIT_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ -lcriterion

unit: tests/test_utils
	./tests/test_utils

leak: CFLAGS+=-g -fsanitize=address
leak: LDFLAGS=-fsanitize=address
leak: clean epitar check unit

clean:
	$(RM) epitar tests/test_utils $(OBJ) tests/test_utils.o

.PHONY: all check unit leak clean unit epitar