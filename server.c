#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

#include <errno.h>

#include <sys/time.h>
/* port we're listening on */

int PORT = 2020;

#define MAXX 1024
#include "types.h"

extern int CURMAXPLAYA;
extern int MIN_PLAYERS;
extern int COUNT_TIME;
extern int PLAY_TIME;
extern int MAX_ATTEMPT;


#include "interface.h"
#include "ssgame.h"
#include "handler.h"


void pexit(char * s){
  fprintf(stderr,"%s :: %d\n",s,errno);
  exit(1);
}


int local_locate_term(char* line){
  int i = -1;
  while (line[++i] != '\0')
    switch(line[i]){
    case '\\':
      i++;
      break;
    case ']':
      return i;
    }
  return -1;
}



int main(int qq, char** argv)/*(int argc, char *argv[])*/
{
  /*
    
    THIS IS WHERE WE WILL INITIALIZE SERVICES
  
  */
  
  CURMAXPLAYA=30;
  MIN_PLAYERS=3;
  COUNT_TIME =15;
  PLAY_TIME  =20;
  MAX_ATTEMPT=10;
  
  int i;  
  for (i = 1; i < qq; i+=2){
    switch(argv[i][1]){
    case 'p':
      PORT = atoi(argv[i+1]);
      break;
    case 'm':
      MIN_PLAYERS = atoi(argv[i+1]);
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
  /*
  fprintf(stderr,"The Max Number of Players is %d\n",CURMAXPLAYA);
  fprintf(stderr,"The Min Number of Players is %d\n",MIN_PLAYERS);
  fprintf(stderr,"Current Active Port is %d\n",PORT);
  fprintf(stderr,"The player timer is set to %d\n",PLAY_TIME);
  fprintf(stderr,"The Maximum Number of attempted moves is %d\n",MAX_ATTEMPT);
  exit(1);
  */
  initialize_game();
  initialize_builtins();
  
  
  extern fd_set master;
  extern fd_set read_fds;

  const char MESG[] = "Yo-Dog";
  
  struct sockaddr_in serveraddr; /* server address */
  
  struct sockaddr_in clientaddr; /* client address */

  /* master and temp sets for fds */
  /*  fd_set master, read_fds;*/
  FD_ZERO(&master);
  FD_ZERO(&read_fds);

  int listener; /* listening socket descripter */
  int newfd; /* newly accpeted sockete descriptors */
  int fdmax; /* maximal number of file descriptors */

  /* buffer information */
  char buffer[MAXX] = {'\0'};
  int nbytes = 0;

  /* magic intended for setsockopt(), SO_REUSEADDR */
  int yes = 1; /*  */
  int addrlen;
  

  int argc;
  char ** tokens;


  if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    pexit("Server-socket() error");
  
  if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) 
    pexit("Server-setsockopt() error");

  /*binding initialization*/
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = INADDR_ANY;
  serveraddr.sin_port = htons(PORT);
  memset(&(serveraddr.sin_zero),'\0',8);

  if(bind(listener, (struct sockaddr *)&serveraddr, sizeof(serveraddr))==-1)
    pexit("Server-bind() error");
  
  if(listen(listener,10)==-1)
    pexit("Server-listen() error");

  FD_SET(listener,&master); /*add the listener to the master set*/
  fdmax = listener; /*There is only one*/


  /* THIS IS OUR TIMMER OF WONDERMENT */
  struct timeval tv = {5,0};/*TIMMER ON PER MOVE BASIS.*/
  while(1){
    sprintf(buffer,"null");
    nbytes = 0;/*gives us compairable value for case issues*/
    
    
    read_fds = master;
    

    /*
      The select_mode integer helps to identify if select terminated on
        error   :: -1
        timeout ::  0
        success ::  1
    */
    int select_mode = select(fdmax+1,&read_fds, NULL, NULL, &tv);
    if(select_mode == -1) pexit("Server-select() loop-error");
    else if (select_mode == 0) {
      
      sprintf(buffer,"[empty]");
      argc = arg_parse(buffer,&tokens);
      serverOp(&tv,tokens,argc,fdmax,i);
      /*check_builtin(tokens,argc,300);*/
      
    }
    else{

      /*The select statement waits until the timmer is out then moves forth*/
      for(i = 0; i <= fdmax; i++){/* for all (socket) file descrptors */
	if(FD_ISSET(i,&read_fds)){/* The descriptor in question is being used */
	  /*fprintf(stderr,"swingin");*/
	  /*
	    If the socket is being used 
	  */
	  if (i == listener){/* handle new connections */
	    
	    addrlen = sizeof(clientaddr);
	    if((newfd = accept(listener,(struct sockaddr *)&clientaddr, &addrlen)) == -1){
	      /*What does this mean?*/
	      fprintf(stderr,"Server-accept() loop-error"); 
	      
	    }else{

	      FD_SET(newfd,&master); /* add filedescriptor to master list */
	      fdmax = (newfd > fdmax)?newfd:fdmax;
	      
	      printf("%s : New connection from %s on socked %d\n",
		     MESG,inet_ntoa(clientaddr.sin_addr),newfd);
	    }

	  }
	  else{ /* This means that data is being read from client that exists*/
	    
	    if((nbytes = recv(i,buffer, sizeof(buffer),0)) <=0){
	      
	      nbytes = sprintf(buffer,"[kick]\n");
	      sanatize(buffer,nbytes);
	      
	      argc = arg_parse(buffer,&tokens);
	      serverOp(&tv,tokens,argc,fdmax,i);
	      
	      
	      if (nbytes == 0)/* it is the client's falt*/
		printf("%s: socket %d hung up\n",MESG,i);
	      else /* it is the server's falt */
		fprintf(stderr,"recv() error\n");
	      
	      
	      close(i);
	      FD_CLR(i,&master);
	      
	    }
	    else {
	      
	      
	      nbytes = sanatize(buffer,nbytes);
	      
	      while (local_locate_term(buffer)<0){
		
		select_mode = select(fdmax+1,&read_fds, NULL, NULL, &tv);

		if (select_mode == 0){
		  nbytes = sprintf(buffer,"[empty]");
		  sanatize(buffer,nbytes);
		  break;
		}
		
		if (FD_ISSET(i,&read_fds)){
		  nbytes += recv(i,buffer+nbytes,sizeof(buffer)-nbytes,0);
		  sanatize(buffer,nbytes);
		}
		
	      }


	      /*
		If there is a tokenizeable message then serverOp is performed
	      */
	      
	      if ((argc = arg_parse(buffer,&tokens))<0) 
		fprintf(stderr,"\n\t\t\t++ NO TOKENS FOUND! ++\n\n");
	      else
		serverOp(&tv,tokens,argc,fdmax,i);
	      
	      /*check_builtin(tokens,argc,i);*/
	      
	      
	      
	    }
	  }
	}	
      }
    }
  }
  return 0;
}
