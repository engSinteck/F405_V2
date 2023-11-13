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
#include "crc.h"
#include "dma.h"
#include "fatfs.h"
#include "i2c.h"
#include "rng.h"
#include "rtc.h"
#include "sdio.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "stdio.h"
#include "../App/key.h"
#include "../App/ILI9341.h"
#include "../App/AT24Cxx.h"
#include "../App/MAX31856.h"
#include "../App/Encoder.h"
#include "../App/log_usb.h"
#include "../lvgl/lvgl.h"
#include "../App/screen.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
void calculate_calibration(void);
void filter_adc(void);
void Zero_Crossing_Int(void);
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
extern char string_log[];
extern uint16_t EepromID, prg_rev;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
encoder rot1;
encoder rot2;
encoder rot3;
RTC_TimeTypeDef RTC_Time = {0};
RTC_DateTypeDef RTC_Date = {0};

volatile uint8_t uart1_tx_complete = 0;
uint32_t debug_cnt = 0;

volatile uint8_t rx_uart1[512] = { 0 };
uint32_t tx_uart_isr = 0;

GPIO_PinState pin_sw_air;
uint8_t sw_air_low = 0;
uint8_t sw_air_high = 0;
volatile uint8_t sw_air = 0;

GPIO_PinState pin_sw_iron;
uint8_t sw_iron_low = 0;
uint8_t sw_iron_high = 0;
volatile uint8_t sw_iron = 0;

uint16_t adcBuffer[4]; 					// Buffer ADC conversion
uint16_t filter_adc_1[8] = { 0 };
uint16_t filter_adc_2[8] = { 0 };
uint16_t filter_adc_3[8] = { 0 };
uint16_t ADC_temp = 0, ADC_vref = 0, ADC_vbat = 0;
uint16_t pwm_iron = 0;
uint16_t idx_flt = 0;
uint16_t flt_flag = 0;
uint32_t timer_key = 0, timer_lvgl = 0, timer_rtc = 0;
uint32_t timer_lcd = 0, timer_debug = 0, timer_therm = 0;

float temp_iron = 0.0f;
float temp_gun  = 0.0f;
float target_iron, target_air;
uint32_t target_speed = 0;

volatile uint32_t enc1_cnt=0, enc1_last=0, enc1_dir=0, enc1_btn=0;
volatile uint32_t enc2_cnt=0, enc2_last=0, enc2_dir=0, enc2_btn=0;
volatile uint32_t enc3_cnt=0, enc3_last=0, enc3_dir=0, enc3_btn=0;
volatile uint32_t enc1_val=0, enc2_val=0, enc3_val=0;

volatile int NumHandled  = 0;
volatile bool zero_cross = 0;
int NumActiveChannels = NUM_DIMMERS;
volatile bool isHandled[NUM_DIMMERS] = { 0, 0 };
uint8_t State[NUM_DIMMERS] = { 1, 1 };
bool pLampState[2]={ false, false };
uint32_t dimmer_value[NUM_DIMMERS]   = { 0, 0 };
uint32_t dimmer_Counter[NUM_DIMMERS] = { 0, 0 };
uint32_t timer_cnt5 = 0;

float vdda = 0.0f; 		// Result of VDDA calculation
float vref = 0.0f; 		// Result of VREF calculation
float vbat = 0.0f;		// Result of VBAT calculation
float temp_stm, ta, tb; // transfer function using calibration data

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[(ILI9341_SCREEN_WIDTH * 20)] __attribute__((section(".ramtft")));	// Declare a buffer for 1/10 screen size
static lv_color_t buf2[(ILI9341_SCREEN_WIDTH * 20)] __attribute__((section(".ramtft")));
static lv_disp_drv_t disp_drv;        					// Descriptor of a display driver

