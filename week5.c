#include "stm32f10x.h"

#define RCC_APB2ENR (*(volatile unsigned int*)0x40021018) // 

#define PB_CRH (*(volatile unsigned int*)0x40010C04) //

#define PC_CRL (*(volatile unsigned int*)0x40011000) //
#define PC_CRH (*(volatile unsigned int*)0x40011004) //

#define PD_CRL (*(volatile unsigned int*)0x40011400) //


#define PB_IDR (*(volatile unsigned int*)0x40010C08)
#define PC_IDR (*(volatile unsigned int*)0x40011008)

#define PD_BSRR (*(volatile unsigned int*)0x40011410)

#define PIN_4 0x00000010
#define PIN_7 0x00000080
#define PIN_10 0x00000400
#define PIN_13 0x00002000


void delay() {
    int i;
    for (i = 0; i < 10000000; i++) {}
}


int main(void) {
  RCC_APB2ENR |= 0x00000038; // Port B, C, D 에 클럭 활성화(bit 3, 4, 5)
  
  PB_CRH &= 0xfffff0ff;
  PB_CRH |= 0x00000800; // 10 00(Input with pull-up / pull down, Input mode(reset state))

  PC_CRL &= 0xfff0ffff;
  PC_CRL |= 0x00080000; // 10 00(Input with pull-up / pull down, Input mode(reset state))

  PC_CRH &= 0xff0fffff;
  PC_CRH |= 0x00800000; // 10 00(Input with pull-up / pull down, Input mode(reset state))

  PD_CRL &= 0x0ff0ffff;
  PD_CRL |= 0x10010000; // 00 01(General purpose output push-pull, Output mode, max speed 10MHz)
  

  PD_BSRR |= PIN_4 << 16;
  PD_BSRR |= PIN_7 << 16;
  
  
  int key = 0;
  while(1) {
    // 눌렀을 때가 낮은 전압
    if (!(PC_IDR & PIN_4)){
      key = 1;
    }
    else if (!(PB_IDR & PIN_10)){
      key = 2;
    }
    else if (!(PC_IDR & PIN_13)){
      key = 3;
    }

    switch (key) {
    case 0:
      break;
    case 1: //  kEY1 :  1번, 2번 
      PD_BSRR |= PIN_7;   
      PD_BSRR |= PIN_4;
      delay();
      PD_BSRR |= PIN_7 << 16;
      PD_BSRR |= PIN_4 << 16;
      break;
      
    case 2: // KEY2 :  1번
      PD_BSRR |= PIN_7;   
      delay();
      PD_BSRR |= PIN_7 << 16;
      delay();
      break;
      
    case 3: // KEY3 :  2번
      PD_BSRR |= PIN_4;   
      delay();
      PD_BSRR |= PIN_4 << 16;
      delay();
      break;
    }
    key = 0;
  }
}