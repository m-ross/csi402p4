/* Author: Marcus Ross
 *         MR867434
 */

#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "proto.h"

/* This function's arguments are a string naming a file to be opened, a string containing a mode as in fopen(), and a string containing a message to show in case the file fails to open. The file named by the first argument is opened in the mode specified by the second argument and the pointer to it is returned. If it fails to open, the third argument is printed to stdout and the program is killed. */
FILE* fopenErrCheck(char* filename, char* mode, char* errMsg) {
	FILE* file = fopen(filename, mode);
	
	if (!file) { /* kill program if failed to open file */
		perror(errMsg);
		exit(-1);
	}
	else {
		return file;
	}
}

/* This function's arguments are a double pointer to the head of a linked list of struct errorNode that, if non-empty, has its errors sorted by line number, an enum errorType specifying an error that occurred, and an int specifying the line number at which an error occurred. The error will be added to the list in such a position that the list will remain sorted by line number. */
void addError(errorNode** errors, errorType type, int line) {
	errorNode *node;
	errorNode *prev;
	errorNode *newNode;
	
	if (!*errors) { /* if the list is empty */
		*errors = newError(type, line); /* shove it in */
		return;
	}
	
	node = *errors; /* else begin at the head of the list */
	
	while (node->next) { /* while there's a node after this */
		if (line > node->line) { /* if the line of the new error is greater than the line of this node */
			prev = node; /* note the current node as previous */
			node = node->next; /* go to next node */
		}
		else { /* if the line of the new error is less than the line of this node */
			break; /* stop traversing */
		}
	}
	
	if (!node->next) { /* if at the end of the list */
		node->next = newError(type, line); /* put the new node there */
	}
	else { /* if not at the end of the list */
		newNode = newError(type, line); /* create the new node */
		
		if (prev) { /* if not at the beginning of the list */
			prev->next = newNode; /* put the new node between the prev and current node */
			newNode->next = node;
		}
		else { /* if at the beginning of the list */
			newNode->next = *errors; /* insert the new node at the head of the list */
			*errors = newNode;
		}
		
	}
}

/* This function's arguments are an enum errorType specifying an error that occurred and an int specifying the line number at which an error occurred. Memory for a struct errorNode will be allocated and the arguments are put in the node then the pointer to the node is returned. */
errorNode* newError(errorType type, int line) {
	errorNode *node = (errorNode *) malloc(sizeof(errorNode));
	node->type = type;
	node->line = line;
	return node;
}

/* This function's argument are a file pointer and the head of a linked list of struct errorNodes. It prints to the file a line containing all information on each element of the list. */
void printErrors(FILE* file, errorNode** error) {
	while (*error) {
		fprintf(file, "Error: line %i: %s\n", (*error)->line, (*error)->type == dataSegment ? "Each line in the data segment must have a label" : (*error)->type == multiplyDefinedSymbol ? "multiply defined symbol" : (*error)->type == undefinedSymbol ? "undefined symbol" : "illegal instruction");
		(*error) = (*error)->next;
	}
}