#include "tinyOS.h"

void tTaskDelay (uint32_t delay)
{
	uint32_t status = tTaskEnterCritical();//添加临界区保护
	tTimeTaskWait(currentTask, delay);
	tTaskSchedUnRdy(currentTask);	
	tTaskExitCritical(status);
	tTaskSched();	
}
