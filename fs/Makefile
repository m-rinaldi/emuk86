INCLUDES    := -I. -isystem../include/
CPPFLAGS    := $(INCLUDES) -Wall -Wextra -Wno-format -Wno-unused-parameter
CPPFLAGS    += -DHOSTED
CFLAGS      := -g -std=gnu99

sources = $(wildcard *.c)
objects = $(sources:%.c=%.o)

.PHONY: default all clean

default: clean all
all: main.exe

main.exe: $(objects)
	gcc -o $@ $^

clean:
	rm -fr *.exe *.o
