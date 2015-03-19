#include "ssgame.h"

int nextActive(void);
int remove_player(int playerNum, player_state s);

int send_mesg(int mode, 
	      char* msg, 
	      int fd){
  int i = 0;
  fprintf(stderr,"<-- %s\n",msg);
  for (i = 0; i < MAX_PLAYERS; i++)
    if ((activePlayers[i].ingame ^ mode) < mode)
      dprintf(activePlayers[i].playerNumber,msg);
  return 1;
}

int p_send_mesg(int fd,char * format,char* msg){
  char buff[BUFFER_SIZE];
  sprintf(buff,format,msg);
  fprintf(stderr,"<-%d %s\n",fd,msg);
  return dprintf(fd,buff);
}

player* getPlayer(int fd);

player* getPlayer(int fd){
  return activePlayers+(fd % MAX_PLAYERS);
}

int initialize_game(){
  
  int i; 
  
  MYSTATE = lobby;
  
  for(i = 0; i < MAX_PLAYERS; i++)
    null_player(activePlayers+i);
  
  CURRENT_PLAYER = -1;
  ACTIVE_PLAYERS = 0;
  
  
  fprintf(stderr,"\t\t\t+++ GAME IS IN WAITING STATE +++\n");
  
  return 1;
}

/*
   THIS IS START SEQUENCE
 */

int start_round(char **tokens,int argc,int fd)
{  

  if (ACTIVE_PLAYERS < MIN_PLAYERS) {
    MYSTATE = lobby;
    return -1;
  }
  return round_operations(tokens,argc,fd);
}

int new_round(char **tokens,int argc,int fd){
  /*
    The fd for this call must be the previous winner
  */
  round_operations(tokens,argc,fd);
  
  fprintf(stderr,"\t\t\t ROUND STARTS WITH %d PLAYERS\n",ACTIVE_PLAYERS);
  
  return broadcast_start(tokens,argc,fd);
}    

int round_operations(char **tokens,int argc,int fd){
  MYSTATE = in_game;
  
  int cup[MAX_DICE];
  
  int i,j;
  
  for(i = 0; i <DIE_VAL; i++)
    bids[i] = -1;

  srandom((int)lastbid);
  lastbid = bids;

  for(i = 0; i < MAX_PLAYERS; i++){
    for(j = 0; j < MAX_DICE; j++)
      cup[j] = (random() % DIE_VAL)+1;
    
    
    set_player(activePlayers+i,cup);

    /*
      formerly active persons become inactive
      they must rejoin to become viewers
    */
    
    
    switch(activePlayers[i].ingame){
    case dead:
      activePlayers[i].ingame = inactive;
      break;
    case active:
      
      if(!(activePlayers[i].attempts > 0 &&
	   activePlayers[i].diceno > 0))
	remove_player(i,view);
      
      break;
    default:
      break;
    }
    
  }
  
  CURRENT_PLAYER = -1;
  /*  return continue_game();*/
  return 1;
}

int broadcast_start(char** tokens, int argc, int fd){
  int i,j;
  int n = 0;
  
  char buff[BUFFER_SIZE];

  for (i = 0; i < MAX_PLAYERS; i++)
    if (activePlayers[i].ingame == active){
      /*	
	 [your_dice,dice_no,x,x,x,x,x]-> 1
       */
      n = 0;
      n += sprintf(buff+n,"[your_dice,%d",activePlayers[i].diceno);
      for (j = 0; j < activePlayers[i].diceno; j++)
	n += sprintf(buff+n,",%d",activePlayers[i].cup[j]);
      
      n+= sprintf(buff +n, "]");
      
      p_send_mesg(activePlayers[i].playerNumber,"%s",buff);
    }
    
  /*
      [round_start,active_players,y,y,y,y,y] -> n
   */
  n = 0;
  n += sprintf(buff + n,"[round_start,%d",ACTIVE_PLAYERS);
  
  for (i = 0; i < MAX_PLAYERS; i++)
    if (activePlayers[i].ingame == active)
      n+= sprintf(buff + n,",%d,%d",i,activePlayers[i].diceno);
  
  n+= sprintf(buff +n, "]");

  send_mesg(active+view,buff,-1);

  /*sleep(5);*/

  CURRENT_PLAYER = -1;
  return continue_game();/*continue_game();*/
}


