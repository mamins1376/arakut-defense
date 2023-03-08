#include "stm32f103xb.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_tim.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_usart.h"
void init()
{
    //enabling clocks
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_TIM1_CLK_ENABLE();

    //configuring GPIO pins
    HAL_GPIO_Init(GPIOA, &(GPIO_InitTypeDef){
                             .Pin = GPIO_PIN_3, .Mode = GPIO_MODE_AF_INPUT, .Pull = GPIO_NOPULL, .Speed = GPIO_SPEED_FREQ_LOW});
    LL_TIM_SetAutoReload(TIM1, 100 - 1);
    LL_TIM_SetPrescaler(TIM1, 800);
    LL_TIM_EnableARRPreload(TIM1);
    LL_TIM_EnableAllOutputs(TIM1);
    LL_TIM_EnableCounter(TIM1);
    LL_TIM_OC_SetMode(TIM1, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1);
    LL_TIM_OC_SetMode(TIM1, LL_TIM_CHANNEL_CH2, LL_TIM_OCMODE_PWM1);
    LL_TIM_OC_SetMode(TIM1, LL_TIM_CHANNEL_CH3, LL_TIM_OCMODE_PWM1);
    LL_TIM_OC_EnablePreload(TIM1, LL_TIM_CHANNEL_CH1);
    LL_TIM_OC_EnablePreload(TIM1, LL_TIM_CHANNEL_CH2);
    LL_TIM_OC_EnablePreload(TIM1, LL_TIM_CHANNEL_CH3);
    LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH2 | LL_TIM_CHANNEL_CH3 | LL_TIM_CHANNEL_CH1N | LL_TIM_CHANNEL_CH2N | LL_TIM_CHANNEL_CH3N);

    //configuring PWM Timer
    LL_TIM_OC_SetCompareCH1(TIM1, 100);
    LL_TIM_OC_SetCompareCH2(TIM1, 100);
    LL_TIM_OC_SetCompareCH3(TIM1, 100);

    //configuring Serial
    LL_USART_SetBaudRate(USART2, 8000000, 9600);
    LL_USART_ConfigAsyncMode(USART2);
    LL_USART_Enable(USART2);
    LL_USART_EnableDirectionRx(USART2);
}

void apply_value(int channel, int value)
{
    int dir = value > 0; // get direction from value sign
    if (!dir)
        value += 32; // value is negative in case of reverse direction
    value = value * 100 / 32; // convert value range to 0-100
    if (channel == 1)
    {
        if (dir)
        {
            //enable forward direction
            HAL_GPIO_Init(GPIOA, &(GPIO_InitTypeDef){
                                     .Pin = GPIO_PIN_8, .Mode = GPIO_MODE_AF_PP, .Pull = GPIO_NOPULL, .Speed = GPIO_SPEED_FREQ_LOW});
            HAL_GPIO_Init(GPIOB, &(GPIO_InitTypeDef){
                                     .Pin = GPIO_PIN_13, .Mode = GPIO_MODE_OUTPUT_PP, .Pull = GPIO_NOPULL, .Speed = GPIO_SPEED_FREQ_LOW});
            //disable reverse direction
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
        }
        else
        {
            //enable reverse direction
            HAL_GPIO_Init(GPIOB, &(GPIO_InitTypeDef){
                                     .Pin = GPIO_PIN_13, .Mode = GPIO_MODE_AF_PP, .Pull = GPIO_NOPULL, .Speed = GPIO_SPEED_FREQ_LOW});
            HAL_GPIO_Init(GPIOA, &(GPIO_InitTypeDef){
                                     .Pin = GPIO_PIN_8, .Mode = GPIO_MODE_OUTPUT_PP, .Pull = GPIO_NOPULL, .Speed = GPIO_SPEED_FREQ_LOW});
            //disable forward direction
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
        }
        //apply value to timer
        LL_TIM_OC_SetCompareCH1(TIM1, value);
    }
    if (channel == 2)
    {
        if (dir)
        {
            //enable forward direction
            HAL_GPIO_Init(GPIOA, &(GPIO_InitTypeDef){
                                     .Pin = GPIO_PIN_9, .Mode = GPIO_MODE_AF_PP, .Pull = GPIO_NOPULL, .Speed = GPIO_SPEED_FREQ_LOW});
            HAL_GPIO_Init(GPIOB, &(GPIO_InitTypeDef){
                                     .Pin = GPIO_PIN_14, .Mode = GPIO_MODE_OUTPUT_PP, .Pull = GPIO_NOPULL, .Speed = GPIO_SPEED_FREQ_LOW});
            //disable reverse direction
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
        }
        else
        {
            //enable reverse direction
            HAL_GPIO_Init(GPIOB, &(GPIO_InitTypeDef){
                                     .Pin = GPIO_PIN_14, .Mode = GPIO_MODE_AF_PP, .Pull = GPIO_NOPULL, .Speed = GPIO_SPEED_FREQ_LOW});
            HAL_GPIO_Init(GPIOA, &(GPIO_InitTypeDef){
                                     .Pin = GPIO_PIN_9, .Mode = GPIO_MODE_OUTPUT_PP, .Pull = GPIO_NOPULL, .Speed = GPIO_SPEED_FREQ_LOW});
            //disable forward direction
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
        }
        //apply value to timer
        LL_TIM_OC_SetCompareCH2(TIM1, value);
    }
    if (channel == 3)
    {
        if (dir)
        {
            //enable forward direction
            HAL_GPIO_Init(GPIOA, &(GPIO_InitTypeDef){
                                     .Pin = GPIO_PIN_10, .Mode = GPIO_MODE_AF_PP, .Pull = GPIO_NOPULL, .Speed = GPIO_SPEED_FREQ_LOW});
            HAL_GPIO_Init(GPIOB, &(GPIO_InitTypeDef){
                                     .Pin = GPIO_PIN_15, .Mode = GPIO_MODE_OUTPUT_PP, .Pull = GPIO_NOPULL, .Speed = GPIO_SPEED_FREQ_LOW});
            //disable reverse direction
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
        }
        else
        {
            //enable reverse direction
            HAL_GPIO_Init(GPIOB, &(GPIO_InitTypeDef){
                                     .Pin = GPIO_PIN_15, .Mode = GPIO_MODE_AF_PP, .Pull = GPIO_NOPULL, .Speed = GPIO_SPEED_FREQ_LOW});
            HAL_GPIO_Init(GPIOA, &(GPIO_InitTypeDef){
                                     .Pin = GPIO_PIN_10, .Mode = GPIO_MODE_OUTPUT_PP, .Pull = GPIO_NOPULL, .Speed = GPIO_SPEED_FREQ_LOW});
            //disable forward direction
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
        }
        //apply value to timer
        LL_TIM_OC_SetCompareCH3(TIM1, value);
    }
}

void main(void)
{
    init(); //initialize hardware
    apply_value(1, 0); //set channel 0 to stop
    apply_value(2, 0); //set channel 1 to stop
    apply_value(3, 0); //set channel 2 to stop
    for (;;)
    {
        //get one byte from serial
        while (!LL_USART_IsActiveFlag_RXNE(USART2))
            __NOP();
        unsigned char byte = LL_USART_ReceiveData8(USART2);
        // (byte>>6) takes two higher bits as channel number and pass to function
        // ((byte & 0x3F) - 32) takes 6 lower bits, down shifts them to range of -32 to +32 and pass to function
        apply_value(byte >> 6, (byte & 0x3F) - 32);
    }
}