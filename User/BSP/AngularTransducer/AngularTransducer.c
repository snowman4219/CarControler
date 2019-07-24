/*******************************************************************************
  * @file    xxx.c
  * @author  郭栋
  * @version V1.0
  * @date    2019-06-04
  * @brief   xxx程序
  ******************************************************************************
  * @attention
  * 未经作者许可，不得用于其它任何用途
  * 版权所有，盗版必究。
  * All rights reserved	
  * 公司：清华无人车团队
  * 开发平台:STM32F429
  * 邮箱：snowman4219@163.com
	
  *		角度传感器由1个主动轮，两个从动轮构成。
	*主动轮转动角度即方向盘角度，主动轮齿数62；
	*从动轮1为大齿轮，齿数为30，从动轮2为小齿轮，齿数为26。
	*方向盘角度由采集到的从动轮1信号为A1及从动轮2信号为A2的两个角度计算得出。(游标算法)
	
  ******************************************************************************
  */
	
#include "AngularTransducer.h"
#include "bsp_led.h"
#include <stdlib.h>
__IO double Angle_1 = 0,Angle_2 = 0;
/**
  * @brief  配置TIM复用输出PWM时用到的I/O
  * @param  无
  * @retval 无
  */
static void TIM1_3_GPIO_Config(void) 
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启LED相关的GPIO外设时钟*/ 
	RCC_AHB1PeriphClockCmd (TIM1_ICPWM_A1_GPIO_CLK, ENABLE);
 	RCC_AHB1PeriphClockCmd (TIM3_ICPWM_A2_GPIO_CLK, ENABLE);

	/* 定时器复用引脚 */
	GPIO_PinAFConfig(TIM1_ICPWM_A1_GPIO_PORT,TIM1_ICPWM_A1_PINSOURCE,TIM1_ICPWM_AF); 
	GPIO_PinAFConfig(TIM3_ICPWM_A2_GPIO_PORT,TIM3_ICPWM_A2_PINSOURCE,TIM3_ICPWM_AF); 
	
	/* 通用定时器PWM输出引脚 */															   	
	GPIO_InitStructure.GPIO_Pin = TIM1_ICPWM_A1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 	
	/* 高级控制定时器PWM输入捕获引脚 */
	GPIO_Init(TIM1_ICPWM_A1_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = TIM3_ICPWM_A2_PIN;
	/* 高级控制定时器PWM输入捕获引脚 */
	GPIO_Init(TIM3_ICPWM_A2_GPIO_PORT, &GPIO_InitStructure);
}

 /**
  * @brief  定时器 TIMx,x[1,8]中断优先级配置
  * @param  无
  * @retval 无
  */
static void TIM1_3_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // 设置中断组为0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
		// 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_TIM_IRQn; 	
		// 设置抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	  // 设置子优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_TIM_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
}


