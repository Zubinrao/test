#ifndef __G_PLATFORM_H
#define __G_PLATFORM_H

#include "stm32f10x.h"
#include "gameapi.h"

extern GAME_CLASS * GAMES[4];

void gamePlatformLoop(void);

#endif
