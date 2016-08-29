/* Author: Marcus Ross
 *         MR867434
 */

#define OPCODE_HASH_SIZE 23 /* elements in the opcode hash table */
#define SYMBOL_HASH_SIZE 7 /* elements in the symbol hash table */
#define MAX_LINE_LEN 81 /* maximum length of a line in the source code including terminator */
#define MAX_LABEL_LEN 8 /* maximum length of a symbol */
#define NUM_INSTR 36 /* the number of valid instruction (in the following constant */
#define INSTRUCTIONS {"hlt", "add", "sub", "mul", "div", "mod", "move", "and", "or", "xor", "com", "sll", "srl", "sra", "jr", "rdr", "prr", "prh", "li", "addi", "subi", "muli", "divi", "modi", "lwb", "swb", "lwa", "swa", "j", "jal", "jeq", "jne", "jlt", "jle", "jgt", "jge"} /* all valid instructions */