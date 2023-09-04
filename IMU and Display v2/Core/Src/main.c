/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
	  /*
   * 	PINOUT
   * 	PA10 -> terminal TX
   * 	PA9	 -> terminal RX
   * 	PB9  -> IMU SDA
   * 	PB8  -> IMU SCL
   *
   * 	UART parameters
   * 	baud 115200
   **/
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "MPU9255.h"
#include "MATRIX.h"
#include "MICROS.h"

#include "st7735.h"
#include "st7735_test.h"
#include "stdbool.h"
#include "ctype.h"
#include "Common.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct 
{
	float x, y;
	int16_t r, c;
}balltype;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
DMA_HandleTypeDef hdma_i2c1_rx;

SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_tx;

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_tx;

/* USER CODE BEGIN PV */
float ACx, ACy, ACz, GYx, GYy, GYz, MAGx, MAGy, MAGz, accTotal, gyroTotal, magTotal;
uint8_t accGyroMagOverload;
const uint16_t PCazimuth = 345;
balltype ball, ballclean;

struct microsPeriod cyclePeriod = {0, 0}, 
										accGyroPeriod = {0, 0}; 
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM4_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */
void transmitBasis(float* floatArray, uint32_t time, uint8_t overload);

void initState(struct matrix *basis, struct matrix *gravity, struct matrix* north);
void restoreVertical(struct matrix* bas, struct matrix* grav, struct matrix* acc);
void initScreen();
void initBall();
void renewConcentricLines();
void moveBall(float x, float y, balltype *ball);	

