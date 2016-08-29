# Author: Marcus Ross
#         MR867434

p4: clean main.o assemble.o hash.o opcode.o symbol.o string.o error.o
	gcc main.o assemble.o hash.o opcode.o error.o symbol.o string.o -o p4
main.o: main.c list.h proto.h
	gcc -c main.c
assemble.o: assemble.c list.h proto.h const.h
	gcc -c assemble.c
hash.o: hash.c
	gcc -c hash.c
opcode.o: opcode.c list.h const.h
	gcc -c opcode.c
symbol.o: symbol.c list.h proto.h const.h
	gcc -c symbol.c
string.o: string.c
	gcc -c string.c
error.o: error.c list.h proto.h
	gcc -c error.c
clean:
	rm -f *.o core
