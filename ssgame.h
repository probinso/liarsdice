#pragma once 

#include <unistd.h>

#include "types.h"
#include "player.h"


/*
THESE ARE NESSACARY TYPES
*/

state MYSTATE;
int CURRENT_PLAYER;
int ACTIVE_PLAYERS;

extern int CURMAXPLAYA;
extern int MIN_PLAYERS;
extern int COUNT_TIME;
extern int PLAY_TIME;
extern int MAX_ATTEMPT;



fd_set master,read_fds;

player activePlayers[MAX_PLAYERS];

int *lastbid;
int bids[DIE_VAL];


int initialize_game(void);
/*
  Sets game to Empty and Null State
 */
int send_mesg(int mode, char* msg, int fd);
/*
  handles broadcast messages
 */
int p_send_mesg(int fd,char * format,char* msg);
/*
  handles private messages
 */
int continue_game(void);
/*
  updates current player
  sets broadcast player_turn
 */
int decrement_player(int fd);

int start_round(char** tokens, int argc, int fd);
int broadcast_start(char** tokens, int argc, int fd);

int add_player(char** tokens, int argc, int fd);
int broadcast_add(char** tokens, int argc, int fd);

int view_player(char** tokens, int argc, int fd);
int broadcast_view(char** tokens, int argc, int fd);

int bid_player(char** tokens, int argc, int fd);
int broadcast_bid(char** tokens, int argc, int fd);

int challenge_player(char** tokens, int argc, int fd);
int broadcast_challenge(char** tokens, int argc, int fd);

int player_quit(char** tokens, int argc, int fd);
int broadcast_quit(char** tokens, int argc, int fd);

int player_kick(char** tokens, int argc, int fd);
int broadcast_kick(char** tokens, int argc, int fd);

