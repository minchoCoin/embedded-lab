
#include "stm32f10x_adc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include "lcd.h"
#include "touch.h"
#include "stm32f10x_dma.h"

void Init(void);
void RccInit(void);
void GpioInit(void);
void AdcInit(void);
void DMA_Configure(void);

const int color[12] = {WHITE,CYAN,BLUE,RED,MAGENTA,LGRAY,GREEN,YELLOW,BROWN,BRRED,GRAY};

volatile uint32_t ADC_Value[1];


void RccInit(void) {
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
}

void GpioInit(void) {
   GPIO_InitTypeDef GPIO_InitStructure;
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
   GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void AdcInit(void) {
    ADC_InitTypeDef ADC_InitStructure;
   
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; // 독립모드(하나의 ADC만 사용) 설정
    ADC_InitStructure.ADC_ScanConvMode = DISABLE; // 스캔 변환 모드(여러 채널을 순차적으로 변환하는 모드) 비활성화
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; //
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 외부 트리거 변환을 사용하지 않도록 설정
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; // 데이터를 오른쪽 정렬
    ADC_InitStructure.ADC_NbrOfChannel = 1; // 변환할 채널 수 1 설정
    ADC_Init(ADC1, &ADC_InitStructure);
   
   // ADC_Channerl_12를 연결, 샘플링 시간 239.5 사이클로 설정
    ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 1, ADC_SampleTime_239Cycles5);
    ADC_DMACmd(ADC1, ENABLE);  // ADC DMA 전송 활성화
   
    ADC_Cmd(ADC1, ENABLE);
    ADC_ResetCalibration(ADC1);

    while(ADC_GetResetCalibrationStatus(ADC1));

    ADC_StartCalibration(ADC1); // ADC 활성화, 보정 시작

    while(ADC_GetCalibrationStatus(ADC1));

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);  // ADC 변환 시작
}

void DMA_Configure(void) {
   DMA_InitTypeDef DMA_Instructure; 
   
   // 구조체를 사용하여 DMA1 채널 1의 초기화 설정
   DMA_Instructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;          
   DMA_Instructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_Value[0]; // ADC_Value배열 주소로 설정
   DMA_Instructure.DMA_DIR = DMA_DIR_PeripheralSRC;
   DMA_Instructure.DMA_BufferSize = 1;   // 전송 버퍼 크기 1 설정                              
   DMA_Instructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;   // 주변 기기 주소의 증가 비활성화
   DMA_Instructure.DMA_MemoryInc = DMA_MemoryInc_Disable;               
   DMA_Instructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;   
   DMA_Instructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;      
   DMA_Instructure.DMA_Mode = DMA_Mode_Circular;   // 순환 모드 설정
   DMA_Instructure.DMA_Priority = DMA_Priority_High;   // 우선순위 높음 설정            
   DMA_Instructure.DMA_M2M = DMA_M2M_Disable;   // 메모리간 전송 비활성화      
   
   // DMA 초기화, DMA 전송 활성화
   DMA_Init(DMA1_Channel1, &DMA_Instructure);                              
   DMA_Cmd(DMA1_Channel1, ENABLE);                                       
}


int main(){
   SystemInit();
   RccInit();
   GpioInit();
   AdcInit();
    DMA_Configure();
   
   LCD_Init();
   Touch_Configuration();
   Touch_Adjust();
   
   int light = 0;            
    int before_light = -1;   
   
   LCD_Clear(WHITE);         
    
   LCD_ShowString(20, 20, "THUR_Team05", RED, WHITE);
   
    while(1){
      // 임계값보다 작을 경우 1, 크면 0
        light = ADC_Value[0] < 3300; 
      
      // 이전 조도값과 다를 경우
        if (before_light != light){      
         if (light) {
            LCD_Clear(GRAY); 
            LCD_ShowString(20, 20, "THUR_Team05", RED, GRAY);
            LCD_ShowNum(20, 50, ADC_Value[0], 4, RED, GRAY); 
         }
         else {
            LCD_Clear(WHITE); 
            LCD_ShowString(20, 20, "THUR_Team05", RED, WHITE);
            LCD_ShowNum(20, 50, ADC_Value[0], 4, RED, WHITE); 
         }
        }
        before_light = light; 
      }
}