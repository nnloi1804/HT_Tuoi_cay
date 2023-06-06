
#include "main.h"
#include "liquidcrystal_i2c.h"
#include "ds1307_for_stm32_hal.h"
#include "stdio.h"
#include <string.h>
#include <math.h>


uint32_t adc_value;
uint8_t status;
char da1[5];
int dem= 0;
int gt;
int dem3 = 0;
int dem2 = 0;
int dem4=0;
char rain1[5];
int dem_mua = 0;


ADC_HandleTypeDef hadc1;
I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c3;

UART_HandleTypeDef huart2;


void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C3_Init(void);

void Realtime();
void Real_Hour_Minute();
void SetRelay();
void Check_Rain();
void Relay_Button();
void Do_am();
void Chip_On(int n);
void waning(int n);


int hour_on = 9;
int minute_on = 55;
int second_on = 60;


char h1[5];
char h2[5];
char zero = '0';
char char1 = '/';


int main(void)
{

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_I2C3_Init();
  HD44780_Init(2);
  DS1307_Init(&hi2c3);

  /*DS1307_SetTimeZone(+7, 00);
	DS1307_SetDate(19);
	DS1307_SetMonth(5);
	DS1307_SetYear(2023);
	DS1307_SetDayOfWeek(5);
	DS1307_SetHour(12);
	DS1307_SetMinute(57);
	DS1307_SetSecond(00); */

	HD44780_SetCursor(5,0);
	HD44780_PrintStr("NHOM 5");
	HAL_Delay(500);
	HD44780_SetCursor(1,1);
	HD44780_PrintStr("THUC TAP CO SO");
	HAL_Delay(1000);
	  HD44780_Clear();
  while (1)
  {
	  HAL_ADC_Start(&hadc1);
	  HAL_ADC_PollForConversion(&hadc1, 1000);
	  adc_value = (HAL_ADC_GetValue(&hadc1)*100)/4095;
	  HAL_ADC_Stop(&hadc1);
	  HAL_Delay(100);


	  	if (HAL_GPIO_ReadPin(BUTTON2_GPIO_Port,BUTTON2_Pin) == 1) {
	  		HD44780_Clear();
	  		Realtime();
	  		HAL_Delay(2000);
	  		HD44780_Clear();
	  	}


	  if (HAL_GPIO_ReadPin(RELAY_GPIO_Port, RELAY_Pin) == 0) {
			HD44780_SetCursor(10,1);
			HD44780_PrintStr("MB:OFF");
	  }	 else {
			HD44780_SetCursor(10,1);
			HD44780_PrintStr("MB:ON ");
	  }
	   Do_am();
	  Check_Rain();
	  Real_Hour_Minute();

  }

}

void Do_am()
{

	itoa(adc_value, da1, 10);
	HD44780_SetCursor(0,0);
	HD44780_PrintStr("DO AM:");
	HD44780_SetCursor(6,0);

	if (adc_value <100) {
		HD44780_PrintStr(da1);
		HD44780_SetCursor(8,0);
		HD44780_PrintStr("% ");
	}
	else {
		HD44780_PrintStr(da1);
		HD44780_SetCursor(9,0);
		HD44780_PrintStr("%");
		  	  }

	if (adc_value > 80) {

		while (adc_value > 80) {
  		HD44780_Clear();
  		HD44780_SetCursor(3,0);
  		HD44780_PrintStr("DO AM THAP");

  		 waning(3);

  		HD44780_SetCursor(1,1);
  		HD44780_PrintStr("ONL MAY BOM");
  		HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, 1);
  		HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 1);
  		for (int i = 5; i>=1; i--) {
  			char so[5];
  			itoa(i, so, 10);
  			if (i>10) {
  			HD44780_SetCursor(13,1);
  			HD44780_PrintStr(so);
  			HAL_Delay(1000);
  			HD44780_SetCursor(15,1);
  			HD44780_PrintStr("s");
  			} else {
  	  			HD44780_SetCursor(13,1);
  	  			HD44780_PrintStr(so);
  	  			HAL_Delay(1000);
  	  			HD44780_SetCursor(14,1);
  	  			HD44780_PrintStr("s ");
  			}
  		}


  		HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 0);
  		HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, 0);

  		break;
		}
		HD44780_Clear();
	}
}
void Check_Rain()
{

	uint8_t hour = DS1307_GetHour();
	uint8_t minute = DS1307_GetMinute();
	uint8_t second = DS1307_GetSecond();

	if (HAL_GPIO_ReadPin(RAIN_GPIO_Port, RAIN_Pin) == 0){
		HD44780_SetCursor(0,1);
		HD44780_PrintStr("CO MUA   ");
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 1);
		dem4=1;
		dem_mua = 1;
		dem=1;

		if (HAL_GPIO_ReadPin(RELAY_GPIO_Port, RELAY_Pin) == 1) {
			HD44780_Clear();
			HD44780_SetCursor(5,0);
			HD44780_PrintStr("MUA TOI");
			waning(2);
			HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, 0);
			HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 0);
			HD44780_SetCursor(3,1);
			HD44780_PrintStr("OFF MAY BOM");
			HAL_Delay(1000);
			HD44780_Clear();


		}

		if (HAL_GPIO_ReadPin(BUTTON1_GPIO_Port, BUTTON1_Pin) ==  1) {
			while (HAL_GPIO_ReadPin(BUTTON1_GPIO_Port, BUTTON1_Pin) ==  1) {}
			dem=0;
			HD44780_Clear();
			HD44780_SetCursor(4,0);
			HD44780_PrintStr("DANG MUA");
			HD44780_SetCursor(1,1);
			HD44780_PrintStr("KHONG THE TUOI");
			waning(2);
			HAL_Delay(1000);
  			HD44780_Clear();
  			dem=0;
		}


	} else {
		HD44780_SetCursor(0,1);
		HD44780_PrintStr("KHONG MUA");
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 0);
		if (dem_mua == 0) {
			SetRelay();
		}
		else {
			if (HAL_GPIO_ReadPin(BUTTON3_GPIO_Port, BUTTON3_Pin) ==  1) {
		  		HD44780_Clear();
				HD44780_SetCursor(1,0);
				HD44780_PrintStr("HOM NAY CO MUA");
				HD44780_SetCursor(3,1);
				HD44780_PrintStr("KHONG TUOI");
		  		HAL_Delay(2000);
		  		HD44780_Clear();
			}
		}

		Relay_Button();
		dem4= 0;
	}
	if (hour == 0 && minute == 0 && second == 0 ) {
		dem_mua = 0;
	}

}

