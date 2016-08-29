/* Author: Marcus Ross
 *         MR867434
 * Description: This program requires as arguments one filename referring to a source code file of (reduced) MIPS. The program uses hash tables for its symbol table and opcode table. The source code is analysed in two passes. If there are any errors in the source, a list of errors is printed to a file with the same name as the argument but with an ".err" extension. If there are no errors, each line of code is translated into a plain text representation of object code then printed to a file with the same name but with an ".obj" extension, and each symbol in the code is printed with its LC to a file with the same name but ".sym".
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list.h"
#include "proto.h"

int main(int argc, char** argv) {
	/* declarations */
	FILE* sourceFile; /* for holding the assembly source code file */
	int i; /* for loop counter */
	
	if (argc != 2) { /* kill program if not 1 argument */
		fprintf(stderr, "Exactly one input filename required.\n");
		return -1;
	}
	
	sourceFile = fopenErrCheck(argv[1], "r", "Failed to open source code file"); /* open the file named by the argument */
	
	for (i = strlen(argv[1]); i >= 0; i--) { /* start from end of the filename and traverse toward the start */
		if (argv[1][i] == '.') { /* once a period is reached */
			argv[1][i] = '\0'; /* truncate it and everything beyond it, to get the root name */
			break;
		}
	}
	
	assemble(sourceFile, argv[1]);
	
	return 0;
}