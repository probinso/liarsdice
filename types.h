#pragma once

#define MAX_PLAYERS 30
int CURMAXPLAYA;
int MIN_PLAYERS;

int COUNT_TIME;
int PLAY_TIME;
#define DEFAULT_T   5


int MAX_ATTEMPT;

#define MAX_DICE 6
#define DIE_VAL 6

#define BUFFER_SIZE 1024
#define NAME_SIZE 40

#include <stdlib.h>

typedef int (*funcptr)(char**,int,int);

typedef struct inMessageList operation;
struct inMessageList{
  char* command;
  int use_states;
  funcptr mycommand;
  funcptr broadcast;
  operation * next;
};

typedef enum the_states {
  lobby = 1,
  in_game = 2,
  count_down = 4
} state;

typedef enum pstate {
  active   = 1, 
  dead     = 2, 
  inactive = 4,
  view     = 8
} player_state;


typedef struct playerData {
  player_state ingame;
  char name[NAME_SIZE];
  int playerNumber; 
  int attempts;
  int cup[MAX_DICE];
  int last_bid;
  int diceno;
} player; 
