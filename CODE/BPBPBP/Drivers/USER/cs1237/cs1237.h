#ifndef __CS_1237_H
#define __CS_1237_H

#include "./platform/platform.h"


#define FILT_N   34

enum
{
	IO_INPUT,
	IO_OUTPUT,
	ERR,
};



//uint32_t cs1237_RdDataTest(void);
void cs1237_init(void);
int32_t cs1237_read(void);

#endif