static void TIM1_3_PWMINPUT_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	// 开启TIMx_CLK,x[1,8] 
  RCC_APB2PeriphClockCmd(TIM1_TIM_CLK, ENABLE); 
	RCC_APB1PeriphClockCmd(TIM3_TIM_CLK, ENABLE);
  TIM_TimeBaseStructure.TIM_Period =0xFFFF-1; 	
	// 高级控制定时器时钟源TIMxCLK = HCLK=180MHz 
	// 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=1MHz
  TIM_TimeBaseStructure.TIM_Prescaler = 180-1;	
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  // 计数方式
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;	
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	// 初始化定时器TIMx, x[1,8]
	TIM_TimeBaseInit(TIM1_TIM, &TIM_TimeBaseStructure);	
	
	
	TIM_TimeBaseStructure.TIM_Prescaler = 90-1;
	// 初始化定时器TIMx, x[1,8]
	TIM_TimeBaseInit(TIM3_TIM, &TIM_TimeBaseStructure);
	/* 高级定时器IC1捕获：上升沿触发 TI1FP1 */
  TIM_ICInitStructure.TIM_Channel = TIM1_IC1PWM_CHANNEL;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
  TIM_PWMIConfig(TIM1_TIM, &TIM_ICInitStructure);
	/* 高级定时器IC2捕获：下降沿触发 TI1FP2 */	
	TIM_ICInitStructure.TIM_Channel = TIM1_IC2PWM_CHANNEL;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_IndirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
  TIM_PWMIConfig(TIM1_TIM, &TIM_ICInitStructure);
	/* 通用定时器IC1捕获：上升沿触发 TI1FP1 */
  TIM_ICInitStructure.TIM_Channel = TIM3_IC1PWM_CHANNEL;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
  TIM_PWMIConfig(TIM3_TIM, &TIM_ICInitStructure);
	/* 通用定时器IC2捕获：下降沿触发 TI1FP2 */	
	TIM_ICInitStructure.TIM_Channel = TIM3_IC2PWM_CHANNEL;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_IndirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
  TIM_PWMIConfig(TIM3_TIM, &TIM_ICInitStructure);
	/* Select the TIM1 Input Trigger: TI1FP1 */ 
	TIM_SelectInputTrigger(TIM1_TIM, TIM_TS_TI1FP1);
	/* Select the slave Mode: Reset Mode */ 
	TIM_SelectSlaveMode(TIM1_TIM, TIM_SlaveMode_Reset);
	/* Enable the Master/Slave Mode */ 
	TIM_SelectMasterSlaveMode(TIM1_TIM, TIM_MasterSlaveMode_Enable);
	/* Select the TIM3 Input Trigger: TI1FP1 */ 
	TIM_SelectInputTrigger(TIM3_TIM, TIM_TS_TI1FP1); 
	/* Select the slave Mode: Reset Mode */ 
	TIM_SelectSlaveMode(TIM3_TIM, TIM_SlaveMode_Reset);
	/* Enable the Master/Slave Mode */ 
	TIM_SelectMasterSlaveMode(TIM3_TIM, TIM_MasterSlaveMode_Enable);
  /* 使能高级控制定时器 */
  TIM_Cmd(TIM1_TIM, ENABLE);
	/* 使能通用控制定时器 */
  TIM_Cmd(TIM3_TIM, ENABLE);
  /* 使能高级定时器捕获/比较2中断请求 */
  TIM_ITConfig(TIM1_TIM, TIM_IT_CC1, ENABLE);
	/* 使能通用定时器捕获/比较2中断请求 */
  TIM_ITConfig(TIM3_TIM, TIM_IT_CC1, ENABLE);
}


/**
  * @brief  初始化高级控制定时器定时，1ms产生一次中断
  * @param  无
  * @retval 无
  */
void AngularTransducer_Config(void)
{
	TIM1_3_GPIO_Config();
	TIM1_3_NVIC_Configuration();	
	TIM1_3_PWMINPUT_Config();
}
/**
  * @brief  This function handles TIM interrupt request.
  * @param  None
  * @retval None
  */
void  TIM1_TIM_IRQHandler (void)
{
	double Duty = 0;	
	uint16_t Value_3 = 0,Value_1 = 0;
	if(TIM_GetITStatus(TIM1_TIM,TIM_IT_CC1)!= RESET)
	{
		Value_3 = TIM_GetCapture1(TIM1_TIM);//频率
		Value_1 = TIM_GetCapture2(TIM1_TIM);//占空比
		if(Value_3 != 0)
		{
			Duty = (Value_1 * 100) / Value_3;
			if((Duty<=87.5)&&(Duty>=12.5))//检查占空比是否在12.5-87.5之间,从而屏蔽硬件的不稳定性
			{
				macLED2_TOGGLE();
				Angle_1 = (Duty-12.5)*360.0/(87.5-12.5);
			}
		}
	/* 清除定时器捕获/比较1中断 */
	TIM_ClearITPendingBit(TIM1_TIM, TIM_IT_CC1);	
	}
}
//定时器中断的任务就是更新占空比
void  TIM3_TIM_IRQHandler (void)
{
	double Duty = 0;	
	uint16_t Value_3 = 0,Value_1 = 0;
	if(TIM_GetITStatus(TIM3_TIM,TIM_IT_CC1)!= RESET)
	{
		Value_3 = TIM_GetCapture1(TIM3_TIM);//频率
		Value_1 = TIM_GetCapture2(TIM3_TIM);//占空比
		if(Value_3 != 0)
		{
			Duty = (Value_1 * 100) / Value_3;
			if((Duty<=87.5)&&(Duty>=12.5))//检查占空比是否在12.5-87.5之间,从而屏蔽硬件的不稳定性
			{
				//macLED1_TOGGLE();检测更新频率
				Angle_2 = (Duty-12.5)*360.0/(87.5-12.5);
			}
			
		}
	/* 清除定时器捕获/比较1中断 */
	TIM_ClearITPendingBit(TIM3_TIM, TIM_IT_CC1);	
	}
}
/*
@brief:角度传感器
@param:小齿（A1）和大齿轮（A2）（从动轮）所对应的位置，
@return:0-成功更新角度，1-更新失败
*/
int8_t AngleTransducer(double A1,double A2,double *Angle)
{
	static double Angle_Last = 0,Angle_final = 0;
	double Delt_Angle = 0,Angle_Now = 0;
	
	//检验所传参数的合法性
	if((Angle_1<0||Angle_1>360)||(Angle_2<0||Angle_2>360))
		return 0;
	
	Angle_Now = A1;
	Delt_Angle = Angle_Now-Angle_Last;
	if((Angle_Now<ANGULAR_MAX)&&(Angle_Last>(360 - ANGULAR_MAX)))
	{
		Delt_Angle = Angle_Now+360-Angle_Last;
	}
	if((Angle_Now>(360-ANGULAR_MAX))&&(Angle_Last<ANGULAR_MAX))
	{
		Delt_Angle = Angle_Now-(Angle_Last+360);
	}
	if(Delt_Angle>ANGULAR_MAX)
		return 0;
	
	Angle_final += Delt_Angle*26.0/62.0;
	Angle_Last = Angle_Now;
	
	*Angle = Angle_final;
	return 1;
}
//static void delay_ms(uint16_t ms)
//{
//	uint16_t i,j,k;
//	for(k=0;k<ms;k++)
//		for(i=0;i<3000;i++)
//			for(j=0;j<3000;j++);
//}
//int cmp ( const void *a , const void *b){
//        return *(double *)a - *(double *)b;
//}

