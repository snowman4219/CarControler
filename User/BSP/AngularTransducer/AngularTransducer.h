#ifndef __ANGULAR_TRANSDUCER__
#define __ANGULAR_TRANSDUCER__

#include "stm32f4xx.h"
//引脚定义
/*******************************************************/
/* PWM输入捕获引脚 */
#define TIM1_ICPWM_A1_PIN             GPIO_Pin_8            
#define TIM1_ICPWM_A1_GPIO_PORT       GPIOA                      
#define TIM1_ICPWM_A1_GPIO_CLK        RCC_AHB1Periph_GPIOA
#define TIM1_ICPWM_A1_PINSOURCE			  GPIO_PinSource8

#define TIM3_ICPWM_A2_PIN             GPIO_Pin_6            
#define TIM3_ICPWM_A2_GPIO_PORT       GPIOC                      
#define TIM3_ICPWM_A2_GPIO_CLK        RCC_AHB1Periph_GPIOC
#define TIM3_ICPWM_A2_PINSOURCE				GPIO_PinSource6


#define TIM1_ICPWM_AF							 GPIO_AF_TIM1
#define TIM1_IC1PWM_CHANNEL        TIM_Channel_1
#define TIM1_IC2PWM_CHANNEL        TIM_Channel_2
#define TIM1_IC3PWM_CHANNEL        TIM_Channel_3
#define TIM1_IC4PWM_CHANNEL        TIM_Channel_4

#define TIM3_ICPWM_AF							 GPIO_AF_TIM3
#define TIM3_IC1PWM_CHANNEL        TIM_Channel_1
#define TIM3_IC2PWM_CHANNEL        TIM_Channel_2
#define TIM3_IC3PWM_CHANNEL        TIM_Channel_3
#define TIM3_IC4PWM_CHANNEL        TIM_Channel_4

/* 高级控制定时器 */
#define TIM1_TIM           		    TIM1
#define TIM1_TIM_CLK       		    RCC_APB2Periph_TIM1

/* 通用控制定时器 */
#define TIM3_TIM           		    TIM3
#define TIM3_TIM_CLK       		    RCC_APB1Periph_TIM3

/* 高级定时器 捕获/比较中断 */
#define TIM1_TIM_IRQn					     TIM1_CC_IRQn
#define TIM1_TIM_IRQHandler        TIM1_CC_IRQHandler
/* 通用定时器 捕获/比较中断 */
#define TIM3_TIM_IRQn					     TIM3_IRQn
#define TIM3_TIM_IRQHandler        TIM3_IRQHandler


#define ANGULAR_MAX                10//单位步长下最大角速度

void AngularTransducer_Config(void);
int8_t AngleTransducer(double A1,double A2,double *Angle);
extern __IO double Angle_1,Angle_2;
#endif //__ANGULAR_TRANSDUCER__
