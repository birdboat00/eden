SOURCES = src/main.c src/pack.c src/op.c src/vm.c src/builtin_test_pack.c
OBJ = $(patsubst %.c,%.o, $(SOURCES))
CC = gcc
ARTIFACT = eden.exe
DUMP_FILE = dump.txt
EDEN_CODE_FILE = code.eden

all: eden
run: eden
	$(ARTIFACT)

cleanartifacts:
	del $(ARTIFACT)
	del $(DUMP_FILE)
	del $(EDEN_CODE_FILE)

dumptestpack: eden
	$(ARTIFACT) --dumptestpack
savetestpack: eden
	$(ARTIFACT) --savetestpack
readtestpack: eden
	$(ARTIFACT) --readtestpack
interptestpack: eden
	$(ARTIFACT) --interptestpack

%.o: %.c
	$(CC) -c -o $@ $<

eden: $(OBJ)
	$(CC) -o $(ARTIFACT) $^
