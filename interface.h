#pragma once

#include "types.h"
#include "handler.h"
#include "ssgame.h"
#include <sys/time.h>
#include <string.h>

#include <sys/select.h>

extern state MYSTATE;
extern int ACTIVE_PLAYERS; 
extern int CURRENT_PLAYER;

extern int CURMAXPLAYA;
extern int MIN_PLAYERS;
extern int COUNT_TIME;
extern int PLAY_TIME;
extern int MAX_ATTEMPT;


operation * messageOps;

int serverOp(struct timeval* tv, 
	     char** argv, 
	     int argc, 
	     int maxfd, 
	     int fd);

void initialize_builtins();
