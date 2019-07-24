/*******************************************************************************
  * @file    xxx.c
  * @author  ����
  * @version V1.0
  * @date    2019-06-04
  * @brief   xxx����
  ******************************************************************************
  * @attention
  * δ��������ɣ��������������κ���;
  * ��Ȩ���У�����ؾ���
  * All rights reserved	
  * ��˾���廪���˳��Ŷ�
  * ����ƽ̨:STM32F429
  * ���䣺snowman4219@163.com
	
  *		�Ƕȴ�������1�������֣������Ӷ��ֹ��ɡ�
	*������ת���Ƕȼ������̽Ƕȣ������ֳ���62��
	*�Ӷ���1Ϊ����֣�����Ϊ30���Ӷ���2ΪС���֣�����Ϊ26��
	*�����̽Ƕ��ɲɼ����ĴӶ���1�ź�ΪA1���Ӷ���2�ź�ΪA2�������Ƕȼ���ó���(�α��㷨)
	
  ******************************************************************************
  */
	
#include "AngularTransducer.h"
#include "bsp_led.h"
#include <stdlib.h>
__IO double Angle_1 = 0,Angle_2 = 0;
/**
  * @brief  ����TIM�������PWMʱ�õ���I/O
  * @param  ��
  * @retval ��
  */
static void TIM1_3_GPIO_Config(void) 
{
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*����LED��ص�GPIO����ʱ��*/ 
	RCC_AHB1PeriphClockCmd (TIM1_ICPWM_A1_GPIO_CLK, ENABLE);
 	RCC_AHB1PeriphClockCmd (TIM3_ICPWM_A2_GPIO_CLK, ENABLE);

	/* ��ʱ���������� */
	GPIO_PinAFConfig(TIM1_ICPWM_A1_GPIO_PORT,TIM1_ICPWM_A1_PINSOURCE,TIM1_ICPWM_AF); 
	GPIO_PinAFConfig(TIM3_ICPWM_A2_GPIO_PORT,TIM3_ICPWM_A2_PINSOURCE,TIM3_ICPWM_AF); 
	
	/* ͨ�ö�ʱ��PWM������� */															   	
	GPIO_InitStructure.GPIO_Pin = TIM1_ICPWM_A1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 	
	/* �߼����ƶ�ʱ��PWM���벶������ */
	GPIO_Init(TIM1_ICPWM_A1_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = TIM3_ICPWM_A2_PIN;
	/* �߼����ƶ�ʱ��PWM���벶������ */
	GPIO_Init(TIM3_ICPWM_A2_GPIO_PORT, &GPIO_InitStructure);
}

 /**
  * @brief  ��ʱ�� TIMx,x[1,8]�ж����ȼ�����
  * @param  ��
  * @retval ��
  */
static void TIM1_3_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // �����ж���Ϊ0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
		// �����ж���Դ
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_TIM_IRQn; 	
		// ������ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	  // ���������ȼ�
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
	// ����TIMx_CLK,x[1,8] 
  RCC_APB2PeriphClockCmd(TIM1_TIM_CLK, ENABLE); 
	RCC_APB1PeriphClockCmd(TIM3_TIM_CLK, ENABLE);
  TIM_TimeBaseStructure.TIM_Period =0xFFFF-1; 	
	// �߼����ƶ�ʱ��ʱ��ԴTIMxCLK = HCLK=180MHz 
	// �趨��ʱ��Ƶ��Ϊ=TIMxCLK/(TIM_Prescaler+1)=1MHz
  TIM_TimeBaseStructure.TIM_Prescaler = 180-1;	
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  // ������ʽ
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;	
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	// ��ʼ����ʱ��TIMx, x[1,8]
	TIM_TimeBaseInit(TIM1_TIM, &TIM_TimeBaseStructure);	
	
	
	TIM_TimeBaseStructure.TIM_Prescaler = 90-1;
	// ��ʼ����ʱ��TIMx, x[1,8]
	TIM_TimeBaseInit(TIM3_TIM, &TIM_TimeBaseStructure);
	/* �߼���ʱ��IC1���������ش��� TI1FP1 */
  TIM_ICInitStructure.TIM_Channel = TIM1_IC1PWM_CHANNEL;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
  TIM_PWMIConfig(TIM1_TIM, &TIM_ICInitStructure);
	/* �߼���ʱ��IC2�����½��ش��� TI1FP2 */	
	TIM_ICInitStructure.TIM_Channel = TIM1_IC2PWM_CHANNEL;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_IndirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
  TIM_PWMIConfig(TIM1_TIM, &TIM_ICInitStructure);
	/* ͨ�ö�ʱ��IC1���������ش��� TI1FP1 */
  TIM_ICInitStructure.TIM_Channel = TIM3_IC1PWM_CHANNEL;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
  TIM_PWMIConfig(TIM3_TIM, &TIM_ICInitStructure);
	/* ͨ�ö�ʱ��IC2�����½��ش��� TI1FP2 */	
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
  /* ʹ�ܸ߼����ƶ�ʱ�� */
  TIM_Cmd(TIM1_TIM, ENABLE);
	/* ʹ��ͨ�ÿ��ƶ�ʱ�� */
  TIM_Cmd(TIM3_TIM, ENABLE);
  /* ʹ�ܸ߼���ʱ������/�Ƚ�2�ж����� */
  TIM_ITConfig(TIM1_TIM, TIM_IT_CC1, ENABLE);
	/* ʹ��ͨ�ö�ʱ������/�Ƚ�2�ж����� */
  TIM_ITConfig(TIM3_TIM, TIM_IT_CC1, ENABLE);
}


