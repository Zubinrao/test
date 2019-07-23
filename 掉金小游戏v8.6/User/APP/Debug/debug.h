#ifndef __DEBUG_H
#define __DEBUG_H
#include "usart.h"
#include <stdio.h>

#define DEBUG_MSG_ERROR 0
#define DEBUG_MSG_WARNG 1
#define DEBUG_MSG_INFO 2

void Debug_SendMsg(char * moduleName, char * msg, u8 msgType);

#endif
