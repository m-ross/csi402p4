/* Author: Marcus Ross
 *         MR867434
 */

#include <string.h>

void removeNewline(char* string, int length) {
	if (string[length - 1] == '\n') { /* if the string ends with a newline */
		string[length - 1] = '\0'; /* truncate it off */
	}
}

int removeInlineComment(char* string, int length) {
	int i;
	
	for (i = length; i >= 0; i--) { /* start from end of string and traverse towards the start */
		if (string[i] == '#') { /* if the string has a comment */
			string[i] = '\0'; /* truncate the comment off */
			length = strlen(string); /* get the new length */
		}
	}
	
	return length;
}