void Relay_Button()
{
	if (HAL_GPIO_ReadPin(BUTTON1_GPIO_Port, BUTTON1_Pin) ==  1) {
		dem+=1;
		if (HAL_GPIO_ReadPin(RAIN_GPIO_Port, RAIN_Pin) == 0){
			dem=0;
		}
		if (dem % 2 != 0) {
			dem=1;
			HD44780_Clear();
			HD44780_SetCursor(3,0);
			HD44780_PrintStr("ON MAY BOM");

		} else {
			dem=2;
			HD44780_Clear();
			HD44780_SetCursor(3,0);
			HD44780_PrintStr("OFF MAY BOM");

		}
			status =~ status;
			Chip_On(1);
			HD44780_Clear();
			HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, status);
			if (HAL_GPIO_ReadPin(RELAY_GPIO_Port, RELAY_Pin) == 1) {
				dem3 = 1;
			} else dem3 =0;

			HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, status);
		}
}

void SetRelay()
{

	uint8_t hour = DS1307_GetHour();
	uint8_t minute = DS1307_GetMinute();
	int8_t second = DS1307_GetSecond();

	itoa(hour_on, h1,10);
	itoa(minute_on, h2,10);

	if (HAL_GPIO_ReadPin(BUTTON3_GPIO_Port, BUTTON3_Pin) ==  1) {
  		HD44780_Clear();
		HD44780_SetCursor(1,0);
		HD44780_PrintStr("HOM NAY KO MUA");
		HD44780_SetCursor(0,1);
		HD44780_PrintStr("TUOI LUC:");

		HD44780_SetCursor(10,1);
		if (hour_on < 10) {
			HD44780_PrintSpecialChar(zero);
			HD44780_SetCursor(11,1);
			HD44780_PrintStr(h1);
			} else HD44780_PrintStr(h1);
		HD44780_SetCursor(12,1);
		HD44780_PrintStr(":");
		HD44780_SetCursor(13,1);
		if (minute_on < 10) {
			HD44780_PrintSpecialChar(zero);
			HD44780_SetCursor(14,1);
			HD44780_PrintStr(h2);
		} else HD44780_PrintStr(h2);

  		HAL_Delay(2000);
  		HD44780_Clear();
	}
	if (hour == hour_on && minute >= minute_on	 && minute < minute_on+1 && second < second_on - 55) {

		dem2 = 1;
	}
	else {
		dem2=0;
	}
	if (dem2 == 1) {
		HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, 1);
		HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 1);

		if (HAL_GPIO_ReadPin(BUTTON1_GPIO_Port, BUTTON1_Pin) ==  1) {
			HD44780_Clear();
			HD44780_SetCursor(1,0);
			HD44780_PrintStr("DANG TUOI");
			Chip_On(2);
			HD44780_Clear();
		}

	} else {

		if (dem3 == 0) {
		HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, 0);
		HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 0);}
		else {
			HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, 1);
			HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 1);
		}
	}

	dem2 = 0;
}

