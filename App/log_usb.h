/*
 * log_usb.h
 *
 *  Created on: Aug 21, 2023
 *      Author: rinaldo.santos
 */

#ifndef LOG_USB_H_
#define LOG_USB_H_

#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "usbd_cdc_if.h"
#include "usart.h"

void LogDebug(const char* fmt, ...);
void LogSerial(const char* fmt, ...);

#endif /* LOG_USB_H_ */
