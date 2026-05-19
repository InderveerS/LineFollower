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
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "motors.h"
#include "data_logging.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
ADC_HandleTypeDef hadc1;
SPI_HandleTypeDef hspi2;
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim6;
UART_HandleTypeDef huart3;
/* USER CODE BEGIN PV */
/* USER CODE END PV */
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_SPI2_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM5_Init(void);
static void MX_TIM6_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */
/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
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
 MX_ADC1_Init();
 MX_SPI2_Init();
 MX_TIM1_Init();
 MX_TIM3_Init();
 MX_TIM4_Init();
 MX_TIM5_Init();
 MX_TIM6_Init();
 MX_USART3_UART_Init();
 /* USER CODE BEGIN 2 */
 TIM1->ARR = 8399;
 TIM1->EGR = TIM_EGR_UG;
 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
 HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
 HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
 HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
 HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
 HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
 HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
 logging_init();
 spinPercent(70);
 /* USER CODE END 2 */
 /* Infinite loop */
 /* USER CODE BEGIN WHILE */
 while (1)
 {
   uint8_t rx_char;
   if (HAL_UART_Receive(&huart3, &rx_char, 1, 0) == HAL_OK)
   {
     switch (rx_char)
     {
       case 'S':
       case 's':
         logging_start();
         break;
       case 'D':
       case 'd':
         logging_dump_csv();
         break;
       case 'E':
       case 'e':
         logging_erase();
         break;
       default:
         break;
     }
   }
   /* USER CODE END WHILE */
   /* USER CODE BEGIN 3 */
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
 __HAL_RCC_PWR_CLK_ENABLE();
 __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
 RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
 RCC_OscInitStruct.HSIState = RCC_HSI_ON;
 RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
 RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
 RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
 RCC_OscInitStruct.PLL.PLLM = 8;
 RCC_OscInitStruct.PLL.PLLN = 168;
 RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
 RCC_OscInitStruct.PLL.PLLQ = 4;
 if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
 {
   Error_Handler();
 }
 RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                             |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
 RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
 RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
 RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
 RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
 if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
 {
   Error_Handler();
 }
}
/**
 * @brief ADC1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC1_Init(void)
{
 /* USER CODE BEGIN ADC1_Init 0 */
 /* USER CODE END ADC1_Init 0 */
 ADC_ChannelConfTypeDef sConfig = {0};
 /* USER CODE BEGIN ADC1_Init 1 */
 /* USER CODE END ADC1_Init 1 */
 hadc1.Instance = ADC1;
 hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
 hadc1.Init.Resolution = ADC_RESOLUTION_12B;
 hadc1.Init.ScanConvMode = DISABLE;
 hadc1.Init.ContinuousConvMode = DISABLE;
 hadc1.Init.DiscontinuousConvMode = DISABLE;
 hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
 hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
 hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
 hadc1.Init.NbrOfConversion = 1;
 hadc1.Init.DMAContinuousRequests = DISABLE;
 hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
 if (HAL_ADC_Init(&hadc1) != HAL_OK)
 {
   Error_Handler();
 }
 sConfig.Channel = ADC_CHANNEL_10;
 sConfig.Rank = 1;
 sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
 if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
 {
   Error_Handler();
 }
 /* USER CODE BEGIN ADC1_Init 2 */
 /* USER CODE END ADC1_Init 2 */
}
/**
 * @brief SPI2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI2_Init(void)
{
 /* USER CODE BEGIN SPI2_Init 0 */
 /* USER CODE END SPI2_Init 0 */
 /* USER CODE BEGIN SPI2_Init 1 */
 /* USER CODE END SPI2_Init 1 */
 hspi2.Instance = SPI2;
 hspi2.Init.Mode = SPI_MODE_MASTER;
 hspi2.Init.Direction = SPI_DIRECTION_2LINES;
 hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
 hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
 hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
 hspi2.Init.NSS = SPI_NSS_SOFT;
 hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
 hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
 hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
 hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
 hspi2.Init.CRCPolynomial = 10;
 if (HAL_SPI_Init(&hspi2) != HAL_OK)
 {
   Error_Handler();
 }
 /* USER CODE BEGIN SPI2_Init 2 */
 /* USER CODE END SPI2_Init 2 */
}
/**
 * @brief TIM1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM1_Init(void)
{
 /* USER CODE BEGIN TIM1_Init 0 */
 /* USER CODE END TIM1_Init 0 */
 TIM_ClockConfigTypeDef sClockSourceConfig = {0};
 TIM_MasterConfigTypeDef sMasterConfig = {0};
 TIM_OC_InitTypeDef sConfigOC = {0};
 TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};
 /* USER CODE BEGIN TIM1_Init 1 */
 /* USER CODE END TIM1_Init 1 */
 htim1.Instance = TIM1;
 htim1.Init.Prescaler = 0;
 htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
 htim1.Init.Period = 8399;
 htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
 htim1.Init.RepetitionCounter = 0;
 htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
 if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
 {
   Error_Handler();
 }
 sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
 if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
 {
   Error_Handler();
 }
 if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
 {
   Error_Handler();
 }
 sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
 sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
 if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
 {
   Error_Handler();
 }
 sConfigOC.OCMode = TIM_OCMODE_PWM1;
 sConfigOC.Pulse = 0;
 sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
 sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
 sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
 sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
 sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
 if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
 {
   Error_Handler();
 }
 if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
 {
   Error_Handler();
 }
 if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
 {
   Error_Handler();
 }
 if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
 {
   Error_Handler();
 }
 sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
 sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
 sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
 sBreakDeadTimeConfig.DeadTime = 0;
 sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
 sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
 sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
 if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
 {
   Error_Handler();
 }
 /* USER CODE BEGIN TIM1_Init 2 */
 /* USER CODE END TIM1_Init 2 */
 HAL_TIM_MspPostInit(&htim1);
}
/**
 * @brief TIM3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM3_Init(void)
{
 /* USER CODE BEGIN TIM3_Init 0 */
 /* USER CODE END TIM3_Init 0 */
 TIM_Encoder_InitTypeDef sConfig = {0};
 TIM_MasterConfigTypeDef sMasterConfig = {0};
 /* USER CODE BEGIN TIM3_Init 1 */
 /* USER CODE END TIM3_Init 1 */
 htim3.Instance = TIM3;
 htim3.Init.Prescaler = 0;
 htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
 htim3.Init.Period = 65535;
 htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
 htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
 sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
 sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
 sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
 sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
 sConfig.IC1Filter = 0;
 sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
 sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
 sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
 sConfig.IC2Filter = 0;
 if (HAL_TIM_Encoder_Init(&htim3, &sConfig) != HAL_OK)
 {
   Error_Handler();
 }
 sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
 sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
 if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
 {
   Error_Handler();
 }
 /* USER CODE BEGIN TIM3_Init 2 */
 /* USER CODE END TIM3_Init 2 */
}
static void MX_TIM4_Init(void)
{
 /* USER CODE BEGIN TIM4_Init 0 */
 /* USER CODE END TIM4_Init 0 */
 TIM_Encoder_InitTypeDef sConfig = {0};
 TIM_MasterConfigTypeDef sMasterConfig = {0};
 /* USER CODE BEGIN TIM4_Init 1 */
 /* USER CODE END TIM4_Init 1 */
 htim4.Instance = TIM4;
 htim4.Init.Prescaler = 0;
 htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
 htim4.Init.Period = 65535;
 htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
 htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
 sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
 sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
 sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
 sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
 sConfig.IC1Filter = 0;
 sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
 sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
 sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
 sConfig.IC2Filter = 0;
 if (HAL_TIM_Encoder_Init(&htim4, &sConfig) != HAL_OK)
 {
   Error_Handler();
 }
 sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
 sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
 if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
 {
   Error_Handler();
 }
 /* USER CODE BEGIN TIM4_Init 2 */
 /* USER CODE END TIM4_Init 2 */
}
/**
 * @brief TIM5 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM5_Init(void)
{
 /* USER CODE BEGIN TIM5_Init 0 */
 /* USER CODE END TIM5_Init 0 */
 TIM_ClockConfigTypeDef sClockSourceConfig = {0};
 TIM_MasterConfigTypeDef sMasterConfig = {0};
 TIM_OC_InitTypeDef sConfigOC = {0};
 /* USER CODE BEGIN TIM5_Init 1 */
 /* USER CODE END TIM5_Init 1 */
 htim5.Instance = TIM5;
 htim5.Init.Prescaler = 0;
 htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
 htim5.Init.Period = 4294967295;
 htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
 htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
 if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
 {
   Error_Handler();
 }
 sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
 if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
 {
   Error_Handler();
 }
 if (HAL_TIM_PWM_Init(&htim5) != HAL_OK)
 {
   Error_Handler();
 }
 sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
 sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
 if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
 {
   Error_Handler();
 }
 sConfigOC.OCMode = TIM_OCMODE_PWM1;
 sConfigOC.Pulse = 0;
 sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
 sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
 if (HAL_TIM_PWM_ConfigChannel(&htim5, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
 {
   Error_Handler();
 }
 /* USER CODE BEGIN TIM5_Init 2 */
 /* USER CODE END TIM5_Init 2 */
 HAL_TIM_MspPostInit(&htim5);
}
/**
 * @brief TIM6 Initialization Function (logging timer - fires every 10ms)
 * @param None
 * @retval None
 */
static void MX_TIM6_Init(void)
{
 /* USER CODE BEGIN TIM6_Init 0 */
	__HAL_RCC_TIM6_CLK_ENABLE();
 /* USER CODE END TIM6_Init 0 */
 /* USER CODE BEGIN TIM6_Init 1 */
 /* USER CODE END TIM6_Init 1 */
 htim6.Instance = TIM6;
 htim6.Init.Prescaler = 8399;
 htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
 htim6.Init.Period = 99;
 htim6.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
 htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
 if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
 {
   Error_Handler();
 }
 /* USER CODE BEGIN TIM6_Init 2 */
  HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 0,0);
  HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
 /* USER CODE END TIM6_Init 2 */


}
/**
 * @brief USART3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART3_UART_Init(void)
{
 /* USER CODE BEGIN USART3_Init 0 */
 /* USER CODE END USART3_Init 0 */
 /* USER CODE BEGIN USART3_Init 1 */
 /* USER CODE END USART3_Init 1 */
 huart3.Instance = USART3;
 huart3.Init.BaudRate = 115200;
 huart3.Init.WordLength = UART_WORDLENGTH_8B;
 huart3.Init.StopBits = UART_STOPBITS_1;
 huart3.Init.Parity = UART_PARITY_NONE;
 huart3.Init.Mode = UART_MODE_TX_RX;
 huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
 huart3.Init.OverSampling = UART_OVERSAMPLING_16;
 if (HAL_UART_Init(&huart3) != HAL_OK)
 {
   Error_Handler();
 }
 /* USER CODE BEGIN USART3_Init 2 */
 /* USER CODE END USART3_Init 2 */
}
/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
 GPIO_InitTypeDef GPIO_InitStruct = {0};
 /* USER CODE BEGIN MX_GPIO_Init_1 */
 /* USER CODE END MX_GPIO_Init_1 */
 __HAL_RCC_GPIOC_CLK_ENABLE();
 __HAL_RCC_GPIOH_CLK_ENABLE();
 __HAL_RCC_GPIOA_CLK_ENABLE();
 __HAL_RCC_GPIOB_CLK_ENABLE();
 __HAL_RCC_GPIOD_CLK_ENABLE();
 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_6
                         |GPIO_PIN_9|GPIO_PIN_12, GPIO_PIN_RESET);
 HAL_GPIO_WritePin(GPIOH, GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_RESET);
 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12|GPIO_PIN_3|GPIO_PIN_4, GPIO_PIN_RESET);
 HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
 GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_6
                         |GPIO_PIN_9|GPIO_PIN_12;
 GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
 HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
 GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
 GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
 HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
 GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_3|GPIO_PIN_4;
 GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
 HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
 GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8;
 GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
 GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_15;
 GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
 GPIO_InitStruct.Pin = GPIO_PIN_2;
 GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
 HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
 GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
 GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
 /* USER CODE BEGIN MX_GPIO_Init_2 */
 /* USER CODE END MX_GPIO_Init_2 */
}
/* USER CODE BEGIN 4 */
/**
 * @brief  Timer interrupt callback - fires every 10ms when TIM6 overflows
 * @param  htim: pointer to a TIM_HandleTypeDef structure
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
   if (htim->Instance == TIM6)
   {
       if (logging_active)
       {
           logging_record();
           log_time_ms += 10;
           if (log_time_ms >= 30000)
           {
               logging_stop();
           }
       }
   }
}



/* USER CODE END 4 */
/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
 /* USER CODE BEGIN Error_Handler_Debug */
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
 * @param  line: pointer to the line number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
 /* USER CODE BEGIN 6 */
 /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */


