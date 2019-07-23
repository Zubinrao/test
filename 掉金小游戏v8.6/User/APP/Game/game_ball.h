#ifndef __GAME_BALL_
#define __GAME_BALL_

#include "gameapi.h"

extern GAME_CLASS gameBall;
GAME_CLASS * PainoReg();
void Piano_Init(void);
void Piano_Game(void);
extern u8 piano_time_count;
extern u8 Time_Count;
#endif // !__GAME_BAL