#include "grbl_hal.h"

grbl_hw_t grbl_hw_get;

void grbl_report_mcu_info(void) {

    grbl_hw_info_get();

    printf("/*********************************************************/\r\n");
    printf("ST-GRBL\r\n");
    printf("CPU Name:%s\r\n", MCU_INFI_NAME);
    printf("CPU Flash:%s\r\n", MCU_INFI_NAME);
    printf("CPU RAM:%s\r\n", MCU_INFI_NAME);
    printf("CPU Clock:%dMHz\r\n", grbl_hw_get.mcu_clk/1000000);
    printf("CPU Step Clock:%dMHz\r\n", grbl_hw_get.step_tim_clk/1000000);
    printf("Flash Info 0x%x, flash_size = %dMB\n", (uint32_t)sFlash.flash_id, (sFlash.flash_size / (uint32_t)1024));
    printf("/*********************************************************/\r\n");
}

void grbl_hw_info_get(void) {
    grbl_hw_get.mcu_clk = HAL_RCC_GetHCLKFreq();
    grbl_hw_get.step_tim_clk = HAL_RCC_GetHCLKFreq();
}

