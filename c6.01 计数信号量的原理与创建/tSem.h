#ifndef TSEM_H
#define TSEM_H

#include "tEvent.h"

typedef struct _tSem//信号量结构
{
	tEvent event;
	uint32_t count;
	uint32_t maxCount;
}tSem;

void tSemInit (tSem * sem, uint32_t startCount, uint32_t maxCount);


#endif

