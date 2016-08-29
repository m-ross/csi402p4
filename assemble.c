/* Author: Marcus Ross
 *         MR867434
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "proto.h"
#include "const.h"

/* This function's arguments are a pointer to file containing MIPS code and a string containing the name of that file with its extension removed. The function, in two passes, traverses the code file, and if there are any errors in the program, a list of errors is printed to the file named by the second argument followed by an ".err" extension. If there are no errors, each line of code is translated into a plain text representation of object code then printed to a file named by the second argument followed by ".obj", and each symbol in the code is printed with its LC to a file named by the second argument followed by ".sym". */
void assemble(FILE* sourceFile, char* rootName) {
	/* declarations */
	opcodeNode **opcodeTable; /* hash table to hold all opcodes to be looked up */
	symbolNode **symbolTable; /* hash table to hold all symbols to be printed */
	errorNode **errors; /* linked list to hold all errors to be printed */
	FILE *symFile; /* name.sym */
	FILE *errFile; /* name.err */
	FILE *objFile; /* name.obj */
	char *outFilename; /* to hold each file's name as it's opened */
	char line[MAX_LINE_LEN]; /* holds each line of code */
	char *token; /* points to each word of each line */
	int rootNameLen; /* length of argument rootName */
	int i; /* for loop counter */
	int lineLen; /* for traversing each line from the tail end */
	int LC = -1;
	int lineNum = 0; /* line of the code file */
	enum segments {text, data};
	int segment = -1; /* keep track of whether in text or data segment */
	enum formats {R, I, J};
	int format = -1; /* keep track of what format is of the instruction being processed */
	int opcode;
	int regSrc0 = 0, regSrc1 = 0, regTrgt = 0; /* parts of the assembled binary instruction */
	int instruction; /* also part of the instruction */
	int operand; /* also part of the instruction */
	int shiftAmt; /* also part of the instruction */
	
	opcodeTable = (opcodeNode **) malloc(sizeof(opcodeNode *) * OPCODE_HASH_SIZE);
	symbolTable = (symbolNode **) malloc(sizeof(symbolNode *) * SYMBOL_HASH_SIZE);
	errors = (errorNode **) malloc(sizeof(errorNode *));
	*symbolTable = 0; /* because Unix mysteriously assigns a pointer to these despite them being unitialised */
	*opcodeTable = 0; /* adding to these arrays relies on the node at [0] initially being null */
	*errors = 0;
	
	buildOpcodeTable(opcodeTable); /* fills opcodeTable with all the instructions and their opcodes */
	
	rootNameLen = strlen(rootName); /* to create the names of the output files */
	outFilename = (char *) malloc(sizeof(char) * rootNameLen + 4); /* +4 to make room for the extension */
	strcpy(outFilename, rootName);
	
	strcat(outFilename, ".obj"); /* use the string for the object file name first */
	objFile = fopenErrCheck(outFilename, "w", "Failed to open the output object file");
	
	while (fgets(line, MAX_LINE_LEN, sourceFile)) { /* first pass; get multiply defined errors and build symbol table */
		lineNum ++;
		
		if (line[0] == '#') { /* if the entire line is a comment, ignore it */
			continue;
		}
		
		lineLen = strlen(line); /* get the length of the line */
		lineLen = removeInlineComment(line, lineLen); /* remove any in-line comments the line has */
		removeNewline(line, lineLen); /* remove a newline character if the line has one */
		token = strtok(line, " "); /* delimit the line by spaces */
		
		if (!token) { /* if line contained no non-spaces, ignore it */
			continue;
		}
		
		if (token[0] == '.') { /* if the line has a pseudo op */
			if (strcmp(token, ".text") == 0) { /* if the pseudo op is .text */
				segment = text; /* now in the text segment */
				continue;
			}
			
			if (strcmp(token, ".data") == 0) { /* if the pseudo op is .data */
				segment = data; /* now in the data segment */
				continue;
			}
		}
		
		if (segment == text) { /* if in the text segment */
			LC ++;
			
			if (token[strlen(token) - 1] == ':') { /* if the last character in the word is a colon */
				token[strlen(token) - 1] = '\0'; /* then the word is a label, so truncate the colon off */
				addSymbol(symbolTable, token, LC, errors, lineNum); /* and add that symbol to the table */
				token = strtok(NULL, " "); /* get next word */
				
				if (strcmp(token, ".word") == 0) { /* if a variable is being declared */
					token = strtok(NULL, " "); /* increment LC by the number of words being declared, then go to the next line */
					token = strtok(token, ":");
					token = strtok(NULL, ":");
					LC += atoi(token) - 1;
					continue;
				}
			} /* if variables are not being declared, then the word is an instruction */
			
			if (findOpcode(opcodeTable, token) == -1) { /* findOpcode returns -1 when the instruction is not in the opcodeTable */
				addError(errors, illegalOp, lineNum);
			}
			
			continue;
		}
		
		if (segment == data) { /* if in the data segment */
			LC ++;
			
			if (token[strlen(token) - 1] == ':') { /* if the last character in the word is a colon */
				token[strlen(token) - 1] = '\0'; /* then the word is a label, so truncate the colon off */
				addSymbol(symbolTable, token, LC, errors, lineNum); /* and add that symbol to the table */
				token = strtok(NULL, " "); /* get next word */
				
				if (strcmp(token, ".word") == 0) { /* if a variable is being declared */
					token = strtok(NULL, " "); /* increment LC by the number of words being declared, then go to the next line */
					token = strtok(token, ":");
					token = strtok(NULL, ":");
					LC += atoi(token) - 1;
				}
			}
			else { /* if there is a label in the data segment, but a variable is *not* being declared, then syntax error */
				addError(errors, dataSegment, lineNum);
			}
			
			continue;
		}
	}
	
	fseek(sourceFile, 0, SEEK_SET); /* rewind for pass two */
	LC = -1; /* and reset the LC, lineNum, and segment counters */
	lineNum = 0;
	segment = -1;
	
	while (fgets(line, MAX_LINE_LEN, sourceFile)) { /* pass two */
		lineNum ++;
		
		if (line[0] == '#') { /* if the entire line is a comment, ignore it */
			continue;
		}
		
		lineLen = strlen(line); /* get the length of the line */
		lineLen = removeInlineComment(line, lineLen); /* remove any in-line comments the line has */
		removeNewline(line, lineLen); /* remove a newline character if the line has one */
		token = strtok(line, " "); /* delimit the line by spaces */
		
		if (!token) { /* if line contained no non-spaces, ignore it */
			continue;
		}
		
		if (token[0] == '.') { /* if the line has a pseudo op */
			if (strcmp(token, ".text") == 0) { /* if the pseudo op is .text */
				segment = text; /* now in the text segment */
				continue;
			}
			
			if (strcmp(token, ".data") == 0) { /* if the pseudo op is .data */
				segment = data; /* now in the data segment */
				continue;
			}
		}
		
		if (segment == text) { /* if in the text segment */
			LC ++;
			
			if (token[strlen(token) - 1] == ':') { /* if the last character in the word is a colon */
				token = strtok(NULL, " "); /* ignore the labels in pass two, so get the next word */
				
				if (strcmp(token, ".word") == 0) {
					token = strtok(NULL, " ");
					token = strtok(token, ":");
					token = strtok(NULL, ":");
					LC += atoi(token) - 1;
					continue;
				}
				
				if (strcmp(token, ".resw") == 0) {
					continue;
				}
			}
			
			fprintf(objFile, "%4X", LC); /* this should be reached only if token is an instruction, so print to obj file */
			opcode = findOpcode(opcodeTable, token);
			
			if (opcode == -1) { /* findOpcode returns -1 if it didn't find the instruction */
				continue; /* so go to the next line; error has already been noted in the first pass */
			}
			else {
				if (opcode < 18) {
					format = R;
				}
				else {
					if (opcode > 25) {
						format = J;
					}
					else {
						format = I;
					}
				}
			}
			
			instruction = opcode; /* put the opcode in the instruction */
			instruction <<= 5; /* move the opcode to more significant bits to make room for the next part  */
			token = strtok(NULL, " "); /* get the arguments of the instruction */
			token = strtok(token, ",");
			
			if (format == R) {
				if (opcode == 0) {
					instruction <<= 15;
				}
				else {
					if (opcode < 6 || (opcode > 6 && opcode < 10)) { /* for instructions add, sub, mul, div, mod, and, or, xor */
						token ++; /* the first character in the argument will be $, so truncate it off */
						regTrgt = atoi(token);
						
						token = strtok(NULL, ",");
						token ++; /* the first character in the argument will be $ again, so truncate it off */
						regSrc0 = atoi(token);
						
						token = strtok(NULL, ",");
						token ++;
						regSrc1 = atoi(token);
						
						instruction |= regSrc0;
						instruction <<= 5;
						instruction |= regSrc1;
						instruction <<= 5;
						instruction |= regTrgt;
						instruction <<= 5;
					}
					else {
						if (opcode == 6 || opcode == 10){ /* for instructions move, com */
							token ++; /* the first character in the argument will be $, so truncate it off */
							regTrgt = atoi(token);
							
							token = strtok(NULL, ",");
							token ++; /* the first character in the argument will be $ again, so truncate it off */
							regSrc0 = atoi(token);
							
							instruction |= regSrc0;
							instruction <<= 10;
							instruction |= regTrgt;
							instruction <<= 5;
						}
						else {
							if (opcode > 10 && opcode < 14) { /* for instructions sll, srl, sra */
								token ++; /* the first character in the argument will be $, so truncate it off */
								regTrgt = atoi(token);
								
								token = strtok(NULL, ",");
								token ++; /* the first character in the argument will be $ again, so truncate it off */
								regSrc0 = atoi(token);
								
								token = strtok(NULL, ",");
								shiftAmt = atoi(token);
								
								instruction |= regSrc0;
								instruction <<= 10;
								instruction |= regTrgt;
								instruction <<= 5;
								instruction |= shiftAmt;
							}
							else { /* for instructions jr, rdr, prr, prh */
								token ++; /* the first character in the argument will be $, so truncate it off */
								regTrgt = atoi(token);
								
								instruction <<= 10;
								instruction |= regTrgt;
								instruction <<= 5;
							}
						}
					}
				}
				
				instruction <<= 6;
			}
			else {
				if (format == I) {
					if (opcode == 18) { /* for instruction li */
						token ++; /* the first character in the argument will be $, so truncate it off */
						regTrgt = atoi(token);
						
						token = strtok(NULL, ",");
						operand = atoi(token);
					}
					else {
						if (opcode > 18 && opcode < 24) { /* for instructions addi, subi, muli, divi, modi */
							token ++; /* the first character in the argument will be $, so truncate it off */
							regTrgt = atoi(token);
							
							token = strtok(NULL, ",");
							token ++;
							regSrc0 = atoi(token);
							
							token = strtok(NULL, ",");
							operand = atoi(token);
							operand &= 0xFFFF; // in order to restrict the immediate operand to the first 16 bits in case of negatives
							
							instruction |= regSrc0;
						}
						else { /* for instruction lwb, swb */
							token ++; /* the first character in the argument will be $, so truncate it off */
							regTrgt = atoi(token);
							
							token = strtok(NULL, ",");
							token = strtok(token, "(");
							operand = atoi(token);
							
							token = strtok(NULL, "(");
							token ++;
							token[strlen(token)] = '\0'; /* remove the last character in the argument which will be a close parenthesis */
							regSrc0 = atoi(token);
							
							instruction |= regSrc0;
							
						}
					}
					
					instruction <<= 5;
					instruction |= regTrgt;
					instruction <<= 16;
					instruction |= operand;
				}
				else { /* if format isn't R or I, it's J */
					if (opcode == 26 || opcode == 27) { /* for instructions lwa, swa */
						token ++; /* the first character in the argument will be $, so truncate it off */
						regTrgt = atoi(token);
						
						token = strtok(NULL, ",");
						
						instruction <<= 5;
						instruction |= regTrgt;
					}
					else {
						if (opcode == 28 || opcode == 29) {
							instruction <<= 5;
						}
						else {
							token ++; /* the first character in the argument will be $, so truncate it off */
							regTrgt = atoi(token);
							
							token = strtok(NULL, ",");
							token ++;
							regSrc0 = atoi(token);
							
							token = strtok(NULL, ",");
							
							instruction |= regSrc0;
							instruction <<= 5;
							instruction |= regTrgt;
						}
					}
					
					operand = findLC(symbolTable, token);
					
					if (operand == -1) { /* findOpcode returns -1 if it didn't find the symbol */
						addError(errors, undefinedSymbol, lineNum);
						continue;
					}
					
					instruction <<= 16;
					instruction |= operand;
				}
			}
			
			fprintf(objFile, "%9X\n", instruction);
			continue;
		}
		
		if (segment == data) { /* if in the data segment */
			LC ++;
			continue; /* no need to look at the data segment in pass two */
		}
	}
	
	if (*errors) {
		outFilename[rootNameLen] = '\0'; /* construct the name of the error file */
		strcat(outFilename, ".err");
		errFile = fopenErrCheck(outFilename, "w", "Failed to open the output error file");
		
		printErrors(errFile, errors); /* print all the errors */
		
		close(objFile);
		outFilename[rootNameLen] = '\0'; /* construct the name of the object file again */
		strcat(outFilename, ".obj");
		remove(outFilename); /* so that it can be deleted, since object file isn't useful if any errors were encountered */
		
		close(errFile);
	}
	else {
		outFilename[rootNameLen] = '\0'; /* construct the name of the symbol file */
		strcat(outFilename, ".sym");
		symFile = fopenErrCheck(outFilename, "w", "Failed to open the output symbol table file");

		printSymbolTable(symFile, symbolTable); /* print all the symbols */
		
		close(symFile);
		close(objFile);
	}
	
	free(outFilename);
	free(opcodeTable);
	free(symbolTable);
}