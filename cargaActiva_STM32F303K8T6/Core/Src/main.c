/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd_i2c_lfs.h"
#include "INA219.h"
#include "menu_cargaActiva.h"
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
int32_t lecturaEnc;
uint8_t flag_tim7 = 0;
uint8_t periodo_enc = 0;
uint8_t periodo_impMed = 0;
uint8_t lectura_boton = 1;
uint8_t last_boton = 1;
INA219_t ina219;
float corriente;
uint16_t vshunt;
uint16_t raw_c;
uint8_t rango_I = 0;
float factor_rango = 1.4615384615f;
uint16_t cal_dinamico [58] = {
		4059,
		4028,
		4018,
		4009,
		4010,
		4009,
		3999,
		3993,
		4002,
		4002,
		3998,
		3996,
		3993,
		3988,
		3981,
		3980,
		3977,
		3976,
		3971,
		3971,
		3967,
		3962,
		3958,
		3957,
		3951,
		3946,
		3937,
		3933,
		3929,
		3923,
		3917,
		3912,
		3906,
		3902,
		3893,
		3884,
		3877,
		3871,
		3865,
		3846,
		3842,
		3842,
		3843,
		3836,
		3830,
		3825,
		3820,
		3812,
		3806,
		3798,
		3790,
		3781,
		3773,
		3766,
		3758,
		3746,
		3735,
		3801,

};

float c2;
int16_t c3;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
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
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim7); //overflow: 10 ms
  HAL_TIM_Encoder_Start_IT(&htim2, TIM_CHANNEL_ALL);

  lcd_init(&hi2c1, 0x3F);

  INA219_Init(&ina219, &hi2c1, INA219_ADDRESS);

  start_menu ();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  corriente = INA219_ReadCurrent_float(&ina219);

	  c2 = corriente * 1000;
	  c3 = c2;

	  rango_I = corriente * factor_rango;
	  //rango_I = corriente * 20;

	  //if (!rango_I) rango_I++;

//	  INA219_setCalibration(&ina219, cal_dinamico[rango_I]);


	  switch (rango_I){
		  case 0:
			  INA219_setCalibration(&ina219, 4002);
		  break;
		  case 1:
			  INA219_setCalibration(&ina219, 3971);
		  break;
		  case 2:
			  INA219_setCalibration(&ina219, 3937);
		  break;
		  case 3:
			  INA219_setCalibration(&ina219, 3846);
		  break;
		  case 4:
			  INA219_setCalibration(&ina219, 3798);
		  break;
		  default:

		  break;
	  }

	  vshunt = INA219_ReadShuntVolage(&ina219);
	  raw_c = INA219_ReadCurrent_raw(&ina219);


//	  continue;
	  if (flag_tim7 != 0){
		  periodo_enc++;
		  periodo_impMed++;
		  flag_tim7 = 0;
	  } //end if flag_tim7

	  if (periodo_enc > 19){
		  lecturaEnc = __HAL_TIM_GET_COUNTER(&htim7);
		  __HAL_TIM_SET_COUNTER(&htim7, 0);
		  periodo_enc = 0;

		  lectura_boton = HAL_GPIO_ReadPin(IN_D4_sw_GPIO_Port, IN_D4_sw_Pin);

	  } //end if periodo_enc

	  check_menu();
	  last_boton = lectura_boton;

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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_TIM1
                              |RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12PLLCLK_DIV1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  PeriphClkInit.Tim1ClockSelection = RCC_TIM1CLK_HCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if (htim->Instance == TIM7){
		flag_tim7 = 1;
	}
} //end HAL_TIM_PeriodElapsedCallback()


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	lecturaEnc = __HAL_TIM_GET_COUNTER(htim);
}
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

#ifdef  USE_FULL_ASSERT
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
