#include <stdbool.h>
#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "core_cm3.h"
#include "stm32f10x_adc.h"
#include "lcd.h"
#include "touch.h"
#include "misc.h"

#include "stm32f10x_tim.h"

void RCC_Configure(void);
void GPIO_Configure(void);
void TIM2_Init(void);
void TIM3_Init(void);
void NVIC_Configure(void);
void TIM2_IRQHandler();
void delayMotor();

uint16_t value, x, y;
uint16_t led1=0, led2=0, count=0, toggle=0, motorflag=0;
uint16_t curDeg = 700, minDeg= 700, maxDeg = 2600;
int color[12] = {WHITE, CYAN, BLUE, RED, MAGENTA, LGRAY, GREEN, YELLOW, BROWN, BRRED, GRAY};
int motor[3] = {700, 1500, 2600};


void RCC_Configure(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE); // LED PD2,3     
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //          PB0     


	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE); // TIMER2, 3 Enable
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
}


void GPIO_Configure(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; // LED PD2, 3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;                   //          PB0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}


void TIM2_Init(void) {

	TIM_TimeBaseInitTypeDef TIM2_InitStructure;

	TIM2_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM2_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM2_InitStructure.TIM_Period = 10000;
	TIM2_InitStructure.TIM_Prescaler = 7200;

	TIM_TimeBaseInit(TIM2, &TIM2_InitStructure);
	TIM_ARRPreloadConfig(TIM2, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	}


void TIM3_Init() {

	TIM_TimeBaseInitTypeDef TIM3_InitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	uint16_t prescale;
	uint16_t period = 20000;
	uint16_t frequency = 50;


	prescale = (uint16_t) (SystemCoreClock / period / frequency);

	TIM3_InitStructure.TIM_Period = period;         
	TIM3_InitStructure.TIM_Prescaler = prescale;
	TIM3_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM3_InitStructure.TIM_CounterMode = TIM_CounterMode_Down;

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0; // us


	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_TimeBaseInit(TIM3, &TIM3_InitStructure);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Disable);
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_Cmd(TIM3, ENABLE);

}


void delayMotor() {
	int i;
	for (i = 0; i < 100000; i ++) {}
	
}

void change(uint16_t per) { //                   
	int pwm_pulse = per;

	TIM_OCInitTypeDef TIM_OCInitStructure;

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = pwm_pulse; 

	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	delayMotor();
}



void NVIC_Configure(void) {

	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_Init(&NVIC_InitStructure);

}

void TIM2_IRQHandler() {
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) != RESET) { //  켱      üũ
		
		
	
		change((uint16_t)curDeg);
		//motorflag = (motorflag+1)%3; // motor  迭   ȸ   flag
		
	
		if(toggle) { // LED             
			
			curDeg += 100;
			
			if (curDeg > maxDeg) {
				curDeg = minDeg;
			}
			
			count++; // count + 1    ְ 
		  
		  // LED 1    1 ʴ        Ŭ
		  if(led1==0) {
			  
			GPIO_ResetBits(GPIOD,GPIO_Pin_2);
			led1 = 1;
			
		  } else {
		
			GPIO_SetBits(GPIOD,GPIO_Pin_2);
			led1 = 0;
			
		  }
		  
		  // 5 ʸ   
		  if(count % 5 ==0) { // count    5       ̸  = 5       
			  
			 //    
			if(led2==0){ 
			  GPIO_ResetBits(GPIOD,GPIO_Pin_3);
			  led2 = 1; 
			} else{
				
			  GPIO_SetBits(GPIOD,GPIO_Pin_3);
			  led2 = 0;
			  
			}
			count = 0;
		  }
		  
		  
		  
		  
		} else { // toggle != 0
			
			curDeg -= 100;
			
			if (curDeg < minDeg) {
				curDeg = maxDeg;
			}
			
		  GPIO_ResetBits(GPIOD,GPIO_Pin_2);
		  GPIO_ResetBits(GPIOD,GPIO_Pin_3);
		}
		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	
	}
	
}

int main(void) {

	SystemInit();

	RCC_Configure();

	GPIO_Configure();

	TIM2_Init();

	TIM3_Init();

	NVIC_Configure();
		
	LCD_Init();
	Touch_Configuration();
	Touch_Adjust();
	LCD_Clear(WHITE);


	LCD_ShowString(80,80,"THU_Team11", BLACK, WHITE); //       
	
	LCD_ShowString(90,200,"button",BLACK,WHITE); //   ư      
	LCD_DrawRectangle(60, 180, 150, 220);
	

	while (1) {
		
	    Touch_GetXY(&x,&y,1); //   ǥ  о      
	    Convert_Pos(x,y,&x,&y); //   ǥ   ȯ
	    if(60<=x && x<=180 && 150<=y && y<=220) { //  簢           (  ư)            1
		  
		  if (toggle==0) { //on    
			LCD_ShowString(80,100,"    ",BLACK,WHITE);
			toggle=1;
			LCD_ShowString(80,100,"ON",BLACK,WHITE);
		  
		  } else { //off    
			LCD_ShowString(80,100,"    ",BLACK,WHITE);
			toggle=0;
			LCD_ShowString(80,100,"OFF",BLACK,WHITE);	
		  }
		  
		}
	}
	
	return 0;
}