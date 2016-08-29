This program simulates an assembler. It requires as arguments one filename referring to a source code file of (reduced) MIPS. The program uses hash tables for its symbol table and opcode table. The source code is analysed in two passes. If there are any errors in the source, a list of errors is printed to a file with the same name as the argument but with an ".err" extension. If there are no errors, each line of code is translated into a plain text representation of object code then printed to a file with the same name but with an ".obj" extension, and each symbol in the code is printed with its LC to a file with the same name but ".sym".
