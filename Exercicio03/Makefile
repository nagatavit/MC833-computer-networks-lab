CC = gcc
CFLAGS = -Wall -Werror -Werror=vla -pedantic-errors -g

SRCS = $(wildcard *.c)

PROGS = $(patsubst %.c,%,$(SRCS))

all: $(PROGS)

%: %.c

	$(CC) $(CFLAGS)  -o $@ $<
