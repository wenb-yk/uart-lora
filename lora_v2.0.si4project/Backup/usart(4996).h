/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

#define USER_UART_BUFF_SIZE	 512
#define USER_UART_NUM		 1

typedef struct USER_UART_PARAMETER
{
	uint8_t 	tx_buff[USER_UART_BUFF_SIZE];
	uint8_t 	rx_buff[USER_UART_BUFF_SIZE];
	union 
	{
		uint8_t byte;
		struct
		{
			uint8_t rx_down : 1;
			uint8_t tx_down : 1;
		}bits;
	}flag;
	uint16_t 	rx_len;
	uint16_t 	tx_len;
}User_Uart_Parameter;

extern User_Uart_Parameter user_param_uart[USER_UART_NUM];

/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
