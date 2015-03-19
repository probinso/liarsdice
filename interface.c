#include "types.h"
#include "ssgame.h"
#include "interface.h"

#include <stdio.h>

int serverOp(struct timeval* tv, char** argv, int argc, int maxfd, int fd){   
  char buff[BUFFER_SIZE];

  int n=0;
  int i;
  

  operation* execute;

  /* Identify Message of Sending Player */
  if (strcmp(argv[0],"empty")==0 && MYSTATE == count_down)
    {
      argv[0] = "start";
    }
  else if (strcmp(argv[0],"join")==0 && MYSTATE == in_game){
    argv[0] = "view";
  }
  else if (strcmp(argv[0],"empty")==0 && MYSTATE == in_game){
    argv[0] = "kick\0";
    argc = 1;
    fd = activePlayers[CURRENT_PLAYER].playerNumber;
    
  }

  for (i = 0; i < argc; i++)
    n+=sprintf(buff+n," :%s",argv[i]);
  fprintf(stderr,"\t%d->%s\n",fd,buff);  

  /* Identify the requested Operation */
  if ((execute = find_builtin(messageOps,argv[0]))==NULL){
    if (MYSTATE == lobby)
      tv->tv_sec = DEFAULT_T;
    return -1;
  }

  /* QUESTIONS
     
     1) do we decrement players who attempt to play out of turn?
     2) do we decrement players who send gibberish out of turn?
     
  */
  
  if ((execute->use_states ^ MYSTATE) < execute->use_states){
    /*
      if the operation's use_state xor-ed with MYSTATE is < use_state
      then the use_state spans the current state
    */
    
    if (MYSTATE != in_game){
      /* The lobby and count_down allow for un-checked operations*/
      state temp = MYSTATE;
      
      if (execute->mycommand(argv,argc,fd)<0)
	return   p_send_mesg(fd,"[invalid_move,%s]",execute->command);
      /* missing broadcast behavior */
      
      execute->broadcast(argv,argc,fd);

      tv->tv_sec = (temp == lobby && MYSTATE == count_down)?COUNT_TIME:PLAY_TIME;
      if (tv->tv_sec == COUNT_TIME)
	{
	  sprintf(buff,"[timer_start,%d]\n",COUNT_TIME);
	  send_mesg(active+view,buff,-1);
	}
      
      return 1;
    }    
    else if (CURRENT_PLAYER == (fd % MAX_PLAYERS) || 
	     0==strcmp(argv[0],"view")){
      tv->tv_sec = PLAY_TIME;
      
      if (execute->mycommand(argv,argc,fd)==-2) return decrement_player(fd);

      return execute->broadcast(argv,argc,fd);
      
    } 
    
    tv->tv_sec = PLAY_TIME;
  }
				      
  return -1;
  
}


void initialize_builtins(){
  messageOps = NULL;

  messageOps = 
    insert_builtin(NULL,
		   "join",
		   &add_player,
		   &broadcast_add,
		   lobby+count_down
		   );
  
  messageOps = 
    insert_builtin(messageOps,
		   "challenge",
		   &challenge_player, 
		   &broadcast_challenge,
		   in_game
		   );
  
  messageOps = 
    insert_builtin(messageOps,
		   "quit",
		   &player_quit,
		   &broadcast_quit,
		   lobby+count_down
		   );
  
  messageOps = 
    insert_builtin(messageOps,
		   "kick",
		   &player_kick,
		   &broadcast_kick,
		   in_game+lobby+count_down
		   );

  messageOps = 
    insert_builtin(messageOps,
		   "start",
		   &start_round,
		   &broadcast_start,
		   count_down+in_game
		   );
  
  messageOps = 
    insert_builtin(messageOps,
		   "bid",
		   &bid_player,
		   &broadcast_bid,
		   in_game
		   );
  
  messageOps = 
    insert_builtin(messageOps,
		   "view",
		   &view_player,
		   &broadcast_view,
		   in_game+lobby+count_down
		   );
  
}
