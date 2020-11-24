CC=gcc
CFLAGS=-D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE -O3 -fopenmp -lm

#installs dependencies
install:
	sudo apt install gcc;\
	sudo apt install libomp-dev;\

#build an execultable named main in folder build/ from main.c in folder src/
build:
	mkdir -p .build;\
	${CC} ${CFLAGS} src/main.c -g -o .build/main;

#cleans build folder
clear:
	rm -rf .build/

#runs main executable in folder build/
run:
	./.build/main 10 1 1000.bin
#                 |  |    |-> Arquivo do estado inicial
#                 |  |-> Numero de threads
#                 |-> Numero de iterações

debug:
	gdb ./.build/main
