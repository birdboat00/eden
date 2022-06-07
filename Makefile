SOURCES = src/main.c src/pack.c src/op.c src/builtin_test_pack.c
OBJ = $(patsubst %.c,%.o, $(SOURCES))
CC = gcc

all: eden
run: eden
	./eden.exe

cleanartifacts:
	del eden.exe
	del code.eden
	del dump.txt

dumptestpack: eden
	./eden.exe --dump
savetestpack: eden
	./eden.exe --save
readtestpack: eden
	./eden.exe --read

%.o: %.c
	$(CC) -c -o $@ $<

eden: $(OBJ)
	$(CC) -o $@ $^
