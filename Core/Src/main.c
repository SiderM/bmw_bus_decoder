/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
#include "main.h"
#include "can.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
// #include "ibus.h"
#include "kbus.h"
#include "kbus_debug.h"
#include "can_debug.h"
// #include "decoder.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t Backlight = 0; // Глобальная переменная для хранения уровня подсветки (пример)
uint8_t Speed = 0;     // Глобальная переменная для хранения скорости автомобиля (пример)
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  if (huart->Instance == USART3)
  {
    KBUS_Parse(Size);
  }
}

void KBUS_RxMsgCallback(KBUS_MessageTypeDef *Msg)
{
  // Выводим каждое сообщение в отладочный интерфейс
  KBUS_Debug_PrintMsg(Msg);

  // Пример обработки команды изменения подсветки приборов
  if (Msg->Data[0] == KBUS_CMD_INSTRUMENT_BACKLIGHTING)
  {
    Backlight = Msg->Data[1];
  }

  // Пример запроса статуса зажигания, отправки ответа и логирования
  if (Msg->Data[0] == KBUS_CMD_IGNITION_REQUEST && Msg->Size >= 1)
  {
    uint8_t src = KBUS_DEV_IKE;
    uint8_t data[] = {KBUS_CMD_IGNITION, 0x03};
    KBUS_Send(src, KBUS_DEV_BROADCAST, data, 2);
    KBUS_Debug_Log(" [KBUS] >> Sent Ignition Status to %02X\r\n", KBUS_DEV_BROADCAST);
  }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  CAN_RxHeaderTypeDef RxHeader;
  uint8_t RxData[8];

  if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
  {
    Error_Handler();
  }

  CAN_Debug_PrintFrame(&RxHeader, RxData, RxHeader.DLC);

  // Пример обработки сообщений с определёнными ID
  if (RxHeader.StdId == 0x153) // Скорость
  {
    float speed_raw = ((uint16_t)RxData[2] << 8 | RxData[1]) >> 4;
    Speed = (uint8_t)(speed_raw * 1.125f);
  }

  // switch (RxHeader.StdId)
  // {
  // case 0x1F5: // Угол руля
  //   uint16_t word_a = (uint16_t)RxData[0] | ((uint16_t)RxData[1] << 8);
  //   uint16_t angle_raw = word_a & 0x7FFF;
  //   uint8_t angle_sign = (word_a >> 15) & 1;
  //   float angle_deg = (angle_sign ? -1.0f : 1.0f) * angle_raw * 0.045f;
  //   car.Steering_Angle = (int16_t)angle_deg;
  //   break;

  // case 0x316: // Обороты
  //   float rpm_raw = ((uint16_t)RxData[3] << 8) | RxData[2];
  //   car.RPM = (uint16_t)rpm_raw / 6.4f;
  //   break;

  // case 0x329: // Температура ОЖ
  //   car.Coolant_Temp = (int8_t)(0.75f * RxData[1] - 48.37f);
  //   break;

  // case 0x545: // Предупреждения
  //             // 0x02 - Check Engine
  //   if (RxData[0] & 0x02)
  //   {
  //     car.Warnings |= WARN_CHECK_ENGINE;
  //   }
  //   else
  //   {
  //     car.Warnings &= ~WARN_CHECK_ENGINE;
  //   }

  //   // 0x08 - Cruise Control
  //   if (RxData[0] & 0x08)
  //   {
  //     car.Warnings |= WARN_CRUISE;
  //   }
  //   else
  //   {
  //     car.Warnings &= ~WARN_CRUISE;
  //   }

  //   // 0x10 - EML
  //   if (RxData[0] & 0x10)
  //   {
  //     car.Warnings |= WARN_EML;
  //   }
  //   else
  //   {
  //     car.Warnings &= ~WARN_EML;
  //   }

  //   // 0x01 - Battery Charge
  //   if (RxData[5] & 0x01)
  //   {
  //     car.Warnings |= WARN_BATTERY;
  //   }
  //   else
  //   {
  //     car.Warnings &= ~WARN_BATTERY;
  //   }
  //   break;

  // case 0x613: // Топливо
  //   float percent;

  //   if (RxData[2] >= 0x80)
  //   {
  //     percent = (float)(RxData[2] - 0x80) / (float)(0x87 - 0x80) * 10.0f;
  //   }
  //   else
  //   {
  //     percent = 10.0f + (float)(RxData[2] - 0x08) / (float)(0x39 - 0x08) * 90.0f;
  //   }

  //   if (percent < 0.0f)
  //   {
  //     percent = 0.0f;
  //   }

  //   if (percent > 100.0f)
  //   {
  //     percent = 100.0f;
  //   }

  //   car.Fuel_Level = (uint8_t)percent;
  //   break;

  // case 0x615: // Температура на улице
  //   if (RxData[3] > 0x80)
  //   {
  //     car.Outside_Temp = -(int8_t)(RxData[3] - 128);
  //   }
  //   else
  //   {
  //     car.Outside_Temp = (int8_t)RxData[3];
  //   }
  //   break;
  // }
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN1_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  KBUS_Init(&huart3);
  KBUS_Debug_Init(&huart2);
  CAN_Debug_Init(&huart2);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    // IBus_Process();
    KBUS_Process();
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
   */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
