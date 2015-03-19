#include "ccgame.h"

int to_server(int fd,char* str){
  fprintf(stderr,"<-- %s\n",str);
  dprintf(fd,str);
}

void initialize_player(void){
  
  self = (player*)malloc(sizeof(player));
  
  null_player(self);
  
  int i;
  for ( i = 0; i < DIE_VAL; i++)
    dice[i] = -1;
  
}

int send_join(char**argv,int argc, int fd){ 
  char buff[BUFFER_SIZE];
  int n = sprintf(buff,"[join,%s]",argv[1]);
  
  self->ingame = dead;
  
  return to_server(fd,buff);
  
}

int send_succ(char** argv, int argc, int fd){
  return (self->ingame = view);
}

int game_end(char** argv, int argc, int fd){
  return make_quit(argv,argc,fd);
}

/* sit and spin */

int join_receipt(char** argv,int argc,int sd){
  
  if (argc < 3) return -1;

  
  self->ingame = active;
  
  self->playerNumber = atoi(argv[2]);

  return 1;
}
/* sit and spin */

 
int is_your_turn(char** argv,int argc, int sd){
  if (argc != 2) return -1;
  if (self->playerNumber != atoi(argv[1])) 
    return -2;

  return 1;
}

int sit_and_spin(char** argv,int argc, int sd){
  return 1;
}

int round_start(char** argv,int argc,int sd){
    if (argc <3) return -1;
    if (self->ingame != active)
      self->ingame = view;
    
    return 1;
  }
  
int make_move(char** argv,int argc,int sd){
  
  char buff[BUFFER_SIZE];
  
  int i = random() % CHALNO--;
  
  
  int n = sprintf(buff,"[bid");
  
  
  if (bid[0] == -1)
    sprintf(buff+n,",%d,%d]",(random() % 15) + 1,self->cup[0]);
  else if (i){
    sprintf(buff+n,",%d,%d]",bid[0]+2,
	    bid[1]);
  }
  else
    sprintf(buff,"[challenge]");
  
  to_server(sd,buff);
  
  return 1;
   
}
  
int i_make_move(char** argv,int argc,int sd){
  
  char buff[BUFFER_SIZE];
  int dice_no;
  int dice_val;
 back:
  fprintf(stdout,"Would you like to [b]id or [c]hallenge?\n\t:");
  fscanf(stdin,"%s",&buff);
  switch(buff[0]){
  case 'b':
    fprintf(stdout,"Of what die would you like to bid\n\t:");
    fscanf(stdin,"%d",&dice_val);
    fprintf(stdout,"How many [%d]'s would you like to bid\n\t:",dice_val);
    fscanf(stdin,"%d",&dice_no);
    sprintf(buff,"[bid,%d,%d]",dice_no,dice_val);
    break;
  case 'c':
    fprintf(stdout,"you have selected to challenge\n");
    sprintf(buff,"[challenge]");
    break;
  default:
    fprintf(stdout,"I am sorry that is not an option\n");
    goto back;
  }
  
  to_server(sd,buff);
  
  return 1;
   
}

int get_dice(char** argv,int argc, int sd){
  if (argc < 2) return -1;
  
  CHALNO = 10;
  
  bid[0] = -1;
  bid[1] = -1;
  
  self->diceno = atoi(argv[1]);

  int i;

  for (i = 2; i < self->diceno; i++)
    self->cup[i-2] = atoi(argv[i]);
  
  self->ingame = active;
  
  return 1;
} 

int player_bid(char** tokens, int argc, int fd){
  if (argc != 4) return -1;
  
  bid[0] = atoi(tokens[2]);/*the number of dice*/
  bid[1] = atoi(tokens[3]);/*the value of the dice*/
  return 1;
}

int make_quit(char** tokens, int argc, int fd){
  fprintf(stderr,"\n :: Goodbye \n");
  close(fd);
  exit(1);
}
