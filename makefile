all: assembler
assembler: assembler.c data.c label.c operation.c valid.c print.c assembler.h
	gcc assembler.c data.c label.c operation.c valid.c print.c -ansi -Wall -pedantic -g -o assembler
