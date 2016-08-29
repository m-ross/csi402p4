/* Author: Marcus Ross
 *         MR867434
 */

#include <stdlib.h>
#include "list.h"
#include "const.h"

/* This function's arguments are a pointer to a hash table of struct opcodeNodes of size specified by the constant OPCODE_HASH_SIZE. It populates the hash table with instructions specified contained in the array specified by the constant INSTRUCTIONS. */
void buildOpcodeTable(opcodeNode** opcodeTable) {
	opcodeNode *node;
	char *instruction[NUM_INSTR] = INSTRUCTIONS; /* INSTRUCTIONS is an constant array holding all valid operations */
	int iHash;
	int i; /* for loop counter */
	
	for (i = 0; i < NUM_INSTR; i++) { /* for each element of the instruction array */
		node = (opcodeNode *) malloc(sizeof(opcodeNode));
		iHash = hash(instruction[i], OPCODE_HASH_SIZE); /* get the hash of the instruction */
		node->opcode = i; /* the instructions are ordered by opcode, ascending, so opcode = i */
		node->instruction = instruction[i];
		node->next = opcodeTable[iHash]; /* insert new node at the start of the list */
		opcodeTable[iHash] = node;
	}
}

/* This function's arguments are a pointer to a hash table of struct opcodeNodes, and a string containing an instruction. The hash table is searched for the instruction and the opcode of that instruction is returned as an int if it's found. If it's not found, -1 is returned. */
int findOpcode(opcodeNode** opcodeTable, char* instruction) {
	int opcode = -1;
	int iHash;
	opcodeNode *node;
	
	iHash = hash(instruction, OPCODE_HASH_SIZE); /* get the symbol's hash */
	node = opcodeTable[iHash]; /* get the head of the list at that index */
	
	while (node) { /* until the end of the list is reached */
		if (strcmp(node->instruction, instruction) == 0) { /* if this node matches the argument */
			opcode = node->opcode; /* this is the right node, so take the opcode */
			break;
		}
		
		node = node->next; /* repeat with next node */
	}
	
	return opcode;
}