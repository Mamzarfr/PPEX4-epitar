CC=gcc
CFLAGS=-std=c99 -pedantic -Werror -Wall -Wextra -Wvla

SRC=src/main.c src/extract.c src/utils.c

all: epitar

epitar: $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

check:
	pytest -v tests/

unit: src/utils.c tests/test_utils.c
	$(CC) $(CFLAGS) -o tests/test_utils $^ -lcriterion
	./tests/test_utils

leak: CFLAGS+=-g -fsanitize=address
leak: clean epitar check unit

clean:
	$(RM) epitar tests/test_utils

.PHONY: all check unit leak clean