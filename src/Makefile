CC = gcc
CFLAGS = -Wall -Wextra -Wshadow -Wdisabled-optimization \
		 -Wlogical-op -Wconversion -Wcast-qual \
		 -Wdeprecated -Wundef -Wpointer-arith \
		 -Wnull-dereference
OPTFLAGS = -march=native -Ofast -flto -frename-registers -DNDEBUG
DBGFLAGS = -ggdb

BINARY = emain

OBJ_FILES = $(patsubst %.c, obj/%.o, $(wildcard *.c))

all: $(BINARY)

perf: $(BINARY) 
perf: CFLAGS += $(OPTFLAGS)

debug: $(BINARY)
debug: CFLAGS += $(DBGFLAGS)


$(BINARY): $(OBJ_FILES)
	$(CC) $(CFLAGS) $^ -o $@

obj/tests.o: bigint.h

obj/main.o: main.c bigint.h tests.h | obj
	$(CC) $(CFLAGS) -c $< -o $@

obj/%.o: %.c %.h Makefile | obj
	$(CC) $(CFLAGS) -c $< -o $@

obj: 
	mkdir obj


.PHONY: clean
clean:
	rm -f $(BINARY) obj/*.o
