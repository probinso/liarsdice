#include "player.h"

int null_player(player* p){
  p->ingame = inactive;
  sprintf(p->name,"No--Name");
  p->playerNumber = -1;
  p->attempts = -1;
  p->diceno = -1;
  return 1;
}

int start_player(player*p,char*name,int playerNum,player_state s){
  p->ingame = s;
  snprintf(p->name,NAME_SIZE,name);
  p->playerNumber = playerNum;
  p->attempts = MAX_ATTEMPT;
  p->diceno = MAX_DICE;
  return 1;
}

int set_player(player* p,int cup[]){
  int i;
  
  for (i = 0; i < MAX_DICE; i++){
    p->cup[i] = cup[i];
  }
  p->last_bid = -1;
  
  return 1;
  
}

int count_dice(player *p, int dice_val){
  int i;
  int s = 0;
  for (i = 0; i < p->diceno; i++)
    s += (p->cup[i] == dice_val)?1:0;
  
  return s;
}

int attempt_punish(player *p){
  if ((--(p->attempts)) > 0)
    return 1;
  
  return -1;
}

int player_loose(player*p){
  if (--(p->diceno) > 0)
    return 1;
  
  return -1;
}

int is_countable(player *p){
  if (p->ingame == dead ||
      p->ingame == active)
    return 1;
  
  return -1;
}

int set_bid(player* p, int dice_val){
  return (p->last_bid = dice_val);
}