int continue_game(void){
  
  if (MYSTATE != in_game) return -1;
  /*fprintf(stderr,"CONTINTUE GAME\n");*/
  
  char buff[BUFFER_SIZE];
  
  CURRENT_PLAYER = nextActive();

  if (ACTIVE_PLAYERS <=1 || CURRENT_PLAYER == -1){
    sprintf(buff,"[game_end,%d]",CURRENT_PLAYER);
    send_mesg(active+view,buff,
	      (CURRENT_PLAYER != -1
	       )?activePlayers[CURRENT_PLAYER].playerNumber:-1);
    
    return initialize_game();

  }
  
  
    
  sprintf(buff,"[player_turn,%d]",CURRENT_PLAYER);
  
  send_mesg(active+view,buff,-1);
  /*fprintf(stderr,"FIN_CONTINUE_GAME");*/
  return 1;
}


/**/
/* --------------------------------------------------------- */
int nextActive(void){
  int i = 0;

  for(i = 1; i < MAX_PLAYERS; i++)
    if (activePlayers[(CURRENT_PLAYER + i) % MAX_PLAYERS].ingame == active)
      return ((CURRENT_PLAYER + i) % MAX_PLAYERS);
  
  return -1;
}


int prevActive(int playerNumber){
  
  player* p = getPlayer(playerNumber);
  
  while (--p >= activePlayers)
    if (p->ingame == active) return p->playerNumber;

  p+=MAX_PLAYERS;
  while (--p > activePlayers)
    if (p->ingame == active) return p->playerNumber;
  
  return -1;
}


int view_player(char** tokens, int argc, int fd ){

  player* p = getPlayer(fd);
  /*
  if (p->ingame != inactive)
    return -1;
  */
  if (argc != 2) return -2;

  start_player(p,tokens[1],fd,view);
  
  return 1;
}

int broadcast_view(char** tokens, int argc, int fd ){
  p_send_mesg(fd,"%s","[view_token]");
  return 1;
}
		       
int add_player(char** tokens, int argc, int fd ){
  /*
    COMPLETE
  */
  
  if (ACTIVE_PLAYERS >= CURMAXPLAYA)
    return -2;
    
  
  player* p = getPlayer(fd);
  if (p->ingame == active)
    return -1;
  
  if (argc != 2) return -2;
  
  start_player(p,tokens[1],fd,active);
  ACTIVE_PLAYERS++;
  
  if (ACTIVE_PLAYERS >= MIN_PLAYERS)
    MYSTATE = count_down;

  return 1;
  
}

int broadcast_add(char** tokens, int argc, int fd){
  /*
    COMPLETE
  */
  int i;
  int n = 0;
  
  char buff[BUFFER_SIZE];
  
  /*
    [state,the_states,fd,active_players,x,y,x,y,x,y]->1
   */
  n = 0;
  n += sprintf(buff+n,
     "[state,%s,%d,%d",
      (MYSTATE == lobby)?"lobby":(MYSTATE == in_game)?"in_game":"lobby_with_timer",
      fd % MAX_PLAYERS,ACTIVE_PLAYERS);
  
  
  for(i = 0; i < MAX_PLAYERS; i++)
    if (activePlayers[i].ingame == active)
      n+= sprintf(buff+n,
		  ",%s,%d",
		  activePlayers[i].name,
		  i);
  n += sprintf(buff+n,"]");
  
  p_send_mesg(fd,"%s",buff);

  /*
    [client_joined,name,#]->(n-1)
  */
  n = 0;
  n += sprintf(buff+n,
	       "[client_joined,%s,%d]",
	       activePlayers[fd % MAX_PLAYERS].name,
	       fd % MAX_PLAYERS);
  
  send_mesg(active+view,buff,fd);
  
  return 1;    
}


int remove_player(int fd, player_state s){
  
  player* p = getPlayer(fd);
  
  p->ingame = s;

  if (s == inactive){

    close(p->playerNumber);
    FD_CLR(p->playerNumber, &master);
  }
  ACTIVE_PLAYERS--;
  
  /*fprintf(stderr,"PLAYER REMOVED");*/
  return 1;
}

int player_quit(char** tokens, int argc, int fd){

  player* p = getPlayer(fd);
  
  
  if (p->ingame != active &&
      p->ingame != view
      ) return -1;

  
  if (argc != 1) return -2;
  
  
  
  return remove_player(fd,inactive);
  
}

int broadcast_quit(char** tokens, int argc, int fd){
  /*
    COMPLETE
   */
  
  char buff[BUFFER_SIZE];
  
  sprintf(buff,"[client_quit,%d]",fd);
  
  send_mesg(active+view,buff,fd);
  
  return 1;
}


