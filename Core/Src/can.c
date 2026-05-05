/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    can.c
 * @brief   This file provides code for the configuration
 *          of the CAN instances.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "can.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

CAN_HandleTypeDef hcan1;

/* CAN1 init function */
void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */
  CAN_FilterTypeDef sFilterConfig;
  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 5;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_15TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */
  sFilterConfig.FilterBank = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  sFilterConfig.FilterActivation = ENABLE;
  sFilterConfig.SlaveStartFilterBank = 14;
  if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_CAN_Start(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE END CAN1_Init 2 */
}

void HAL_CAN_MspInit(CAN_HandleTypeDef *canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (canHandle->Instance == CAN1)
  {
    /* USER CODE BEGIN CAN1_MspInit 0 */

    /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**CAN1 GPIO Configuration
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
    /* USER CODE BEGIN CAN1_MspInit 1 */

    /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef *canHandle)
{

  if (canHandle->Instance == CAN1)
  {
    /* USER CODE BEGIN CAN1_MspDeInit 0 */

    /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN1 GPIO Configuration
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11 | GPIO_PIN_12);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
    /* USER CODE BEGIN CAN1_MspDeInit 1 */

    /* USER CODE END CAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  CAN_RxHeaderTypeDef RxHeader;
  uint8_t RxData[8];

  if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
  {
    Error_Handler();
  }

  switch (RxHeader.StdId)
  {

  case 0x153: // Скорость
    float speed_raw = ((uint16_t)RxData[2] << 8 | RxData[1]) >> 4;
    car.Speed = (uint8_t)speed_raw * 1.125f;
    break;

  case 0x1F5: // Угол руля
    uint16_t word_a = (uint16_t)RxData[0] | ((uint16_t)RxData[1] << 8);
    uint16_t angle_raw = word_a & 0x7FFF;
    uint8_t angle_sign = (word_a >> 15) & 1;
    float angle_deg = (angle_sign ? -1.0f : 1.0f) * angle_raw * 0.045f;
    car.Steering_Angle = (int16_t)angle_deg;
    break;

  case 0x316: // Обороты
    float rpm_raw = ((uint16_t)RxData[3] << 8) | RxData[2];
    car.RPM = (uint16_t)rpm_raw / 6.4f;
    break;

  case 0x329: // Температура ОЖ
    car.Coolant_Temp = (int8_t)(0.75f * RxData[1] - 48.37f);
    break;

  case 0x545: // Предупреждения
              // 0x02 - Check Engine
    if (RxData[0] & 0x02)
    {
      car.Warnings |= WARN_CHECK_ENGINE;
    }
    else
    {
      car.Warnings &= ~WARN_CHECK_ENGINE;
    }

    // 0x08 - Cruise Control
    if (RxData[0] & 0x08)
    {
      car.Warnings |= WARN_CRUISE;
    }
    else
    {
      car.Warnings &= ~WARN_CRUISE;
    }

    // 0x10 - EML
    if (RxData[0] & 0x10)
    {
      car.Warnings |= WARN_EML;
    }
    else
    {
      car.Warnings &= ~WARN_EML;
    }

    // 0x01 - Battery Charge
    if (RxData[5] & 0x01)
    {
      car.Warnings |= WARN_BATTERY;
    }
    else
    {
      car.Warnings &= ~WARN_BATTERY;
    }
    break;

  case 0x613: // Топливо
    float percent;

    if (RxData[2] >= 0x80)
    {
      percent = (float)(RxData[2] - 0x80) / (float)(0x87 - 0x80) * 10.0f;
    }
    else
    {
      percent = 10.0f + (float)(RxData[2] - 0x08) / (float)(0x39 - 0x08) * 90.0f;
    }

    if (percent < 0.0f)
    {
      percent = 0.0f;
    }

    if (percent > 100.0f)
    {
      percent = 100.0f;
    }

    car.Fuel_Level = (uint8_t)percent;
    break;

  case 0x615: // Температура на улице
    if (RxData[3] > 0x80)
    {
      car.Outside_Temp = -(int8_t)(RxData[3] - 128);
    }
    else
    {
      car.Outside_Temp = (int8_t)RxData[3];
    }
    break;
  }
}
/* USER CODE END 1 */