max31856_t therm_iron = {&hspi2, {CS_IRON_GPIO_Port, CS_IRON_Pin}};
max31856_t therm_gun  = {&hspi2, {CS_GUN_GPIO_Port, CS_GUN_Pin}};
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
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_RTC_Init();
  MX_SDIO_SD_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_TIM9_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_TIM12_Init();
  MX_FATFS_Init();
  MX_CRC_Init();
  MX_RNG_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
  timer_cnt5 = 0;
  HAL_TIM_Base_Start_IT(&htim5);
  HAL_TIM_Base_Start_IT(&htim12);
  HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);

  // PWM
  pwm_iron = 80;
  HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_1);
  __HAL_TIM_SetCompare(&htim9, TIM_CHANNEL_1, pwm_iron);	// PWM_CH1 = 4095 IRON
  // Start ADC
  idx_flt = 0;
  flt_flag = 0;
  calculate_calibration();
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcBuffer, 3);		// Start ADC in DMA

  HAL_UART_Receive_IT(&huart1, (uint8_t *)rx_uart1, 1);

  // Apaga LEDS
  // LED-1 - PWM GUN
  // LED-2 - REPOUSO
  // LED-3 - OPERATE
  // LED-4 - PWM IRON
  HAL_GPIO_WritePin(DIMMER_1_GPIO_Port, DIMMER_1_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(DIMMER_2_GPIO_Port, DIMMER_2_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(RELAY_GPIO_Port,RELAY_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);

  sprintf(string_log, "\n\r STM32F405 CPU YAXUN v2.0.0 \n\r\n\r");
  LogSerial(string_log);

  HAL_Delay(100);
  sprintf(string_log, "ILI9341 Init....\n\r\n\r");
  LogSerial(string_log);
  ILI9341_Init();
  ILI9341_Set_Address(0, 0, ILI9341_SCREEN_WIDTH-1, ILI9341_SCREEN_HEIGHT-1);
  ILI9341_Set_Rotation(1);
  ILI9341_Fill_Screen(BLACK);

  Evt_InitQueue();
  KeyboardInit(0x01);

  EepromID = 0xFFFF;
  prg_rev = 0xFFFF;
  if(AT24Cxx_IsConnected() == HAL_OK){
	  sprintf(string_log, "EEPROM Present! \n\r");
	  LogSerial(string_log);
  }
  else {
	  sprintf(string_log, "EEPROM Not Connected. \n\r");
	  LogSerial(string_log);
  }
  ReadID_EEPROM();
  if(EepromID != 0x55AA || prg_rev != PRG_REVISION) {
	  sprintf(string_log, "Carrega Prog. Default \n\r");
	  LogSerial(string_log);
	  Save_Default_EEPROM();
  }
  else {
	  sprintf(string_log, "Carrega Prog. EEPROM \n\r");
	  LogSerial(string_log);
	  Read_Config_EEPROM();
  }

  HAL_Delay(100);
  sprintf(string_log, "MAX31856 Init....\n\r\n\r\n\r");
  LogSerial(string_log);

  // Thermocouple Iron
  max31856_init(&therm_iron);
  max31856_set_noise_filter(&therm_iron, CR0_FILTER_OUT_60Hz);
  max31856_set_cold_junction_enable(&therm_iron, CR0_CJ_ENABLED);
  max31856_set_thermocouple_type(&therm_iron, CR1_TC_TYPE_K);
  max31856_set_average_samples(&therm_iron, CR1_AVG_TC_SAMPLES_8);
  max31856_set_open_circuit_fault_detection(&therm_iron, CR0_OC_DETECT_ENABLED_TC_LESS_2ms);
  max31856_set_conversion_mode(&therm_iron, CR0_CONV_CONTINUOUS);
  temp_iron = max31856_read_TC_temp(&therm_iron);

  // Thermocouple GUN
  max31856_init(&therm_gun);
  max31856_set_noise_filter(&therm_gun, CR0_FILTER_OUT_60Hz);
  max31856_set_cold_junction_enable(&therm_gun, CR0_CJ_ENABLED);
  max31856_set_thermocouple_type(&therm_gun, CR1_TC_TYPE_K);
  max31856_set_average_samples(&therm_gun, CR1_AVG_TC_SAMPLES_8);
  max31856_set_open_circuit_fault_detection(&therm_gun, CR0_OC_DETECT_ENABLED_TC_LESS_2ms);
  max31856_set_conversion_mode(&therm_gun, CR0_CONV_CONTINUOUS);
  temp_gun = max31856_read_TC_temp(&therm_gun);

  lv_init();

  lv_disp_draw_buf_init(&draw_buf, buf1, buf2, (ILI9341_SCREEN_WIDTH * 20) );		// Initialize the display buffer.
  lv_disp_drv_init(&disp_drv);          			// Basic initialization

  disp_drv.flush_cb = ILI9341_Flush_IT;   			// Set your driver function
  disp_drv.hor_res  = ILI9341_SCREEN_WIDTH;   		// Set the horizontal resolution of the display
  disp_drv.ver_res  = ILI9341_SCREEN_HEIGHT;   		// Set the vertical resolution of the display

  disp_drv.draw_buf   = &draw_buf;        			// Assign the buffer to the display
  disp_drv.rotated    = LV_DISP_ROT_90;
  disp_drv.sw_rotate  = 1;
  lv_disp_drv_register(&disp_drv);      			// Finally register the driver

  screen_main();
  screen_debug();
  load_screen(0);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	if(HAL_GetTick() - timer_lvgl > 5) {
		timer_lvgl = HAL_GetTick();
		lv_timer_handler();
	}

	if(HAL_GetTick() - timer_rtc > 1000) {
		timer_rtc = HAL_GetTick();
		HAL_RTC_GetTime(&hrtc, &RTC_Time, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &RTC_Date, RTC_FORMAT_BIN);
	}

	// Read Encoders
	Read_Encoder();

	// Buttons Encoders
	KeyboardEvent();

	// Read ADC
	filter_adc();

	if(HAL_GetTick() - timer_therm > 250) {
		timer_therm = HAL_GetTick();
		max31856_read_fault(&therm_iron);
		if (!therm_iron.sr.val) {
			temp_iron = max31856_read_TC_temp(&therm_iron);
		}
		else {
			temp_iron = 999.99f;
		}
		max31856_read_fault(&therm_gun);
		if (!therm_gun.sr.val) {
			temp_gun = max31856_read_TC_temp(&therm_gun);
		}
		else {
			temp_gun = 999.99f;
		}
	}

	// Log Debug
	if(HAL_GetTick() - timer_debug > 1000) {
		timer_debug = HAL_GetTick();
		sprintf(string_log, "ENC1: %04ld  BT: %ld - ENC2: %04ld  BT: %ld - ENC3: %04ld  BT: %ld - idx: %ld BT: %d\n\r",
			    enc1_last, enc1_btn, enc2_last, enc2_btn, enc3_last, enc3_btn, tx_uart_isr,
				HAL_GPIO_ReadPin(KEY_USER_GPIO_Port, KEY_USER_Pin) );
		LogSerial(string_log);
	}
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enables the Clock Security System
  */
  HAL_RCC_EnableCSS();
}

/* USER CODE BEGIN 4 */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi->Instance == SPI1) {
		ILI9341_End_Flush(&disp_drv);
		timer_lcd = HAL_GetTick() - timer_lcd;
	}
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	filter_adc_1[idx_flt] = (uint32_t)adcBuffer[0];
	filter_adc_2[idx_flt] = (uint32_t)adcBuffer[1];
	filter_adc_3[idx_flt] = (uint32_t)adcBuffer[2];
	idx_flt++;
	if(idx_flt >= 8) {
		idx_flt = 0;
		flt_flag = 1;
	}
}

