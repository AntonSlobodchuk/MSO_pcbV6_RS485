#ifndef _GPIO_H
#define _GPIO_H


#include "stm32f10x.h"

/*** Out mode defined *********************************************************/
#define OUT_MODE_PP     0       // push-pull
#define OUT_MODE_OD     1       // open-drain
#define OUT_MODE_ALT    2       // alternate
#define OUT_MODE_GEN    0       // general       
/*** Out frequency defined ****************************************************/
#define OUT_10_MHZ      1       // 10 Mhz
#define OUT_2_MHZ       2       // 2 Mhz
#define OUT_50_MHZ      3       // 50 Mhz
/*** In mode defined **********************************************************/
#define IN_MODE_AN      0       // Analog in
#define IN_MODE_FLOAT   1       // Floating state
#define IN_MODE_PULL_ON 2       // Pull-up/down ON
/*** In pull defined **********************************************************/
#define IN_PULL_UP      1       // Pull-up
#define IN_PULL_DOWN    0       // Pull-down
/******************************************************************************/


/******************************************************************************/
#define SET_PIN(port, bit)      (port)->BSRR = (0x0001<<(bit))
#define CLR_PIN(port, bit)      (port)->BSRR = (0x10000<<(bit))
#define GET_PIN(port, bit)      (((port)->IDR & (1<<(bit)))!=0)
#define TGL_PIN(port, bit)      ((port)->ODR ^=(1<<(bit)))
/******************************************************************************/

/******************************************************************************/
#define PORT_TAKT_ON(port)      do{                             \
  RCC->APB2ENR |=(1<<((((long)port-(long)GPIOA)/                \
                 ((long)GPIOB-(long)GPIOA))+2));                \
}while(0)
/******************************************************************************/
#define CONFIG_PIN_OUT(port, bit, mode, freq)  do{      \
  if(bit > 7){                                          \
    port->CRH &= ~((3)<<((bit-8)*4));                   \
    port->CRH |=  ((freq)<<((bit-8)*4));                \
    port->CRH &= ~((3)<<((bit-8)*4+2));                 \
    port->CRH |=  ((mode)<<((bit-8)*4+2));              \
  }                                                     \
  else{                                                 \
    port->CRL &= ~((3)<<((bit)*4));                     \
    port->CRL |= ((freq)<<(bit*4));                     \
    port->CRL &= ~((3)<<((bit)*4+2));                   \
    port->CRL |= ((mode)<<(bit*4+2));                   \
  }                                                     \
}while(0)
/******************************************************************************/
#define CONFIG_PIN_IN(port, bit, mode, pull_mode)  do{  \
  if(bit > 7){                                          \
    port->CRH &=~(3<<((bit-8)*4));                      \
    port->CRH &= ~((3)<<((bit-8)*4+2));                 \
    port->CRH |= ((mode)<<((bit-8)*4+2));               \
    port->BSRR =((pull_mode?0x0001:0x10000)<<(bit));    \
  }                                                     \
  else{                                                 \
    port->CRL &=~(3<<((bit-8)*4));                      \
    port->CRL &= ~((3)<<((bit-8)*4+2));                 \
    port->CRL |= ((mode)<<((bit-8)*4+2));               \
    port->BSRR =((pull_mode?0x0001:0x10000)<<(bit));    \
  }                                                     \
}while(0)
/******************************************************************************/



#endif  // _GPIO_H
