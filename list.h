/* Author: Marcus Ross
 *         MR867434
 */

#include "const.h"

typedef struct _symbolNode { /* linked list node */
	char* symbol;
	int LC;
	struct _symbolNode *next;
} symbolNode;

typedef struct _opcodeNode { /* linked list node */
	char* instruction;
	int opcode;
	struct _opcodeNode *next;
} opcodeNode;

typedef enum _errorType { /* possible values for each errorNode */
	undefinedSymbol,
	multiplyDefinedSymbol,
	illegalOp,
	dataSegment
} errorType;

typedef struct _errorNode { /* linked list node */
	errorType type;
	int line;
	struct _errorNode *next;
} errorNode;