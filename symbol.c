/* Author: Marcus Ross
 *         MR867434
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "proto.h"
#include "const.h"

/* This function's first two arguments include a pointer to a hash table of struct symbolNodes (a symbol table), and a string containing a label, and are for adding a symbol to the table. The next three arguments include an int containing an LC, the head of a linked list of struct errorNodes, and an int containing a line number, and are for creating an error in case adding the symbol fails. */
void addSymbol(symbolNode** symbolTable, char* label, int LC, errorNode** errors, int line) {
	symbolNode *symbolNode; /* temp holder */
	int iHash = hash(label, SYMBOL_HASH_SIZE); /* hash the label */
	
	if (!symbolTable[iHash]) { /* if there is no node at that index */
		symbolTable[iHash] = newSymbol(label, LC); /* put a new node at that index */
	}
	else { /* if there is a node there */
		symbolNode = symbolTable[iHash]; /* note the address of the list header, for traversing the list */
		
		if (strcmp(symbolNode->symbol, label) == 0) { /* if the label matches any node */
			addError(errors, multiplyDefinedSymbol, line); /* then there's a multiply defined symbol error */
			return;
		}
		
		while (symbolNode->next) { /* traverse to the end of the list, to put the new symbol there */
			symbolNode = symbolNode->next;
			
			if (strcmp(symbolNode->symbol, label) == 0) { /* again, stop and add an error if symbol is already in the table */
				addError(errors, multiplyDefinedSymbol, line);
				return;
			}
		}
		
		symbolNode->next = newSymbol(label, LC); /* put the new node behind the end of the list */
	}
}

/* This function's arguments are a string containing a label and an LC as an int. A struct symbolNode is created using the argument and a pointer to it is returned. */
symbolNode* newSymbol(char* label, int LC) {
	symbolNode *node = (symbolNode *) malloc(sizeof(symbolNode)); /* alloc for the node */
	node->symbol = (char *) malloc(sizeof(char) * strlen(label)); /* alloc for the node's symbol */
	strcpy(node->symbol, label); /* copy the argument into the node */
	node->LC = LC; /* put the other argument into the node */
	return node; /* return the pointer */
}

/* This function's arguments are a file, and a pointer to a hash table of struct symbolNodes. For each element in the table, the function prints a line with the node's symbol and LC. */
void printSymbolTable(FILE* file, symbolNode** symbolTable) {
	int i; /* for loop counter */
	
	for (i = 0; i < SYMBOL_HASH_SIZE; i++) { /* for each index in the hash table */
		while (symbolTable[i]) { /* if a node exists at that index */
			fprintf(file, "%-8s %5i\n", symbolTable[i]->symbol, symbolTable[i]->LC); /* print the node's info */
			symbolTable[i] = symbolTable[i]->next; /* then repeat for next node */
		}
	}
}

/* This function's arguments are a pointer to a hash table of struct symbolNodes and a symbol as a string. It searches the table for the symbol and returns the LC of that symbol as an int, or it returns -1 if the symbol is not in the table. */
int findLC(symbolNode** symbolTable, char* symbol) {
	int LC = -1;
	int iHash;
	symbolNode *node;
	
	iHash = hash(symbol, SYMBOL_HASH_SIZE); /* hash the symbol */
	node = symbolTable[iHash]; /* get the node at the head of the list of that index */
	
	while (node) { /* as long as we're not looking at an empty node */
		if (strcmp(node->symbol, symbol) == 0) { /* if the argument matches this node's symbol */
			LC = node->LC; /* the LC we're looking for is this node's LC */
			break; /* so stop looking */
		}
		
		node = node->next; /* repeat for next node */
	}
	
	return LC;
}