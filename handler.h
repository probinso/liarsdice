
#include "types.h"
#include <ctype.h>
#include <string.h>

int sanatize(char* line, int size);
int arg_parse(char* line, char *** argvp);



operation* insert_builtin(operation* messageOps, 
		    char* command, 
		    funcptr mycommand, 
		    funcptr broadcast, 
		    int acceptable_states);

operation * find_builtin(operation* node, char* command);
