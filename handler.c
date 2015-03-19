/*#include <stdio.h>*/
#include "handler.h"

int tokenize(char* line, char *** store, int size);

int rem_null(char* orig, int o_size){
  /* Removes null characters */
  int dest = 0;
  int i = 0;
  
  for (i = 0; i < o_size; i++)
    if (*(orig+i)!='\0')
      orig[dest++]=orig[i];
  
  orig[dest]='\0';
  return dest;
}

int eat_spaces(char *orig, int o_size){
  /* produces string with only single space white space */
  /* as side effect removes trailing characters that aren't printable */
  int dest = -1; 
  int i = -1; 
  
  
  while (i < o_size){
    while(isspace((int)orig[++i]));
    i--;/* eat all leading spaces  */
    
    while(isgraph((int) (orig[++dest] = orig[++i])));
    /* preserve graphable characters */

    if (orig[i] != '\0')
      orig[dest] = ' ';
    /*  standardize spaces */
    
  }
  orig[dest]='\0';
  return dest;
}

int sanatize(char *line, int size){
  
  size = rem_null(line,size);

  int i; 

  
  for (i = 0; i <= size; i++)
    line[i] = (char)tolower((int)line[i]);

  size = eat_spaces(line,size);
  
  line[size]='\0';
  return size;

}

int arg_parse(char *line, char ***argvp){
  return (*line == '[')?tokenize(line+1, argvp, 0):-1;
}
  

int locate_term(char* line){
  int i = -1;
  while (line[++i] != '\0')
    switch(line[i]){
    case '\\':
      i++;
      break;
    case ',':
    case ']':
      return i;
    }
  return -1;
}

int tokenize(char* line, char *** store, int size){

  int termloc = locate_term(line);
  int retval;
  
  if (termloc == -1) return -1;

  int i = -1;
  while(isspace(line[++i]));  

  if (*(line+termloc) == ']'){
    *(line+termloc)='\0';
    *store = (char **)malloc((size+1)*sizeof(char*));
    (*store)[size] = line+i;
    return size+1;
  }
  else{
    *(line+termloc)='\0';
    retval = tokenize(line+termloc+1,store, size+1);
    (*store)[size] = line +i;
    return retval;
  }
}


operation* insert_builtin(operation* messageOps, 
		    char* command,
		    funcptr mycommand, 
		    funcptr broadcast, 
		    int acceptable_states)
{
  operation* tmp = (operation*)malloc(sizeof(operation));
  
  tmp->next = messageOps;
  tmp->command = command;
  tmp->use_states = acceptable_states;
  tmp->mycommand = mycommand;
  tmp->broadcast = broadcast;

  messageOps = tmp;
  return messageOps;
}


operation * find_builtin(operation* node, char* command){
  if (node == NULL)
    return NULL;
  else if (strcmp(command,node->command)==0)
    return node;
  else 
    return find_builtin(node->next,command);
}
