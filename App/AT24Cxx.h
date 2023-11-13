/*
 * AT24Cxx.h
 *
 *  Created on: Nov 7, 2023
 *      Author: rinaldo.santos
 */

#ifndef AT24CXX_H_
#define AT24CXX_H_

#include "main.h"
#include "i2c.h"
#include "stdbool.h"

#define	EEPROM_I2C			hi2c1
#define EEPROM_ADDRESS		0x50
#define EEPROM_PAGESIZE		128
#define EEPROM_WRITE		10					// time to wait in ms
#define EEPROM_TIMEOUT		5 * EEPROM_WRITE	// timeout while writing

#define PRG_REVISION		1

#define ADDR_ID				0x00
#define ADDR_REV			0x02
//
#define TARGET_IRON			0x10
#define TARGET_AIR			0x14
#define TARGET_GUN			0x18

HAL_StatusTypeDef AT24Cxx_IsConnected(void);
HAL_StatusTypeDef AT24Cxx_ReadEEPROM(unsigned address, const void* src, unsigned len);
HAL_StatusTypeDef AT24Cxx_WriteEEPROM(unsigned address, const void* src, unsigned len);
void ReadID_EEPROM(void);
void Save_Default_EEPROM(void);
void Read_Config_EEPROM(void);

#endif /* AT24CXX_H_ */
