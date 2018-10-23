/*----------------------------------------------------------------------------
 * Copyright (c) <2016-2018>, <Huawei Technologies Co., Ltd>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
 * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
 * applicable export control laws and regulations.
 *---------------------------------------------------------------------------*/

#include "los_base.h"
#include "los_task.ph"
#include "los_typedef.h"
#include "los_sys.h"

#include "los_mpu.h"
#include "los_hw.h"

UINT32 g_TskHandle;

VOID HardWare_Init(VOID)
{
}

volatile unsigned long long c111;
volatile unsigned long long c222;

void task1 (void)
{
    while (1)
        c111++;
}

void task2 (void)
{
    //volatile int * p = 0x20000004;

    LOS_DO_PRIVILEDGED (LOS_TaskDelay (1));

    //LOS_DO_PRIVILEDGED ((*p)++);

    while (1)
    {
        malloc (1);
#if 1
        LOS_DO_PRIVILEDGED(c222++);
#else
        c222++;
#endif
    }
}

#if (LOSCFG_STATIC_TASK == YES)
LOS_TASK_DEF (t1, "t1", task1, 0, 1, 0x130, 0, NULL);
static const LOS_MPU_PARA mpuPara [MPU_NR_USR_ENTRIES] = {0,};
LOS_TASK_DEF (t2, "t2", task2, 0, 1, 0x130, 0x120, (VOID*)mpuPara);

UINT32 t1, t2;
#endif

UINT32 creat_main_task()
{
    UINT32 uwRet = LOS_OK;

#if LOSCFG_ENABLE_MPU == YES
    LOS_MPU_PARA mpuPara [MPU_NR_USR_ENTRIES];
#endif

#if (LOSCFG_STATIC_TASK == NO)
    TSK_INIT_PARAM_S task_init_param;

    task_init_param.usTaskPrio = 1;
    task_init_param.pcName = "t1";
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)task1;
    task_init_param.uwStackSize = 0x130;

    uwRet = LOS_TaskCreate(&g_TskHandle, &task_init_param);
    if(LOS_OK != uwRet)
    {
        return uwRet;
    }

    task_init_param.usTaskPrio = 1;
    task_init_param.pcName = "t2";
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)task2;

#if LOSCFG_ENABLE_MPU == YES
#if 1
    memset (&mpuPara, 0, sizeof (mpuPara));

#if 0
    mpuPara [0].uwRegionAddr = ((UINT32)(&c222) & ~0xff);
    mpuPara [0].uwRegionSize = 256;
    mpuPara[0].uwRegionAttr = MPU_ATTR_RW_RW | MPU_ATTR_WB | MPU_ATTR_EN;
#endif
    task_init_param.uwHeapSize = 0x123;
    task_init_param.pRegions = mpuPara;
#endif
#endif

    uwRet = LOS_TaskCreate(&g_TskHandle, &task_init_param);
    if(LOS_OK != uwRet)
    {
        return uwRet;
    }

    return uwRet;
#else
    LOS_TASK_INIT(t1, &t1);
    LOS_TASK_INIT(t2, &t2);

    return 0;
#endif
}


int main(void)
{
    UINT32 uwRet = LOS_OK;
    HardWare_Init();

    uwRet = LOS_KernelInit();
    if (uwRet != LOS_OK)
    {
        return LOS_NOK;
    }

    creat_main_task();

    (void)LOS_Start();
    return 0;
}
