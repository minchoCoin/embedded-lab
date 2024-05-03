#include "stm32f10x.h"

#define RCC (*(volatile unsigned int*)0x40021018)
/*
 - RCC:     0x4002 1000
 - Port A:  0x4001 0800
 - Port B:  0x4001 0C00
 - port C:  0x4001 1000
 - port D:  0x4001 1400
*/
//RCC_APB2ENR == 0x40021000 + 0x18

#define PA_CRL (*(volatile unsigned int*)0x40010800)//PA_CRL == PA_base_address
#define PC_CRL (*(volatile unsigned int*)0x40011000)//PC_CRL == PC_base_address
#define PD_CRL (*(volatile unsigned int*)0x40011400)//PD_CRL == PD_base_address

#define PB_CRH (*(volatile unsigned int*)0x40010C04)//PB_CRH == PB_base_address + 0x04
#define PC_CRH (*(volatile unsigned int*)0x40011004)//PC_CRH == PC_base_address + 0x04

#define PA_IDR (*(volatile unsigned int*)0x40010808)//PA_IDR == PA_base_address + 0x08
#define PB_IDR (*(volatile unsigned int*)0x40010C08)//PB_IDR == PB_base_address + 0x08
#define PC_IDR (*(volatile unsigned int*)0x40011008)//PC_IDR == PC_base_address + 0x08

#define PD_BSRR (*(volatile unsigned int*)0x40011410)//PD_BSRR == PD_base_address + 0x10

//IDR과 BSRR 레지스터에서 해당 핀을 확인하는 용도 및 해당핀을 set/reset하는데 쓰인다.
#define PIN_0 0x00000001 
#define PIN_2 0x00000004
#define PIN_3 0x00000008
#define PIN_4 0x00000010
#define PIN_7 0x00000080
#define PIN_10 0x00000400
#define PIN_13 0x00002000
//그림7에서 알 수 있듯, 예를 들어 포트A의 0번 핀이 0인지 확인하려면
//!(PA_IDR & PIN_0)로 확인할 수 있다.

//그림8에서 알 수 있듯, 예를 들어 BSRR 레지스터에 2번핀을 set으로 하려면
//PD_BSRR |= PIN_2로 하면된다. 그리고 그림2에서 알 수 있듯, PD2,3,4,7이 reset이 되어야 LED가 켜지게 된다.

//RCC의 초기값은 0x00이라서, or 연산자를 이용해 초기화해도됨
RCC |= 0x0000003c;

PA_CRL &= 0xfffffff0;//PA0를 설정하기 위해 PA0의 CRL값을 0으로 먼저 초기화한다(or 연산으로 값을 넣기 위함).
PA_CRL |= 0x00000008;//PA0을 input mode로 설정한다.
  
PC_CRL &= 0xfff0ffff;//PC4를 설정하기 위해 PC4의 CRL값을 0으로 먼저 초기화한다.
PC_CRL |= 0x00080000;//PC4을 input mode로 설정한다.
  

PB_CRH &= 0xfffff0ff;//PB10을 설정하기 위해 PB10의 CRH의 값을 0으로 먼저 초기화한다.
PB_CRH |= 0x00000800;//PB10을 input mode로 설정한다.

PC_CRH &= 0xff0fffff;//PC13을 설정하기 위해 PC13의 CRH의 값을 0으로 먼저 초기화한다.
PC_CRH |= 0x00800000;//PC13을 input mode로 설정한다.

PD_CRL &= 0x0ff000ff;//PD2, PD3, PD4, PD7을 설정하기 위해 각 포트의 CRL값을 0으로 먼저 초기화한다.
PD_CRL |= 0x10011100;//PD2,PD3,PD4,PD7을 output mode로 설정한다.

PD_BSRR |= PIN_2; // BSRR을 이용하여 PD2번 핀을 set한다. 그림2에서 PD2번핀이 set되면 LED가 꺼진다.
PD_BSRR |= PIN_3; // BSRR을 이용하여 PD3번 핀을 set한다.
PD_BSRR |= PIN_4; // BSRR을 이용하여 PD4번 핀을 set한다.
PD_BSRR |= PIN_7; // BSRR을 이용하여 PD7번 핀을 set한다.

int key = 0;
  while(1) {
    if (!(PC_IDR & PORT_4)){ //Key1 버튼이 눌리면 PC_IDR에서 뒤에서 4번째 비트가 0이된다. 따라서 이것을 port_4와 & 하면 0이되고, key=1이 들어가게 된다.
      key = 1;
    }
    else if (!(PB_IDR & PORT_10)){ // key2 버튼이 눌리지 않는다면 PC_IDR에서 뒤에서 10번째 비트가 1이된다. 따라서 이것을 port_10과 & 하면 1이 되고, key=2가 실행되지 않는다.
      key = 2;
    }
    else if (!(PC_IDR & PORT_13)){
      key = 3;
    }
    else if (!(PA_IDR & PORT_0)){
      key = 4;
    }
    
    //printf("key = %d\n", key);
    
    switch (key) {
    case 0:
      break;
    case 1:
    //PD_BSRR을 이용하여 포트2번과 포트7번을 reset한다.
      PD_BSRR |= PORT_2<<16;
      PD_BSRR |= PORT_7<<16;
      break;
    case 2:
    //PD_BSRR을 이용하여 포트2번과 포트7번을 set한다.
      PD_BSRR |= PORT_2;
      PD_BSRR |= PORT_7;
      break;
    case 3:
    //PD_BSRR을 이용하여 포트3번과 포트4번을 reset한다.
      PD_BSRR |= PORT_3<<16;
      PD_BSRR |= PORT_4<<16;
      break;
    case 4:
     //PD_BSRR을 이용하여 포트3번과 포트4번을 set한다.
      PD_BSRR |= PORT_3;
      PD_BSRR |= PORT_4;
      break;
    }
    
    key = 0;
  }