//输入变量为double型DutyCycle_A1，DutyCycle_A2
//返回当前角度值
//double AngleTransducerInit(void)
//{
//	double A1_Angle[10] = {0},A2_Angle[10] = {0},A1_sum=0,A2_sum=0,A1_agv=0,A2_agv=0;
//	double AngleDelt = 0,Angle_final = 0;
//	uint8_t i=0,n=0;
//	//分别获取10次大小齿轮所测量的角度值
//	for(i=0;i<10;i++)
//	{
//		A1_Angle[i] = (DutyCycle_A1-12.5)*360/(87.5-12.5);//占空比转换成角度
//		A2_Angle[i] = (DutyCycle_A2-12.5)*360/(87.5-12.5);
//		delay_ms(10);
//	}
//	//排序后剔除最大值和最小值，并求平均值
//	qsort(A1_Angle,10,sizeof(A1_Angle[0]),cmp);
//	qsort(A1_Angle,10,sizeof(A1_Angle[0]),cmp);
//	for(i=1;i<9;i++)
//	{
//		A1_sum += A1_Angle[i];
//		A2_sum += A2_Angle[i];
//	}
//	A1_agv = A1_sum/8;
//	A2_agv = A2_sum/8;
//	AngleDelt = A1_agv-A2_agv;
//	//通过判断A1-A2所处的区间来判断圈数
//	if(((AngleDelt>=0)&&(AngleDelt<=48))||((AngleDelt>=-312)&&(AngleDelt<=-304.616)))//A1转过的圈数为 0
//	{
//		n=0;
//	}
//	else if(((AngleDelt>=55.385)&&(AngleDelt<=96))||((AngleDelt>=-264)&&(AngleDelt<=-249.231)))//A1转过的圈数为 1
//	{
//		n=1;
//	}
//	else if(((AngleDelt>=110.770)&&(AngleDelt<=144.000))||((AngleDelt>=-216.000)&&(AngleDelt<=-193.846)))//A1转过的圈数为 2
//	{
//		n=2;
//	}
//	else if(((AngleDelt>=166.154)&&(AngleDelt<=192.000))||((AngleDelt>=-168.000)&&(AngleDelt<=-138.462)))//A1转过的圈数为 3
//	{
//		n=3;
//	}
//	else if(((AngleDelt>=221.539)&&(AngleDelt<=240))||((AngleDelt>=-120)&&(AngleDelt<=-83.077)))//A1转过的圈数为 4
//	{
//		n=4;
//	}
//	else if(((AngleDelt>=276.923)&&(AngleDelt<=288))||((AngleDelt>=-72)&&(AngleDelt<=-27.692)))//A1转过的圈数为 5
//	{
//		n=5;
//	}
//	else if(((AngleDelt>=332.308)&&(AngleDelt<=336))||((AngleDelt>=-24)&&(AngleDelt<=-24)))//A1转过的圈数为 6
//	{
//		n=6;
//	}
//	else if(((AngleDelt>=-336)&&(AngleDelt<=-332.308))||((AngleDelt>=27.692)&&(AngleDelt<=53.24)))//A1转过的圈数为 7
//	{
//		n=7;
//	}
//	return Angle_final = (n*360+A1_agv)*30/62-630.00;//以最左侧（-630度）为0，则目标点为630
//}

/*********************************************END OF FILE**********************/