float xMoveTo=0, yMoveTo=0;
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
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_TIM4_Init();
  MX_SPI1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
	microsInit(&htim4, 72000000);	// start micros timer
	
	// startup the sensor, assign pointers of ACC, GYRO, MAG and overload flag variables
	MPU9255setup(&hi2c1, &ACx, &ACy, &ACz, &GYx, &GYy, &GYz, &MAGx, &MAGy, &MAGz, &accGyroMagOverload);
	
	struct matrix accVec,	gyroVec, magVec, rotMatrix, basis, gravityVec, northVec, velocityVec, coordinate;
	
	matrixZeroes(&accVec , 3, 1);
	matrixZeroes(&gyroVec , 3, 1);
	matrixZeroes(&magVec , 3, 1);
	matrixZeroes(&rotMatrix , 3, 3);
	matrixEye(&basis, 3);
	matrixZeroes(&gravityVec, 3,1);
	matrixZeroes(&northVec, 3, 1);
	matrixZeroes(&velocityVec, 3,1);
	matrixZeroes(&coordinate, 3, 1);	
	
	matrixSE(&northVec, 0, 0, 1);
	matrixRotZ(&northVec, PCazimuth);
	// indication before the ongoing calibration. Sensor should be kept still!
	HAL_Delay(500);
	for(uint8_t v = 0; v<10; v++){
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		HAL_Delay(150);
	}
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
	
	initState(&basis, &gravityVec, &northVec);
	
	ST7735_Init(Displ_Orientat_270);
	ST7735_SetBacklight('I');
	ST7735_SetBacklight('F');
	
	initScreen();
	initBall();
	renewConcentricLines();
	
	ST7735_DrawLine(3,3, 13, 3, ST7735_RED);
	ST7735_DrawLine(3,3, 3, 18, ST7735_RED);
	ST7735_FillCircle(3, 3, 2, ST7735_RED);
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	microsStart(&accGyroPeriod);
  while (1)//////////////////////////////////////////////////////////////////////////////////////////
  {
		static uint32_t ledToggleCnt = 0;
		HAL_GPIO_TogglePin(StatusPin_GPIO_Port, StatusPin_Pin);
			
		MPU9255_cycle(micros());
			
		if( MPU9255_ACC_GYRO_isReady() == 1){
			struct matrix ds;
			float periodS;
			
			matrixSE(&accVec, 0, 0, ACx);
			matrixSE(&accVec, 1, 0, ACy);
			matrixSE(&accVec, 2, 0, ACz);
			accTotal = matrixVecLen(&accVec);
			
			microsUpdate(&accGyroPeriod);
			periodS = (float) accGyroPeriod.period/1000000;
			GYx = GYx * periodS;
			GYy = GYy * periodS;
			GYz = GYz * periodS;
			
			matrixRot3D(&rotMatrix, GYx, GYy, GYz);
			matrixMultiply(&basis, &rotMatrix, &basis);
			
			matrixMultiply(&basis, &accVec, &accVec);
			restoreVertical(&basis, &gravityVec, &accVec);
			matrixSum(&accVec, &gravityVec, &accVec);
			
/*			if ( fabs(matrixVecLen(&accVec)) > 0.5){
				matrixZeroes(&ds, 3, 1);
				matrixMultiplyByNumb(&velocityVec, &ds, periodS);
				matrixSum(&coordinate, &ds, &coordinate);
				matrixMultiplyByNumb(&accVec, &ds, pow(periodS, 2)/2);
				matrixSum(&coordinate, &ds, &coordinate);
				matrixMultiplyByNumb(&accVec, &ds, periodS);
				matrixSum(&velocityVec, &ds, &velocityVec);		
			} else
				matrixZeroes(&velocityVec, 3, 1);*/
		}		
		if( MPU9255_MAG_isReady() == 1){
			matrixSE(&magVec, 0, 0, MAGx);
			matrixSE(&magVec, 1, 0, MAGy);
			matrixSE(&magVec, 2, 0, MAGz);
			magTotal = matrixVecLen(&magVec);
		}		
		
		transmitBasis(basis.arr, micros(), accGyroMagOverload);
		
		if(ledToggleCnt == 10000){// I hope this can indicate the load of the CPU
				HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);	
				ledToggleCnt = 0;
		}
		ledToggleCnt++;
		
		moveBall(basis.arr[7], basis.arr[6], &ball);
		
		microsUpdate(&cyclePeriod);		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		}////////////////////////////////////////////////////////////////////////////////////////////////
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  hi2c1.Init.ClockSpeed = 400000;
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
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_1LINE;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 11811;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 10;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 47;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 1000;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
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
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  /* DMA1_Channel7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, ST7735_CS_Pin|ST7735_RES_Pin|ST7735_DC_Pin|StatusPin_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : ST7735_CS_Pin ST7735_RES_Pin ST7735_DC_Pin */
  GPIO_InitStruct.Pin = ST7735_CS_Pin|ST7735_RES_Pin|ST7735_DC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : StatusPin_Pin */
  GPIO_InitStruct.Pin = StatusPin_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(StatusPin_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
// this fcn is needed to orient the MPU sensor
void initState(struct matrix *basis, struct matrix *gravity, struct matrix* north){
	struct matrix	rotVec, Rinit, acc, imGravity, magInit;
	float c, s, lx, ly, lz, theta;
	uint8_t magCnt = 0;
	
	matrixZeroes(&rotVec, 3,1);
	matrixZeroes(&Rinit, 3,3);
	matrixZeroes(&acc, 3,1);
	matrixZeroes(&imGravity, 3,1);
	matrixZeroes(&magInit, 3, 1);
	// acquisition ACC GYRO MAG meawurements
	for(uint8_t i = 0; i < 100; i++){
		while ( MPU9255_cycle(micros()) && 1<<0 == 0)
			HAL_Delay(1);
		
		while (MPU9255_ACC_GYRO_isReady() == 0){
			HAL_Delay(1);
			MPU9255_cycle(micros());
		}
			
		acc.arr[0] += ACx;
		acc.arr[1] += ACy;
		acc.arr[2] += ACz;
		if (MPU9255_MAG_isReady()){
			magInit.arr[0] += MAGx;
			magInit.arr[1] += MAGy;
			magInit.arr[2] += MAGz;
			magCnt ++;
		}
	}
	//// vertical alignment
	matrixMultiplyByNumb(&acc, gravity, -0.01);
	imGravity.arr[2] = -matrixVecLen(gravity);
	matrixCross(gravity, &imGravity, &rotVec);
	matrixNormVec(&rotVec, &rotVec);
	
	theta = acos(matrixDot(gravity, &imGravity)/(matrixVecLen(&imGravity)*matrixVecLen(gravity)));
	matrixRotArbVec(&Rinit, &rotVec, theta);
	
	matrixMultiply(&Rinit, basis , basis);
	matrixMultiply(basis, gravity, gravity);	
	
	//// azimuth alignment
/*	matrixMultiplyByNumb(&magInit, &magInit, (float) 1/magCnt);
	matrixMultiply(basis, &magInit, &magInit);	
	magInit.arr[2] = 0;
	matrixNormVec(&magInit, &magInit);
	theta = -matrixVecAzimuth(north, &magInit);
	
	matrixSE(&Rinit,	0,	0, cos(theta));
	matrixSE(&Rinit,	0,	1, -sin(theta));
	matrixSE(&Rinit,	0,	2, 0);
	matrixSE(&Rinit,	1,	0, sin(theta));
	matrixSE(&Rinit,	1,	1, cos(theta));
	matrixSE(&Rinit,	1,	2, 0);
	matrixSE(&Rinit,	2,	0, 0);
	matrixSE(&Rinit,	2,	1, 0);
	matrixSE(&Rinit,	2,	2, 1);	
	
	matrixMultiply(&Rinit, basis , basis);*/
}
// transmits array of vectors and sensor overload flag to PC via COM
void transmitBasis(float* floatArray, uint32_t time, uint8_t overload){
	static uint32_t prewTime;
	if(time - prewTime < 30000) return;
	
	static uint8_t data[10];
	
	prewTime = time;
	for(uint8_t i = 0; i<9; i++){
		data[i] = (int8_t) (floatArray[i]*100);
	}
	data[9] = overload;
		
	HAL_UART_Transmit_DMA(&huart1, data, 10);
}
// allign vertical orientation to mitigate drift of gyro sensors or after a loss of angles due to overload
void restoreVertical(struct matrix* bas, struct matrix* grav, struct matrix* acc){
	struct matrix	rotVec, Rinit;
	float accLen, angle;
	
	matrixZeroes(&rotVec, 3,1);
	matrixZeroes(&Rinit, 3,3);
	accLen = matrixVecLen(acc);
	
	if( accLen > 9.8 && accLen <  10.2){
		matrixCross(acc, grav, &rotVec);
		matrixNormVec(&rotVec, &rotVec);
		angle = -acos(-matrixDot(acc, grav)/(accLen*matrixVecLen(grav)));
		if( angle < -0.03 || angle > 0.03){
			matrixRotArbVec(&Rinit, &rotVec, angle/50);
			matrixMultiply(&Rinit, bas, bas);
		}
	}
}

void initScreen(){
	ST7735_FillScreen(ST7735_BLACK);
	ST7735_DrawLine(0, 0, 158, 0, ST7735_CYAN);
	ST7735_DrawLine(0, 0, 0, 128, ST7735_CYAN);
	ST7735_DrawLine(0, 125, 158, 125, ST7735_CYAN);
	ST7735_DrawLine(158, 0, 158, 128, ST7735_CYAN);
}

void initBall(){
	ball.x = 0;
	ball.y = 0;
	ball.r = BALL_RADIUS;
	ball.c = ST7735_CYAN;
	ST7735_FillCircle(ball.x+CIRCLE_CENTER_X, ball.y+CIRCLE_CENTER_Y, ball.r, ball.c);
}

void renewConcentricLines(){
	ST7735_DrawCircle(CIRCLE_CENTER_X, CIRCLE_CENTER_Y, CIRCLE_SMALL_R, ST7735_RED);
	ST7735_DrawCircle(CIRCLE_CENTER_X, CIRCLE_CENTER_Y, CIRCLE_BIG_R, ST7735_RED);
	ST7735_DrawCircle(CIRCLE_CENTER_X, CIRCLE_CENTER_Y, CIRCLE_BIG_R+BALL_RADIUS-1, ST7735_RED);
//	ST7735_DrawCircle(CIRCLE_CENTER_X, CIRCLE_CENTER_Y, CIRCLE_BIG_R+BALL_RADIUS, ST7735_RED);
	ST7735_DrawCircle(CIRCLE_CENTER_X, CIRCLE_CENTER_Y, BALL_RADIUS+1, ST7735_RED);
	ST7735_DrawLine(CIRCLE_CENTER_X+BALL_RADIUS+1, CIRCLE_CENTER_Y, CIRCLE_CENTER_X + 40, CIRCLE_CENTER_Y, ST7735_RED);
	ST7735_DrawLine(CIRCLE_CENTER_X-BALL_RADIUS-1, CIRCLE_CENTER_Y, CIRCLE_CENTER_X - 40, CIRCLE_CENTER_Y, ST7735_RED);
	ST7735_DrawLine(CIRCLE_CENTER_X, CIRCLE_CENTER_Y+BALL_RADIUS+1, CIRCLE_CENTER_X, CIRCLE_CENTER_Y + 40, ST7735_RED);
	ST7735_DrawLine(CIRCLE_CENTER_X, CIRCLE_CENTER_Y-BALL_RADIUS-1, CIRCLE_CENTER_X, CIRCLE_CENTER_Y - 40, ST7735_RED);
}

void moveBall(float x, float y, balltype *ball){
	const float velProp = 1.5;
	static float xTarget, yTarget, xPrev, yPrev, R2 = CIRCLE_BIG_R*CIRCLE_BIG_R, xVel, yVel,\
		xBallPrev=0, yBallPrev=0;
	float r;
	static uint8_t displayCnt=0;
	static uint64_t drawPeriodPrev, calcPeriodPrev, calcPeriodPeriod;
	
	if(micros()-drawPeriodPrev>80000 && (xBallPrev != ball->x || yBallPrev != ball->y)){
		drawPeriodPrev = micros();
		
		ST7735_FillCircle(xBallPrev+CIRCLE_CENTER_X, yBallPrev+CIRCLE_CENTER_Y, ball->r, ST7735_BLACK);
		ST7735_FillCircle(ball->x+CIRCLE_CENTER_X, ball->y+CIRCLE_CENTER_Y, ball->r, ball->c);
		renewConcentricLines();
		xBallPrev = ball->x;
		yBallPrev = ball->y;
		displayCnt=0;
	}
	
	if( fabs(x-xPrev)<0.005 && fabs(y-yPrev)<0.005 && fabs(xVel) < 0.5 && fabs(yVel) < 0.5  || micros() - calcPeriodPrev < 1000)  return;
	xPrev = x;
	yPrev = y;
	
	calcPeriodPeriod = (uint32_t)(micros() - calcPeriodPrev);
	calcPeriodPrev = micros();
	
	x*=360;
	y*=360;
	
	ball->x += xVel * ((float)calcPeriodPeriod)/1000000;
	ball->y += yVel * ((float)calcPeriodPeriod)/1000000;
	r = ball->x*ball->x + ball->y*ball->y;	
	if (r > R2){
		r=sqrt(r);
		ball->y*=CIRCLE_BIG_R/r;
		ball->x*=CIRCLE_BIG_R/r;
		r=R2;
	}	
	
	if(r==R2 && fabs(y/x*ball->x-ball->y)<0.02 && x*x+y*y>R2 && ball->x*x>=0 && ball->y*y>=0){
		xVel = 0;
		yVel = 0;		
	}
	else{
		xVel = velProp*(x-ball->x);
		yVel = velProp*(y-ball->y);
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
