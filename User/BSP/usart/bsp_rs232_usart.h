#ifndef __RS232_USART_H
#define	__RS232_USART_H

#include "stm32f4xx.h"
#include <stdio.h>



//���Ŷ���
/*******************************************************/
#define RS232_USART                             USART2
#define RS232_USART_CLK                         RCC_APB1Periph_USART2

#define RS232_USART_RX_GPIO_PORT                GPIOD
#define RS232_USART_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOD
#define RS232_USART_RX_PIN                      GPIO_Pin_6
#define RS232_USART_RX_AF                       GPIO_AF_USART2
#define RS232_USART_RX_SOURCE                   GPIO_PinSource6

#define RS232_USART_TX_GPIO_PORT                GPIOD
#define RS232_USART_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOD
#define RS232_USART_TX_PIN                      GPIO_Pin_5
#define RS232_USART_TX_AF                       GPIO_AF_USART2
#define RS232_USART_TX_SOURCE                   GPIO_PinSource5


#define RS232_USART_IRQHandler                   USART2_IRQHandler
#define RS232_USART_IRQ                 				USART2_IRQn
/************************************************************/


//���ڲ�����
#define RS232_USART_BAUDRATE                    115200



void USART_Config(void);
void Usart_SendStr_length( USART_TypeDef * pUSARTx, uint8_t *str,uint32_t strlen );
void Usart_SendString( USART_TypeDef * pUSARTx, uint8_t *str);

//int fputc(int ch, FILE *f);

#endif /* __USART1_H */