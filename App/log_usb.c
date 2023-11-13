/*
 * log_usb.c
 *
 *  Created on: Aug 21, 2023
 *      Author: rinaldo.santos
 */

#include "log_usb.h"

extern RTC_TimeTypeDef RTC_Time;
extern float temperature_K, temperature_air_K, temperature_iron, temperature_air;
extern uint32_t ADC_iron, ADC_air;
extern volatile uint8_t uart1_tx_complete;

char string_usb[1024];
char string_log[1024];

HAL_StatusTypeDef usart_hal = HAL_OK;

void HAL_printf_valist(const char *fmt, va_list argp)
{
  if (vsprintf(string_usb, fmt, argp) > 0) {
    CDC_Transmit_FS((uint8_t*)string_usb, strlen(string_usb));				// send message via USB CDC
  } else {
	CDC_Transmit_FS((uint8_t*)"E - Print\n", 14);
  }
  HAL_Delay(10);
}

void HAL_printf_serial(const char *fmt, va_list argp)
{
	if (vsprintf(string_usb, fmt, argp) > 0) {
		while(!__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TXE) && uart1_tx_complete == 0);
		usart_hal = HAL_UART_Transmit_IT(&huart1, (uint8_t*)string_log, strlen(string_log));

		if(usart_hal != HAL_OK) {
			Error_Handler();
		}
	}
}

void HAL_printf(const char *fmt, ...)
{
  va_list argp;

  va_start(argp, fmt);
  HAL_printf_valist(fmt, argp);
  va_end(argp);
}

void logUSB(const char *fmt, va_list argp)
{
	HAL_printf_valist(fmt, argp);
}

void LogDebug(const char* fmt, ...)
{
	va_list argp;

	va_start(argp, fmt);
	logUSB(fmt, argp);
	va_end(argp);
}

void LogSerial(const char* fmt, ...)
{
	va_list argp;

	va_start(argp, fmt);
	HAL_printf_serial(fmt, argp);
	va_end(argp);
}
