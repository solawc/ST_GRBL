#include "hal_tim.h"
// #include "stepper.h"

/*  
 * Note: I use general timer by tim3 and tim4 on stm32h429,
 * also you can choose other timer as step count, the timer,
 * need handle with updata
*/
hal_tim_t hal_step_tim;

void hal_set_timer_init(void) {

    SET_TIM_CLK_ENABLED();   

    STEP_SET_TIM.Instance = SETP_SET_TIM; 
    STEP_SET_TIM.Init.Period = 1-1;     
    STEP_SET_TIM.Init.Prescaler = 10-1;
    STEP_SET_TIM.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    STEP_SET_TIM.Init.CounterMode = TIM_COUNTERMODE_UP;
    HAL_TIM_Base_Init(&STEP_SET_TIM);

    HAL_NVIC_SetPriority(SET_TIM_IRQn, 0, 1);       // 使定时器中断的优先级保持最高
    HAL_NVIC_DisableIRQ(SET_TIM_IRQn);
    HAL_TIM_Base_Start_IT(&STEP_SET_TIM);

    hal_step_tim.set_timer_psc = 0;
    hal_step_tim.set_timer_arr = 0;

    hal_step_tim.reset_timer_psc = 0;
    hal_step_tim.reset_timer_arr = 0;
}


/*
 * 设置用于拉低的定时器的tick为1us
*/
void hal_reset_timer_init(void) {

    RESET_TIM_CLK_ENABLED();
    STEP_RESET_TIM.Instance = SETP_RESET_TIM;
    STEP_RESET_TIM.Init.Period = 1-1;     
    STEP_RESET_TIM.Init.Prescaler = 10-1; 
    STEP_RESET_TIM.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    STEP_RESET_TIM.Init.CounterMode = TIM_COUNTERMODE_UP;
    HAL_TIM_Base_Init(&STEP_RESET_TIM);

    HAL_NVIC_SetPriority(RESET_TIM_IRQn, 0, 2);
    HAL_NVIC_DisableIRQ(RESET_TIM_IRQn);
    HAL_TIM_Base_Start_IT(&STEP_RESET_TIM);
}

void hal_set_timer_irq_enable(void) {
    HAL_NVIC_EnableIRQ(SET_TIM_IRQn);
}

void hal_set_timer_irq_disable(void) {
    NVIC_DisableIRQ(SET_TIM_IRQn);
}

void hal_tim_set_reload(TIM_HandleTypeDef *htim, uint32_t reload) {
    __HAL_TIM_SET_AUTORELOAD(htim, reload);
}
 
void hal_set_tim_cnt(TIM_HandleTypeDef *htim, uint32_t cnt) {
    __HAL_TIM_SET_COUNTER(htim, cnt);
}

void hal_tim_generateEvent_update(TIM_HandleTypeDef *htim) {
    HAL_TIM_GenerateEvent(htim, TIM_EVENTSOURCE_UPDATE);
}

void hal_tim_clear_flag_update(TIM_HandleTypeDef *htim) {
    __HAL_TIM_CLEAR_FLAG(htim, TIM_FLAG_UPDATE);
}

void hal_reset_timer_irq_enable(void) {
    HAL_NVIC_EnableIRQ(RESET_TIM_IRQn);
}

void hal_reset_timer_irq_disable(void) {
    HAL_NVIC_DisableIRQ(RESET_TIM_IRQn);
}

void STEP_RESET_HANDLER(void) {
    HAL_TIM_IRQHandler(&STEP_RESET_TIMER);
}

void STEP_SET_HANDLER(void) {
    HAL_TIM_IRQHandler(&STEP_SET_TIMER);
}

/*******************PWM SET***************************/

static void laser_pin_config() {

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_TIM4_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = LASER_PIN_AF;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void hal_pwm_init() {

    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

    laser_pin_config();

    LASER_TIM.Instance = LASER_TIM_PORT;
    LASER_TIM.Init.Prescaler = 64-1;       // psc
    LASER_TIM.Init.CounterMode = TIM_COUNTERMODE_UP;
    LASER_TIM.Init.Period = 1000-1;            // arr
    LASER_TIM.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    LASER_TIM.Init.RepetitionCounter = 0;
    LASER_TIM.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_OC_Init(&LASER_TIM) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&LASER_TIM, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_LOW;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    if (HAL_TIM_OC_ConfigChannel(&LASER_TIM, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }
    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime = 0;
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
    if (HAL_TIMEx_ConfigBreakDeadTime(&LASER_TIM, &sBreakDeadTimeConfig) != HAL_OK)
    {
        Error_Handler();
    }
    HAL_TIM_PWM_Start(&LASER_TIM, TIM_CHANNEL_2);
}

void hal_pwm_set(uint32_t duty) {
    __HAL_TIM_SetCompare(&LASER_TIM, TIM_CHANNEL_2, duty);
}

uint32_t hal_pwm_ccr_get(void) {
    
    return __HAL_TIM_GetCompare(&LASER_TIM, TIM_CHANNEL_2);
}