void filter_adc(void)
{
	if(flt_flag == 1) {
        ADC_temp = ( ( filter_adc_1[0]+filter_adc_1[1]+filter_adc_1[2]+filter_adc_1[3]+filter_adc_1[4]+filter_adc_1[5]+filter_adc_1[6]+filter_adc_1[7] ) / 8 );
		ADC_vref = ( ( filter_adc_2[0]+filter_adc_2[1]+filter_adc_2[2]+filter_adc_2[3]+filter_adc_2[4]+filter_adc_2[5]+filter_adc_2[6]+filter_adc_2[7] ) / 8 );
        ADC_vbat = ( ( filter_adc_3[0]+filter_adc_3[1]+filter_adc_3[2]+filter_adc_3[3]+filter_adc_3[4]+filter_adc_3[5]+filter_adc_3[6]+filter_adc_3[7] ) / 8 );

		// VDDA can be calculated based on the measured vref and the calibration data
	    vdda = (float)VREFINT_CAL_VREF * (float)*VREFINT_CAL_ADDR / ADC_vref;

	    // Knowing vdda and the resolution of adc - the actual voltage can be calculated
	    vref = (float) vdda / 4095 * ADC_vref;

	    //
	    vbat = (float)VREFINT_CAL_VREF * (float)*VREFINT_CAL_ADDR / ADC_vbat;

	    temp_stm = (float) (ta * (float) (ADC_temp) + tb);
		flt_flag = 0;
	}
}

void calculate_calibration(void)
{
    float x1 = (float) *TEMPSENSOR_CAL1_ADDR;
    float x2 = (float) *TEMPSENSOR_CAL2_ADDR;
    float y1 = (float) TEMPSENSOR_CAL1_TEMP;
    float y2 = (float) TEMPSENSOR_CAL2_TEMP;

    // Simple linear equation y = ax + b based on two points
    ta = (float) ((y2 - y1) / (x2 - x1));
    tb = (float) ((x2 * y1 - x1 * y2) / (x2 - x1));
}

