/* client.c - code for example client program that uses TCP */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <stdio.h>
#include <string.h>



#define PROTOPORT 2020
/* default protocol port number */
extern int errno;
char localhost[] =  "localhost";
/* default host name*/
/*-------------------------------------------------------------
 * Program:
 client
 *
 * Purpose:
 allocate a socket, connect to a server, and print all output
 *
 * Syntax:
 client [ host [port] ]
 *
 *
 host - name of a computer on which server is executing
 *
 port - protocol port number server is using
 *
 * Note:
 Both arguments are optional. If no host name is specified,
 *
 the client uses "localhost"; if no protocol port is
 *
 specified, the client uses the default given by PROTOPORT.
 *
 *-----------------------------------------------------------*/


#include "clientface.h"
#include "handler.h"

int recvdelim(int sd,char * buff, int delim, int max);

int main(int argc, char *argv[]){

  

  /*
  for (i = 1; i < argc; i+=2){
    switch(argv[i][1]){
    case 'p':
      PORT = atoi(argv[i+1]);
      break;
    case 'h':
      LOCALHOST = atoi(argv[i+1]);
      break;
    case 'M':
      CURMAXPLAYA = atoi(argv[i+1]);
      break;
    case 't': 
      PLAY_TIME = atoi(argv[i+1]);
      break;
    case 'a':
      MAX_ATTEMPT = atoi(argv[i+1]);
      break;
    }
  }
  */

  
  struct hostent *ptrh; /* pointer to a host table entry
			 */
  struct protoent *ptrp; /* pointer to a protocol table entry
			  */
  struct sockaddr_in sad; /* structure to hold an IP address
			   */
  int sd; /* socket descriptor*/
  int port;/* protocol port number*/
  char  *host;/* pointer to host name*/
  int  n;/* number of characters read*/
  
  memset((char *)&sad,0,sizeof(sad)); /* clear sockaddre */
  sad.sin_family = AF_INET; /* set family to Internet */

  /* Check command-line argument for protocol port and extract*/

  /* port number if one is specified. Otherwise, use default */

  /* port value given by constant PROTOPORT */

  if (argc > 2) { 
    /* if protocol port specified */
    port = atoi(argv[2]); 
  } else {
    port = PROTOPORT;
  }
  
  if (port > 0){
    sad.sin_port = htons((u_short)port); 
  }

  
  
  /* Check host argument and assign host name. */
  if (argc > 1) {
    host = argv[1];
  } else {
    host = localhost;
  }
 
  initialize_builtins((argc > 3)?0:1);
  
  
  /* if host argument specified
   */
  /* Convert host name to equivalent IP address -> copy to sad. */
  
  ptrh = gethostbyname(host);
  if ( ((char *)ptrh) == NULL ) {
    fprintf(stderr,"invalid host: %s\n", host);
    exit(1);
  }
  memcpy(&sad.sin_addr, ptrh->h_addr, ptrh->h_length);
  /* Map TCP transport protocol name to protocol number. */
  if ( ((int)(ptrp = getprotobyname("tcp"))) == 0) {
    fprintf(stderr, "cannot map \"tcp\" to protocol number");
    exit(1);
  }
  /* Create a socket. */
  sd = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
  if (sd < 0) {
    fprintf(stderr, "socket creation failed\n");
    exit(1);
  }
  /* Connect the socket to the specified server. */
  if (connect(sd, (struct sockaddr *)&sad, sizeof(sad)) < 0) {
    fprintf(stderr,"connect failed\n");
    exit(1);
  }
  /*Repeatedly read data from socket -> write to user's screen. */
  

  char buff[2048];
  
  
  char** tokens;
  if (argc > 3)
    sprintf(buff+1024,"%s",argv[3]);
  else{
    printf("What do you want your name to be? \n\t:");
    
    gets(buff+1024);
  }
  n = sprintf(buff,"[join,%s]",buff+1024);
  
  sanatize(buff,n);
  argc = arg_parse(buff,&tokens);
  clientOp(tokens,argc,sd);
   
  int start,end;
  
  while (n > 0) {
    
    start = recvdelim(sd,buff,'[',sizeof(buff)-1);
    if (start < 0 ) break;
    end = recvdelim(sd,buff+start+1,']',sizeof(buff)-start-1);
    if (end < 0 ) break;
    
    sanatize(buff+start,end+2);
    
    if((argc = arg_parse(buff+start,&tokens))<0)
      fprintf(stderr,"NO TOKENS FOUND");
    else 
      clientOp(tokens,argc,sd);

  }
  /* Close the socket. */
  close(sd);
  /* Terminate the client program gracefully. */
  exit(0);
}



int recvdelim(int sd,char * buff, int delim, int max){
  
  int offs = 0;
  buff[offs] = '\0';
  
  int tmp = 0;
  struct timeval tv = {180,0};
  
  do {
    offs+= tmp;
    if ((tmp = recv(sd,buff+offs,sizeof(char),0))<0){
      perror("recvdelim error");
      return -1;
    }

  } while (offs < max-1 && buff[offs] != delim);
  buff[offs]=delim;
  
  return offs;
  
}
