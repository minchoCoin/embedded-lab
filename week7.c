char msg[] = "Hello Team05\r\n";

#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include <stdio.h>

#include "misc.h"

/* function prototype */
void RCC_Configure(void);
void GPIO_Configure(void);
void EXTI_Configure(void);
void USART1_Init(void);
void NVIC_Configure(void);

// -KEY2, KEY3 EXTI
void EXTI15_10_IRQHandler(void);
// -KEY1 EXTI
void EXTI4_IRQHandler(void);

void Delay(void);

void sendDataUART1(uint16_t data);

int direction = 1;

//---------------------------------------------------------------------------------------------------

void RCC_Configure(void)
{
   /* UART TX/RX port clock enable */
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

   // -PC
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
   // -PB
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

   /* LED port clock enable */
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

   /* USART1 clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

   /* Alternate Function IO clock enable */
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}

void GPIO_Configure(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

   // TODO: Initialize the GPIO pins using the structure 'GPIO_InitTypeDef' and the function 'GPIO_Init'

    GPIO_InitTypeDef GPIOA_input;
    GPIO_InitTypeDef GPIOA_output;
    GPIO_InitTypeDef GPIOB_input;
    GPIO_InitTypeDef GPIOC_input;
    GPIO_InitTypeDef GPIOD_input;

    /* Button S1, S2, S3 pin setting */
    GPIOC_input.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_13;
    GPIOB_input.GPIO_Pin = GPIO_Pin_10;
    GPIOC_input.GPIO_Mode = GPIO_Mode_IPU;
    GPIOB_input.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &GPIOC_input);
    GPIO_Init(GPIOB, &GPIOC_input);
    
    /* LED pin setting*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
   
    /* UART pin setting */
    //TX
    GPIOA_output.GPIO_Pin = GPIO_Pin_9;
    GPIOA_output.GPIO_Speed = GPIO_Speed_50MHz;
    GPIOA_output.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIOA_output);
    
   //RX
    GPIOA_input.GPIO_Pin = GPIO_Pin_10;
    GPIOA_input.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOA, &GPIOA_input);
   
}

void EXTI_Configure(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;

   // TODO: Select the GPIO pin (Joystick, button) used as EXTI Line using function 'GPIO_EXTILineConfig'
   // TODO: Initialize the EXTI using the structure 'EXTI_InitTypeDef' and the function 'EXTI_Init'
   
    /* Button S1 */
   GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource4);
    EXTI_InitStructure.EXTI_Line = EXTI_Line4;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Button S2 */
   GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource10);
    EXTI_InitStructure.EXTI_Line = EXTI_Line10;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

   /* Button S3 */
   GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource13);
    EXTI_InitStructure.EXTI_Line = EXTI_Line13;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
   // NOTE: do not select the UART GPIO pin used as EXTI Line here
}

void USART1_Init(void)
{
   USART_InitTypeDef USART1_InitStructure;

   // Enable the USART1 peripheral
   USART_Cmd(USART1, ENABLE);
   
   // TODO: Initialize the USART using the structure 'USART_InitTypeDef' and the function 'USART_Init'
   USART1_InitStructure.USART_BaudRate = 9600;
    USART1_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART1_InitStructure.USART_StopBits = USART_StopBits_1;
    USART1_InitStructure.USART_Parity = USART_Parity_No;
    USART1_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART1_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART1_InitStructure);
   
   // TODO: Enable the USART1 RX interrupts using the function 'USART_ITConfig' and the argument value 'Receive Data register not empty interrupt'
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
   
}

void NVIC_Configure(void) {

    NVIC_InitTypeDef NVIC_InitStructure;
    
    // TODO: fill the arg you want
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

   // TODO: Initialize the NVIC using the structure 'NVIC_InitTypeDef' and the function 'NVIC_Init'
   
    // Button S1
    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn; // PC4
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //        
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure); //     

    // Button S2, S3
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn; // PB10, PC13
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //               
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure); //    

    // UART1
   // 'NVIC_EnableIRQ' is only required for USART setting
    NVIC_EnableIRQ(USART1_IRQn);
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // TODO
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // TODO
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void USART1_IRQHandler() {
   uint16_t word;
    if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET){
       // the most recent received data by the USART1 peripheral
        word = USART_ReceiveData(USART1);

        // TODO implement
        if(word == 'a') {
            direction = 1;
        }else if (word == 'b') {
            direction = -1;
        }

        // clear 'Read data register not empty' flag
       USART_ClearITPendingBit(USART1,USART_IT_RXNE);
    }
}

void EXTI15_10_IRQHandler(void) { // when the button is pressed
   if (EXTI_GetITStatus(EXTI_Line10) != RESET) {
      if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == Bit_RESET) {
         // TODO implement
         direction = -1;
      }
        EXTI_ClearITPendingBit(EXTI_Line10);
   }

   if (EXTI_GetITStatus(EXTI_Line13) != RESET) {
      if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == Bit_RESET) {
         // TODO implement
         for(int i = 0; msg[i] != '\0'; ++i) {
            sendDataUART1(msg[i]);
         }
      }
        EXTI_ClearITPendingBit(EXTI_Line13);
   }
}

void EXTI4_IRQHandler(void) { // when the button is pressed
   if (EXTI_GetITStatus(EXTI_Line4) != RESET) {
      if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4) == Bit_RESET) {
         // TODO implement
        direction = 1;
      }
        EXTI_ClearITPendingBit(EXTI_Line4);
   }
}

void Delay(void) {
   int i;

   for (i = 0; i < 2000000; i++) {}
}

void sendDataUART1(uint16_t data) {
   /* Wait till TC is set */
   while ((USART1->SR & USART_SR_TC) == 0);
   USART_SendData(USART1, data);
}

int main(void)
{
    int index = 0;

    SystemInit();

    RCC_Configure();

    GPIO_Configure();

    EXTI_Configure();

    USART1_Init();

    NVIC_Configure();

    while (1) {
       // TODO: implement 
      printf("i: %d\n", index);
      printf("dir: %d\n", direction); 
       GPIOD->BSRR = GPIO_BSRR_BS2 | GPIO_BSRR_BS3 | GPIO_BSRR_BS4 | GPIO_BSRR_BS7;
        switch(index) {
            case 0: { GPIOD->BSRR = GPIO_BSRR_BR2; break;}
            case 1: { GPIOD->BSRR = GPIO_BSRR_BR3; break;}
            case 2: { GPIOD->BSRR = GPIO_BSRR_BR4; break;}
            case 3: { GPIOD->BSRR = GPIO_BSRR_BR7; break;}
        }
        if (direction == 1) {
            index ++;
            if (index == 4) {index = 0;}
        }
        else {
            index --;
            if (index == -1) {index = 3;}
        }

       Delay();
    }
    return 0;
}
