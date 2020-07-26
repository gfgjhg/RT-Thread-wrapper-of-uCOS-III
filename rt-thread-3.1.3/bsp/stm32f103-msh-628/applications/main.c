/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-11-05     yangjie      First edition
 * 2020-07-14     Meco Man     implement uCOS-III Wrapper
 */

/*����ʹ��USART2*/
/*�����Լ���ʼ�������ϸ����չٷ�����������*/

#include <os.h>
#include <os_app_hooks.h>

/*�궨��*/
#define APP_TASK_START_STK_SIZE     150   /*��ʼ���� �����ջ��С*/
#define APP_TASK_START_PRIO         5     /*��ʼ���� �������ȼ�*/

/*�����ջ�Լ�TCB*/
ALIGN(RT_ALIGN_SIZE)
static CPU_STK AppTaskStartStk[APP_TASK_START_STK_SIZE];/*�����ջ*/
static OS_TCB AppTaskStartTCB;

/*��������*/
static void AppTaskStart(void *p_arg);
static void AppTaskCreate(void);

/*�պ���*/
#define BSP_Init()          do{ rt_kprintf("BSP init!\r\n"); }while(0)
#define BSP_LED_Toggle(x)   do{ rt_kprintf("LED Toggle!\r\n"); }while(0)


int main(void)
{
    OS_ERR err;
    
    OSInit(&err);                                   /*uCOS-III����ϵͳ��ʼ��*/
        
    /*������ʼ����*/
	OSTaskCreate((OS_TCB 	* )&AppTaskStartTCB,		
				 (CPU_CHAR	* )"App Task Start", 		
                 (OS_TASK_PTR )AppTaskStart, 			
                 (void		* )0,			
                 (OS_PRIO	  )APP_TASK_START_PRIO,     
                 (CPU_STK   * )&AppTaskStartStk[0],	
                 (CPU_STK_SIZE)APP_TASK_START_STK_SIZE/10,
                 (CPU_STK_SIZE)APP_TASK_START_STK_SIZE,	
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);    
                 
    OSStart(&err);                                  /*��ʼ����uCOS-III����ϵͳ*/
}


/*��ʼ����*/
static void AppTaskStart(void *p_arg)
{
    OS_ERR err;
    
    (void)&p_arg;
    
    BSP_Init();
    CPU_Init();
    
#if OS_CFG_APP_HOOKS_EN > 0u
    App_OS_SetAllHooks();                           /*���ù��Ӻ���*/
#endif  
    
#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);  	                /*ͳ������*/    
    OSStatReset(&err);                              /*��λͳ������*/    
#endif
    
    AppTaskCreate();                                /*��������*/
    
    while(DEF_TRUE)
    {
        BSP_LED_Toggle(1);
        OSTimeDlyHMSM(0,0,1,0,
                      OS_OPT_TIME_HMSM_NON_STRICT,
                      &err);
    }
}


/*ʾ������*/
void timer_sample (void);
void mutex_sample (void);
void sem_sample   (void);
void q_sample     (void);

static void AppTaskCreate(void)
{
    timer_sample();
    mutex_sample();
    sem_sample();
    q_sample();
}