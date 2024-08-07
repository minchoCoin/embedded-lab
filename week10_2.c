#include <stdbool.h>
#include "stm32f10x.h"
#include "core_cm3.h"
#include "misc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_adc.h"
#include "lcd.h"
#include "touch.h"

#include "stm32f10x_exti.h"
#include "stm32f10x_rcc.h"

uint16_t value, x, y;
int color[12] = {WHITE, CYAN, BLUE, RED, MAGENTA, LGRAY, GREEN, YELLOW, BROWN, BRRED, GRAY};

//---------------------------------------------------------------------------------------------------

void RCC_Configure(void)
{
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE); 
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE); 
}

void GPIO_Configure(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOC, &GPIO_InitStructure);

  
  
}

void ADC_Configure(void) {
	
    ADC_InitTypeDef ADC_12; 
	
	ADC_12.ADC_ContinuousConvMode = ENABLE; 
    ADC_12.ADC_DataAlign = ADC_DataAlign_Right; 
    ADC_12.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_12.ADC_Mode = ADC_Mode_Independent; 
    ADC_12.ADC_NbrOfChannel = 1; 
    ADC_12.ADC_ScanConvMode = DISABLE;
    
	ADC_Init(ADC1, &ADC_12); 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 1, ADC_SampleTime_239Cycles5);
    ADC_ITConfig(ADC1,ADC_IT_EOC, ENABLE);
    ADC_Cmd(ADC1, ENABLE);
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}


void NVIC_Configure(void) {

    NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
    NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_Init(&NVIC_InitStructure);
    
}

void ADC1_2_IRQHandler() {
	
  if(ADC_GetITStatus(ADC1,ADC_IT_EOC) != RESET) { 
	  
    value = ADC_GetConversionValue(ADC1); 
	
  }
  
  ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
  
}



int main(void)
{

    SystemInit();
    RCC_Configure();
    GPIO_Configure();
    ADC_Configure();
    NVIC_Configure();
    
    LCD_Init();
    Touch_Configuration();
    Touch_Adjust();
    LCD_Clear(WHITE);

    
    LCD_ShowString(80,80,"THUR_Team05",BLACK,WHITE); 
    
    while (1) {
      Touch_GetXY(&x,&y,1); 
	  
      Convert_Pos(x,y,&x,&y); 
      
	  if(x !=0 || y !=0) {
          LCD_DrawCircle(x,y,3);							      
		  
          LCD_ShowNum(80,120,value,4,BLACK,WHITE); 
          LCD_ShowNum(80,140,x,3,BLACK,WHITE); 	  
          LCD_ShowNum(80,160,y,3,BLACK,WHITE);      
        }
      
    }
    return 0;
}
