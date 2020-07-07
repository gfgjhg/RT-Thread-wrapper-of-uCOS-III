/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-08     Meco Man     the first verion
 */
 
/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2012; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                                  STATISTICS MODULE
*
* File    : OS_STAT.C
* By      : JJL
* Version : V3.03.00
*
* LICENSING TERMS:
* ---------------
*           uC/OS-III is provided in source form for FREE short-term evaluation, for educational use or 
*           for peaceful research.  If you plan or intend to use uC/OS-III in a commercial application/
*           product then, you need to contact Micrium to properly license uC/OS-III for its use in your 
*           application/product.   We provide ALL the source code for your convenience and to help you 
*           experience uC/OS-III.  The fact that the source is provided does NOT mean that you can use 
*           it commercially without paying a licensing fee.
*
*           Knowledge of the source code may NOT be used to develop a similar product.
*
*           Please help us continue to provide the embedded community with the finest software available.
*           Your honesty is greatly appreciated.
*
*           You can contact us at www.micrium.com, or by phone at +1 (954) 217-2036.
************************************************************************************************************************
*/

#include <os.h>

#if OS_CFG_STAT_TASK_EN > 0u

#define  OS_CFG_TASK_STK_LIMIT_PCT_EMPTY  10u               /* Stack limit position in percentage to empty            */
#define  OS_CFG_STAT_TASK_STK_LIMIT      ((OS_CFG_STAT_TASK_STK_SIZE  * OS_CFG_TASK_STK_LIMIT_PCT_EMPTY) / 100u)

static CPU_STK        OSCfg_StatTaskStk   [OS_CFG_STAT_TASK_STK_SIZE];

/*
************************************************************************************************************************
*                                                   RESET STATISTICS
*
* Description: This function is called by your application to reset the statistics.
*
* Argument(s): p_err      is a pointer to a variable that will contain an error code returned by this function.
*
*                             OS_ERR_NONE
*
* Returns    : none
************************************************************************************************************************
*/

void  OSStatReset (OS_ERR  *p_err)
{
}

/*
************************************************************************************************************************
*                                                DETERMINE THE CPU CAPACITY
*
* Description: This function is called by your application to establish CPU usage by first determining how high a 32-bit
*              counter would count to in 1/10 second if no other tasks were to execute during that time.  CPU usage is
*              then determined by a low priority task which keeps track of this 32-bit counter every second but this
*              time, with other tasks running.  CPU usage is determined by:
*
*                                             OS_Stat_IdleCtr
*                 CPU Usage (%) = 100 * (1 - ------------------)
*                                            OS_Stat_IdleCtrMax
*
* Argument(s): p_err      is a pointer to a variable that will contain an error code returned by this function.
*
*                             OS_ERR_NONE
*
* Returns    : none
************************************************************************************************************************
*/

void  OSStatTaskCPUUsageInit (OS_ERR  *p_err)
{
}

/*
************************************************************************************************************************
*                                                    STATISTICS TASK
*
* Description: This task is internal to uC/OS-III and is used to compute some statistics about the multitasking
*              environment.  Specifically, OS_StatTask() computes the CPU usage.  CPU usage is determined by:
*
*                                                   OSStatTaskCtr
*                 OSStatTaskCPUUsage = 100 * (1 - ------------------)     (units are in %)
*                                                  OSStatTaskCtrMax
*
* Arguments  : p_arg     this pointer is not used at this time.
*
* Returns    : none
*
* Note(s)    : 1) This task runs at a priority level higher than the idle task.
*
*              2) You can disable this task by setting the configuration #define OS_CFG_STAT_TASK_EN to 0.
*
*              3) You MUST have at least a delay of 2/10 seconds to allow for the system to establish the maximum value
*                 for the idle counter.
*
*              4) This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_StatTask (void  *p_arg)
{
    OS_ERR err;
    
    while(1)
    {

        OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);
    }
}

/*
************************************************************************************************************************
*                                              INITIALIZE THE STATISTICS
*
* Description: This function is called by OSInit() to initialize the statistic task.
*
* Argument(s): p_err     is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_STK_INVALID       If you specified a NULL stack pointer during configuration
*                            OS_ERR_STK_SIZE_INVALID  If you didn't specify a large enough stack.
*                            OS_ERR_PRIO_INVALID      If you specified a priority for the statistic task equal to or
*                                                     lower (i.e. higher number) than the idle task.
*                            OS_ERR_xxx               An error code returned by OSTaskCreate()
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_StatTaskInit (OS_ERR  *p_err)
{
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

    OSStatTaskCtr    = (OS_TICK)0;
    OSStatTaskCtrRun = (OS_TICK)0;
    OSStatTaskCtrMax = (OS_TICK)0;
    OSStatTaskRdy    = OS_STATE_NOT_RDY;                    /* Statistic task is not ready                            */
    OSStatResetFlag  = DEF_FALSE;

                                                            /* ---------------- CREATE THE STAT TASK ---------------- */
    if (OSCfg_StatTaskStk == (CPU_STK *)0) {
       *p_err = OS_ERR_STAT_STK_INVALID;
        return;
    }

    if (OS_CFG_STAT_TASK_STK_SIZE < OS_CFG_STK_SIZE_MIN) {
       *p_err = OS_ERR_STAT_STK_SIZE_INVALID;
        return;
    }

    if (OS_CFG_STAT_TASK_PRIO >= (OS_CFG_PRIO_MAX - 1u)) {
       *p_err = OS_ERR_STAT_PRIO_INVALID;
        return;
    }

    OSTaskCreate((OS_TCB     *)&OSStatTaskTCB,
                 (CPU_CHAR   *)((void *)"uC/OS-III Stat Task"),
                 (OS_TASK_PTR )OS_StatTask,
                 (void       *)0,
                 (OS_PRIO     )OS_CFG_STAT_TASK_PRIO,
                 (CPU_STK    *)OSCfg_StatTaskStk,
                 (CPU_STK_SIZE)OS_CFG_STAT_TASK_STK_LIMIT,
                 (CPU_STK_SIZE)OS_CFG_STAT_TASK_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK     )0,
                 (void       *)0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)p_err);
}

#endif