void debounce_input(void)
{
	// Debounce SW_AIR PIN
	pin_sw_air = HAL_GPIO_ReadPin(SW_AIR_GPIO_Port, SW_AIR_Pin);
	if(pin_sw_air == 0) {
		sw_air_low++;
		sw_air_high = 0;
		if(sw_air_low >= DEBOUNCE_SW) {
			sw_air_low = DEBOUNCE_SW + 1;
			sw_air = 0;
		}
	}
	else {
		sw_air_high++;
		sw_air_low = 0;
		if(sw_air_high >= DEBOUNCE_SW) {
			sw_air_high = DEBOUNCE_SW + 1;
			sw_air = 1;
		}
	}
	//
	// Debounce SW_IRON PIN
	pin_sw_iron = HAL_GPIO_ReadPin(SW_AIR_GPIO_Port, SW_AIR_Pin);
	if(pin_sw_iron == 0) {
		sw_iron_low++;
		sw_iron_high = 0;
		if(sw_iron_low >= DEBOUNCE_SW) {
			sw_iron_low = DEBOUNCE_SW + 1;
			sw_iron = 0;
		}
	}
	else {
		sw_iron_high++;
		sw_iron_low = 0;
		if(sw_iron_high >= DEBOUNCE_SW) {
			sw_iron_high = DEBOUNCE_SW + 1;
			sw_iron = 1;
		}
	}
}

// handles EXTI line3 interrupt
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == ZERO_CROSS_Pin)
    {
		HAL_NVIC_DisableIRQ(EXTI4_IRQn);
		Zero_Crossing_Int();
		HAL_NVIC_EnableIRQ(EXTI4_IRQn);
    }
}

void Zero_Crossing_Int(void)
{
	NumHandled = 0;

	isHandled[0] = 0;
	isHandled[1] = 0;
	timer_cnt5 = 0;
	zero_cross = 1;
	HAL_GPIO_WritePin(DIMMER_1_GPIO_Port, DIMMER_1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(DIMMER_2_GPIO_Port, DIMMER_2_Pin, GPIO_PIN_SET);
}

void dimmerTimerISR(void)
{
	if(zero_cross == 1) {
		for(int i = 0; i < NUM_DIMMERS; i++) {
			if(State[i] == 1) {
				if( dimmer_Counter[i] > (DIMMER_VALUE_MAX - dimmer_value[i]) ) {
					if(i == 0){
						HAL_GPIO_WritePin(DIMMER_1_GPIO_Port, DIMMER_1_Pin, GPIO_PIN_RESET);
						pLampState[i] = true;
					}else if(i  == 1){
						HAL_GPIO_WritePin(DIMMER_2_GPIO_Port, DIMMER_2_Pin, GPIO_PIN_RESET);
						pLampState[i] = true;
					}
					isHandled[i] = 1;
					NumHandled++;
					if(NumHandled == NumActiveChannels) {
						zero_cross = 0;
					}
				}
				else if(isHandled[i] == 0) {
					dimmer_Counter[i]++;
				}
			}
		}
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART1) {
		tx_uart_isr++;
		uart1_tx_complete = 1;
	}
}
/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
  if (htim->Instance == TIM1) {
	  debounce_input();
	  lv_tick_inc(1);
	  timer_key++;
	  if(timer_key >= PUSHBTN_TMR_PERIOD) {
		  timer_key = 0;
		  Key_Read();
	  }
  }
  // Timer Dimmer 12Khz
  if(htim->Instance == TIM5) {			// 12Khz - 84us
	  timer_cnt5++;
  	  dimmerTimerISR();
  }
  // Timer Teste Zero Cross 120Hz
  if(htim->Instance == TIM12) {			// 120Hz - 8.33ms
	  timer_cnt5 = 0;
	  HAL_GPIO_WritePin(TEST_120Hz_GPIO_Port, TEST_120Hz_Pin, GPIO_PIN_SET);
  	  for(uint16_t u = 0; u < 800; u++) {
  		  __NOP();
  	  }
  	  HAL_GPIO_WritePin(TEST_120Hz_GPIO_Port, TEST_120Hz_Pin, GPIO_PIN_RESET);
  }
  /* USER CODE END Callback 1 */
}

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
