#ifndef __LOOKUP__H
#define __LOOKUP__H

#include "stm32f10x.h"
#include "ff.h"

const char* strcasestr(const char* str, const char* subStr);
char FatReadDir(const char* Filedir,const char* Filetype,UINT* Number);

#endif
