#pragma once

#include "types.h"
#include "player.h"
#include "ccgame.h"
#include "handler.h"

int clientOp(char** argv,int argc,int sd);

extern player *self;

operation* messageOps;

void initialize_builtins();