int player_kick(char** tokens, int argc, int fd){
  /*
    COMPLETE
   */
  
  player *p = getPlayer(fd);
  
  if (p->ingame != active) return -1;
  
  if (argc != 1) return -2;
  
  return remove_player(fd,dead);
  
}

int broadcast_kick(char** tokens, int argc, int fd){
  /*
    COMPLETE
   */
  char buff[BUFFER_SIZE];
  sprintf(buff,"[client_kicked,%d]",fd);
  send_mesg(active+view,buff,fd);
  
  return continue_game();
}


int challenge_player(char** tokens, int argc, int fd){
  
  player* p = getPlayer(fd);
  
  if (p->ingame != active) return -1;
  if (argc != 1) return -2;
  
  if (*lastbid == -1) return -3;
  /*
    
   */
  
  return 1;
  
}

int broadcast_challenge(char** tokens, int argc, int fd){
  
  
  char buff[1024];
  
  sprintf(buff,"[challenge_report,%d]",fd);
  
  send_mesg(active+view,buff,-1);
  

  int challenged_val = (int)(lastbid - bids + 1);
  
  int number_of_val = *lastbid;
  
  int i,j;
  int s = 0;
  j = 0;
  for (i = 0; i < MAX_PLAYERS; i++)
    if (activePlayers[i].ingame == active ||
	activePlayers[i].ingame == dead   
	){
      j++;/*for now this denotes number of players in the thing*/
      s += count_dice(activePlayers+i,challenged_val);
    }
  
  int loser = (s < number_of_val)?fd:prevActive(fd);
  
  
  int n = 0;
  n += sprintf(buff,"[round_end,%d,%d",loser,j);
  /*[round_end,
     loser_number,
     number_of_players,
     p_i,sizeof(diceof(p_i)),diceof(p_i)
     ]
  */
  
  for (i = 0; i < MAX_PLAYERS; i++)
    if (activePlayers[i].ingame == active ||
	activePlayers[i].ingame == dead
	){
      n+= sprintf(buff+n,",%d,%d",
		  activePlayers[i].playerNumber,
		  activePlayers[i].diceno
		  );
      for (j = 0; j < activePlayers[i].diceno; j++)
	n+= sprintf(buff+n,",%d",activePlayers[i].cup[j]);
    }
  
  n += sprintf(buff+n,"]");
  send_mesg(active+view,buff,-1);
  
  n = 0; 
  
  
  activePlayers[loser].diceno--;
  
  
  return new_round(tokens,argc,-1);
  
}


int bid_player(char** tokens, int argc, int fd){
  
  player* p = getPlayer(fd);
  
  if (p->ingame != active) return -1;
  if (argc != 3) return -2;
  
  
  int dice_count = atoi(tokens[1]);
  int dice_val = atoi(tokens[2]);
  /*
  fprintf(stderr,
	  "\t\t\t%d is bidding %d [%d] \n",
	  fd,
	  dice_count,
	  dice_val);
  */
  
  if (dice_val > DIE_VAL || 
      dice_val  < 1 ||
      /* dice_count > DIE_VAL*ACTIVE_PLAYERS || */
      dice_count <= bids[dice_val-1])
    {
      fprintf(stderr,"\t\t\tINNAPROPRIATE BID %d %d\n",dice_count, dice_val);
      return -2;
    }
  /*
    -2 results in decrement_player
   */
  
  bids[dice_val-1] = dice_count;
  lastbid = bids + dice_val-1;
  
  
  return 1;
}


int broadcast_bid(char** tokens,int argc, int fd){
  
  /* must store previous bids... */
  
  
  int dice_count = atoi(tokens[1]);
  int dice_val = atoi(tokens[2]);

  
  char buff[BUFFER_SIZE];
  
  sprintf(buff,
	  "[bid_report,%d,%d,%d]",
	  fd,
	  dice_count,
	  dice_val);
  
  send_mesg(active+view,buff,fd);
  
  return continue_game();
}


int decrement_player(int fd){
  
  /*
    There is something big missing here.
   */
  player* p = getPlayer(fd);
  char buff[BUFFER_SIZE];
  
  if (p->ingame != active) return -1;
  
  fprintf(stderr,"\t\t\tdecrementing %s \n",p->name);
  
  if (attempt_punish(p)){
    
    sprintf(buff,"[invalid_move,%d]",p->attempts);
    p_send_mesg(fd,"%s",buff);
    
    return 1;
  }
    
  else {
    remove_player(fd,dead);
    return continue_game();    
  }
  
  return 1;
}

