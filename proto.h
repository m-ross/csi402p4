/* Author: Marcus Ross
 *         MR867434
 */

void assemble(FILE*, char*);
void buildOpcodeTable(opcodeNode**);
FILE* fopenErrCheck(char*, char*, char*);
void addSymbol(symbolNode**, char*, int, errorNode**, int);
symbolNode* newSymbol(char*, int);
void outputSymbolTable(FILE*, symbolNode**);
void addError(errorNode**, errorType, int);
errorNode* newError(errorType, int);
void outputErrors(FILE*, errorNode**);
int hash(char*, int);
void removeNewline(char*, int);
int removeInlineComment(char*, int);
int findOpcode(opcodeNode**, char*);
int findLC(symbolNode**, char*);