/**
  * @brief  ��ʼ���߼����ƶ�ʱ����ʱ��1ms����һ���ж�
  * @param  ��
  * @retval ��
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
		Value_3 = TIM_GetCapture1(TIM1_TIM);//Ƶ��
		Value_1 = TIM_GetCapture2(TIM1_TIM);//ռ�ձ�
		if(Value_3 != 0)
		{
			Duty = (Value_1 * 100) / Value_3;
			if((Duty<=87.5)&&(Duty>=12.5))//���ռ�ձ��Ƿ���12.5-87.5֮��,�Ӷ�����Ӳ���Ĳ��ȶ���
			{
				macLED2_TOGGLE();
				Angle_1 = (Duty-12.5)*360.0/(87.5-12.5);
			}
		}
	/* �����ʱ������/�Ƚ�1�ж� */
	TIM_ClearITPendingBit(TIM1_TIM, TIM_IT_CC1);	
	}
}
//��ʱ���жϵ�������Ǹ���ռ�ձ�
void  TIM3_TIM_IRQHandler (void)
{
	double Duty = 0;	
	uint16_t Value_3 = 0,Value_1 = 0;
	if(TIM_GetITStatus(TIM3_TIM,TIM_IT_CC1)!= RESET)
	{
		Value_3 = TIM_GetCapture1(TIM3_TIM);//Ƶ��
		Value_1 = TIM_GetCapture2(TIM3_TIM);//ռ�ձ�
		if(Value_3 != 0)
		{
			Duty = (Value_1 * 100) / Value_3;
			if((Duty<=87.5)&&(Duty>=12.5))//���ռ�ձ��Ƿ���12.5-87.5֮��,�Ӷ�����Ӳ���Ĳ��ȶ���
			{
				//macLED1_TOGGLE();������Ƶ��
				Angle_2 = (Duty-12.5)*360.0/(87.5-12.5);
			}
			
		}
	/* �����ʱ������/�Ƚ�1�ж� */
	TIM_ClearITPendingBit(TIM3_TIM, TIM_IT_CC1);	
	}
}
/*
@brief:�Ƕȴ�����
@param:С�ݣ�A1���ʹ���֣�A2�����Ӷ��֣�����Ӧ��λ�ã�
@return:0-�ɹ����½Ƕȣ�1-����ʧ��
*/
int8_t AngleTransducer(double A1,double A2,double *Angle)
{
	static double Angle_Last = 0,Angle_final = 0;
	double Delt_Angle = 0,Angle_Now = 0;
	
	//�������������ĺϷ���
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

//�������Ϊdouble��DutyCycle_A1��DutyCycle_A2
//���ص�ǰ�Ƕ�ֵ
//double AngleTransducerInit(void)
//{
//	double A1_Angle[10] = {0},A2_Angle[10] = {0},A1_sum=0,A2_sum=0,A1_agv=0,A2_agv=0;
//	double AngleDelt = 0,Angle_final = 0;
//	uint8_t i=0,n=0;
//	//�ֱ��ȡ10�δ�С�����������ĽǶ�ֵ
//	for(i=0;i<10;i++)
//	{
//		A1_Angle[i] = (DutyCycle_A1-12.5)*360/(87.5-12.5);//ռ�ձ�ת���ɽǶ�
//		A2_Angle[i] = (DutyCycle_A2-12.5)*360/(87.5-12.5);
//		delay_ms(10);
//	}
//	//������޳����ֵ����Сֵ������ƽ��ֵ
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
//	//ͨ���ж�A1-A2�������������ж�Ȧ��
//	if(((AngleDelt>=0)&&(AngleDelt<=48))||((AngleDelt>=-312)&&(AngleDelt<=-304.616)))//A1ת����Ȧ��Ϊ 0
//	{
//		n=0;
//	}
//	else if(((AngleDelt>=55.385)&&(AngleDelt<=96))||((AngleDelt>=-264)&&(AngleDelt<=-249.231)))//A1ת����Ȧ��Ϊ 1
//	{
//		n=1;
//	}
//	else if(((AngleDelt>=110.770)&&(AngleDelt<=144.000))||((AngleDelt>=-216.000)&&(AngleDelt<=-193.846)))//A1ת����Ȧ��Ϊ 2
//	{
//		n=2;
//	}
//	else if(((AngleDelt>=166.154)&&(AngleDelt<=192.000))||((AngleDelt>=-168.000)&&(AngleDelt<=-138.462)))//A1ת����Ȧ��Ϊ 3
//	{
//		n=3;
//	}
//	else if(((AngleDelt>=221.539)&&(AngleDelt<=240))||((AngleDelt>=-120)&&(AngleDelt<=-83.077)))//A1ת����Ȧ��Ϊ 4
//	{
//		n=4;
//	}
//	else if(((AngleDelt>=276.923)&&(AngleDelt<=288))||((AngleDelt>=-72)&&(AngleDelt<=-27.692)))//A1ת����Ȧ��Ϊ 5
//	{
//		n=5;
//	}
//	else if(((AngleDelt>=332.308)&&(AngleDelt<=336))||((AngleDelt>=-24)&&(AngleDelt<=-24)))//A1ת����Ȧ��Ϊ 6
//	{
//		n=6;
//	}
//	else if(((AngleDelt>=-336)&&(AngleDelt<=-332.308))||((AngleDelt>=27.692)&&(AngleDelt<=53.24)))//A1ת����Ȧ��Ϊ 7
//	{
//		n=7;
//	}
//	return Angle_final = (n*360+A1_agv)*30/62-630.00;//������ࣨ-630�ȣ�Ϊ0����Ŀ���Ϊ630
//}

/*********************************************END OF FILE**********************/













