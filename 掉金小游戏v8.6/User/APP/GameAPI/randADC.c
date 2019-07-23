#include "randADC.h"
#include "bsp.h"
extern __IO u16 AD_Value[CH_N*Sampling_N];

u8 randomADC(u8 x, u8 y){
    u8 buf;
	static u16	Indexes=0;
	static u16	Indexes_max = CH_N*Sampling_N;
	
	buf = AD_Value[Indexes] % (y-x+1) + x;
	Indexes++;
	if(Indexes == Indexes_max)
	Indexes = 0;
	return buf;
}
