/**
  ******************************************************************************
  * @file    Project/STM32F0xx_StdPeriph_Templates/stm32f0xx_it.c 
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    05-December-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_it.h"

#include "jiffy.h"
#include "cpu.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern void(*hooks[])(void);
extern void(*hooks_redundance[])(void);

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
    if((hooks[1] != 0) && (hooks[1] == hooks_redundance[1]))
    {
        hooks[1]();
    }
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
    if((hooks[2] != 0) && (hooks[2] == hooks_redundance[2]))
    {
        hooks[2]();
    }
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    if(cpu.core.status() != CPU_POWERSAVE)
    {
        jitter_update(1);
    }
    
    if((hooks[0] != 0) && (hooks[0] == hooks_redundance[0]))
    {
        hooks[0]();
    }
}

/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 

/**
  * @brief  This function handles RTC Handler.
  * @param  None
  * @retval None
  */
void RTC_IRQHandler(void)
{
	if(RTC_GetITStatus(RTC_IT_WUT) != RESET)
	{
		/* Clear the Alarm A Pending Bit */
		RTC_ClearITPendingBit(RTC_IT_WUT);
	}
    
    /* Clear EXTI line20 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line20);
    
    if(cpu.core.status() == CPU_POWERSAVE)
    {
        jitter_update(1000);
    }
}

extern void VUART1_Recv_Handler(void);
extern void VUART1_Trans_Handler(void);
extern void VUART2_Recv_Handler(void);
extern void VUART2_Trans_Handler(void);
extern void VUART3_Recv_Handler(void);
extern void VUART3_Trans_Handler(void);
extern void VUART4_Recv_Handler(void);
extern void VUART4_Trans_Handler(void);

void USART3_8_IRQHandler(void)
{
	if(USART_GetITStatus(USART4, USART_IT_RXNE) != RESET)
	{
        if(USART_GetFlagStatus(USART4, USART_FLAG_PE) != RESET)
        {
            USART_ClearFlag(USART4, USART_FLAG_PE);
            USART_ReceiveData(USART4);
            return;
        }
        
        VUART1_Recv_Handler();
	}
    
	if(USART_GetITStatus(USART4, USART_IT_TC) != RESET)
	{
        VUART1_Trans_Handler();
        USART_ClearFlag(USART4, USART_FLAG_TC);
	}
    
	if(USART_GetITStatus(USART4, USART_IT_TXE) != RESET)
	{
		USART_ClearITPendingBit(USART4, USART_IT_TC);
	}
    
    USART_ClearITPendingBit(USART4, USART_IT_ORE|USART_IT_FE);
    
    
    
    
	if(USART_GetITStatus(USART5, USART_IT_RXNE) != RESET)
	{
        if(USART_GetFlagStatus(USART5, USART_FLAG_PE) != RESET)
        {
            USART_ClearFlag(USART5, USART_FLAG_PE);
            USART_ReceiveData(USART5);
            return;
        }
        
        VUART2_Recv_Handler();
	}
    
	if(USART_GetITStatus(USART5, USART_IT_TC) != RESET)
	{
        VUART2_Trans_Handler();
        USART_ClearFlag(USART5, USART_FLAG_TC);
	}
    
	if(USART_GetITStatus(USART5, USART_IT_TXE) != RESET)
	{
		USART_ClearITPendingBit(USART5, USART_IT_TC);
	}
    
    USART_ClearITPendingBit(USART5, USART_IT_ORE|USART_IT_FE);
    
    
    
    
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
        if(USART_GetFlagStatus(USART3, USART_FLAG_PE) != RESET)
        {
            USART_ClearFlag(USART3, USART_FLAG_PE);
            USART_ReceiveData(USART3);
            return;
        }
        
        VUART3_Recv_Handler();
	}
    
	if(USART_GetITStatus(USART3, USART_IT_TC) != RESET)
	{
        VUART3_Trans_Handler();
        USART_ClearFlag(USART3, USART_FLAG_TC);
	}
    
	if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
	{
		USART_ClearITPendingBit(USART3, USART_IT_TC);
	}
    
    USART_ClearITPendingBit(USART3, USART_IT_ORE|USART_IT_FE);
}

void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
        if(USART_GetFlagStatus(USART1, USART_FLAG_PE) != RESET)
        {
            USART_ClearFlag(USART1, USART_FLAG_PE);
            USART_ReceiveData(USART1);
            return;
        }
        
        VUART4_Recv_Handler();
	}
    
	if(USART_GetITStatus(USART1, USART_IT_TC) != RESET)
	{
        VUART4_Trans_Handler();
        USART_ClearFlag(USART1, USART_FLAG_TC);
	}
    
	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
	{
		USART_ClearITPendingBit(USART1, USART_IT_TC);
	}
    
    USART_ClearITPendingBit(USART1, USART_IT_ORE|USART_IT_FE);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
