#include "arm_support.h"

void enterCli(void) {
#ifdef USE_FREERTOS_RTOS
  taskENTER_CRITICAL();
#else
  __disable_irq(); 
#endif
}

void exitCli(void) {
#ifdef USE_FREERTOS_RTOS
  taskEXIT_CRITICAL();
#else
  __enable_irq(); 
#endif
}





