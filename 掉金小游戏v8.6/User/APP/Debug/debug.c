#include "debug.h"
#include "string.h"

void Debug_SendMsg(char * moduleName, char * msg, u8 msgType){
	USART3_Send("[", 1);
	USART3_Send(moduleName, strlen(moduleName));
	USART3_Send("] ", 2);
	USART3_Send("<", 1);
	switch(msgType){
		case DEBUG_MSG_ERROR :
			USART3_Send("ERROR", 5);
			break;
		case DEBUG_MSG_WARNG :
			USART3_Send("WARNING", 7);
			break;
		case DEBUG_MSG_INFO :
			USART3_Send("INFO", 4);
			break;
	}
	USART3_Send("> ", 2);
	USART3_Send(msg, strlen(msg));
	USART3_Send("\n\r ", 2);
}

