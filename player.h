#pragma once
#include "types.h"
#include <stdio.h>

extern int CURMAXPLAYA;
extern int MIN_PLAYERS;
extern int COUNT_TIME;
extern int PLAY_TIME;
extern int MAX_ATTEMPT;

int null_player(player * p);

int start_player(player * p,char* name, int playerNum, player_state s);

int set_player(player * p, int cup[]);

int count_dice(player *p, int dice_val);

int attempt_punish(player* p);

int player_loose(player*p);
