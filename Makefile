SOURCES = src/main.c src/builtin_test_pack.c
OBJ = $(patsubst %.c,%.o, $(SOURCES))
CC = gcc

all: eden
run: eden
	./eden.exe

%.o: %.c
	$(CC) -c -o $@ $<

eden: $(OBJ)
	$(CC) -o $@ $^