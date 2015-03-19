#pragma once

#include "types.h"
#include "player.h"

#include <string.h>

#include <unistd.h>


player *self; 

int dice[DIE_VAL];

int bid[2];

int CHALNO;

void initialize_player(void);

int sit_and_spin(char** argv,int argc, int sd);
int join_receipt(char** argv,int argc,int sd);
int get_dice(char** argv,int argc, int sd);
int player_bid(char** tokens, int argc, int fd);
int make_quit(char** tokens, int argc, int fd);
int player_bid(char** argv,int argc,int sd);
int game_end(char** argv,int argc,int sd);
int round_start(char** argv,int argc,int sd);


int send_join(char** argv,int argc,int sd);
int send_succ(char** argv,int argc,int sd);

int is_your_turn(char** argv,int argc, int sd);
int make_move(char** argv,int argc,int sd);
int i_make_move(char** argv,int argc,int sd);