void waning(int n)
{
	for (int i=1; i<=n; i++) {
			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 1);
			HAL_GPIO_WritePin(CHIP_GPIO_Port, CHIP_Pin, 1);
			HAL_Delay(500);
			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 0);
			HAL_GPIO_WritePin(CHIP_GPIO_Port, CHIP_Pin, 0);
			HAL_Delay(500);
	}
}

void Chip_On(int n)
{
	for (int i=1; i<=n; i++) {
			HAL_GPIO_WritePin(CHIP_GPIO_Port, CHIP_Pin, 1);
			HAL_Delay(500);
			HAL_GPIO_WritePin(CHIP_GPIO_Port, CHIP_Pin, 0);
			HAL_Delay(500);
	}
}

void Realtime()
{
	const char *DAYS_OF_WEEK[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
	uint8_t date = DS1307_GetDate();
	uint8_t month = DS1307_GetMonth();
	uint16_t year = DS1307_GetYear();
	uint8_t dow = DS1307_GetDayOfWeek();
	uint8_t hour = DS1307_GetHour();
	uint8_t minute = DS1307_GetMinute();
	char snum1[5]; //hour
	char snum2[5];// minute
	char snum4[5];// date
	char snum5[5];// month
	char snum6[5];// year

	itoa(hour, snum1, 10);
	itoa(minute, snum2, 10);
	itoa(date, snum4, 10);
	itoa(month, snum5, 10);
	itoa(year, snum6, 10);

	HD44780_SetCursor(11,0);
	if (hour < 10) {
		HD44780_PrintSpecialChar(zero);
		HD44780_SetCursor(12,0);
		HD44780_PrintStr(snum1);
		} else HD44780_PrintStr(snum1);
	HD44780_SetCursor(13,0);
	HD44780_PrintStr(":");
	HD44780_SetCursor(14,0);
	if (minute < 10) {
		HD44780_PrintSpecialChar(zero);
		HD44780_SetCursor(15,0);
		HD44780_PrintStr(snum2);
	} else HD44780_PrintStr(snum2);
	HD44780_SetCursor(0,0);
	HD44780_PrintStr(DAYS_OF_WEEK[dow]);

	HD44780_SetCursor(3,1);
		if (date < 10) {
			HD44780_PrintSpecialChar(zero);
			HD44780_SetCursor(4,1);
			HD44780_PrintStr(snum4);
		} else HD44780_PrintStr(snum4);

		HD44780_SetCursor(5,1);
		HD44780_PrintStr("/");

		HD44780_SetCursor(6,1);
		if (month < 10) {
			HD44780_PrintSpecialChar(zero);
			HD44780_SetCursor(7,1);
			HD44780_PrintStr(snum5);
		} else HD44780_PrintStr(snum5);

		HD44780_SetCursor(8,1);
		HD44780_PrintStr("/");

		HD44780_SetCursor(9,1);
		HD44780_PrintStr(snum6);
}

void Real_Hour_Minute()
{
	uint8_t hour = DS1307_GetHour();
	uint8_t minute = DS1307_GetMinute();
	char snum1[5];
	char snum2[5];
	char zero = '0';
	itoa(hour, snum1, 10);
	itoa(minute, snum2, 10);

	HD44780_SetCursor(11,0);
	if (hour < 10) {
		HD44780_PrintSpecialChar(zero);
		HD44780_SetCursor(12,0);
		HD44780_PrintStr(snum1);
		} else HD44780_PrintStr(snum1);
	HD44780_SetCursor(13,0);
	HD44780_PrintStr(":");
	HD44780_SetCursor(14,0);
	if (minute < 10) {
		HD44780_PrintSpecialChar(zero);
		HD44780_SetCursor(15,0);
		HD44780_PrintStr(snum2);
	} else HD44780_PrintStr(snum2);
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
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

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
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
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.ClockSpeed = 100000;
  hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, CHIP_Pin|LED3_Pin|LED1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, RELAY_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : BUTTON1_Pin BUTTON2_Pin BUTTON3_Pin */
  GPIO_InitStruct.Pin = BUTTON1_Pin|BUTTON2_Pin|BUTTON3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : CHIP_Pin LED3_Pin LED1_Pin */
  GPIO_InitStruct.Pin = CHIP_Pin|LED3_Pin|LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : RAIN_Pin */
  GPIO_InitStruct.Pin = RAIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(RAIN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : RELAY_Pin */
  GPIO_InitStruct.Pin = RELAY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(RELAY_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LED2_Pin */
  GPIO_InitStruct.Pin = LED2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(LED2_GPIO_Port, &GPIO_InitStruct);

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
