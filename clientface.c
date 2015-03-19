#include "clientface.h"
#include "ccgame.h"

int clientOp(char** argv,int argc,int fd){
  
  char buff[BUFFER_SIZE];
  int n = 0;
  int i = 0;

  for (i = 0; i < argc; i++)
    n+=sprintf(buff+n," :%s",argv[i]);
  fprintf(stderr,"\t-->%s\n",buff);

  operation * execute;
    
  if ((execute = find_builtin(messageOps,argv[0]))
      == NULL)
    return -1;

  
  
  if (1/*(execute->use_states ^ self->ingame) 
	< execute->use_states*/){
    /*
      
      use_state xor-ed with MYSTATE is < use_state
      then the use_state spans the current state
      
    */
    
    
    if (execute->mycommand(argv,argc,fd)<0)
      return 1;

    return execute->broadcast(argv,argc,fd);

  }
    
  return -2;
}

void initialize_builtins(int QQ){
  messageOps = NULL;
  
  int tmp = 8+4+2+1;
  
  initialize_player();
  


  messageOps = 
    insert_builtin(messageOps,
		   "player_turn",
		   &is_your_turn,
		   (QQ==1)?&i_make_move:&make_move,
		   tmp
		   );
  messageOps = 
    insert_builtin(messageOps,
		   "state",
		   &join_receipt,
		   &sit_and_spin,
		   tmp
		   );


  messageOps = 
    insert_builtin(
		   messageOps,
		   "bid_report",
		   &player_bid,
		   &sit_and_spin,
		   tmp
		   );
  
  messageOps = 
    insert_builtin(
		   messageOps,
		   "your_dice",
		   &get_dice,
		   &sit_and_spin,
		   tmp
		   );
  messageOps = 
    insert_builtin(
		   messageOps,
		   "game_end",
		   &game_end,
		   &sit_and_spin,
		   tmp
		   );
  
  messageOps = 
    insert_builtin(
		   messageOps,
		   "state",
		   &join_receipt,
		   &sit_and_spin,
		   tmp
		   );
  
  messageOps = 
    insert_builtin(
		   messageOps,
		   "join",
		   &send_join,
		   &send_succ,
		   tmp
		   );

  messageOps = 
    insert_builtin(
		   messageOps,
		   "game_end",
		   &make_quit,
		   &sit_and_spin,
		   tmp
		   );
  
  messageOps = 
    insert_builtin(
		   messageOps,
		   "round_start",
		   &round_start,
		   &sit_and_spin,
		   tmp
		   );
  
  
  
}
