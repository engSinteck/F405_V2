/*
 * AT24Cxx.c
 *
 *  Created on: Nov 7, 2023
 *      Author: rinaldo.santos
 */

#include "AT24Cxx.h"

extern uint32_t target_speed;
extern float target_air, target_iron;

static unsigned eeprom_address = EEPROM_ADDRESS << 1;
static unsigned inpage_addr_mask = EEPROM_PAGESIZE - 1;

uint16_t EepromID, prg_rev;

static HAL_StatusTypeDef AT24Cxx_WriteReadEEPROM(unsigned address, const void* src, unsigned len, bool write);
static unsigned size_to_page_end(unsigned addr);

HAL_StatusTypeDef AT24Cxx_IsConnected(void)
{
	return HAL_I2C_IsDeviceReady(&EEPROM_I2C, eeprom_address, 1, EEPROM_TIMEOUT);
}

HAL_StatusTypeDef AT24Cxx_ReadEEPROM(unsigned address, const void* src, unsigned len)
{
	return AT24Cxx_WriteReadEEPROM(address, src, len, false);
}

HAL_StatusTypeDef AT24Cxx_WriteEEPROM(unsigned address, const void* src, unsigned len)
{
	return AT24Cxx_WriteReadEEPROM(address, src, len, true);
}

static HAL_StatusTypeDef AT24Cxx_WriteReadEEPROM(unsigned address, const void* src, unsigned len, bool write)
{
	uint8_t *pdata = (uint8_t*) src;
	HAL_StatusTypeDef result = HAL_OK;

    unsigned max_portion = size_to_page_end(address);
    unsigned portion;

    while (len != 0 && result == HAL_OK) {
    	portion = len;

        if (portion > max_portion) {
        	portion = max_portion;
        }

        if(write) {
			result = HAL_I2C_Mem_Write(&EEPROM_I2C,
									eeprom_address,
									address,
									I2C_MEMADD_SIZE_16BIT,
									pdata,
									portion,
									EEPROM_TIMEOUT);
		}
        else {
        	result = HAL_I2C_Mem_Read(&EEPROM_I2C,
        	        				eeprom_address,
        							address,
        							I2C_MEMADD_SIZE_16BIT,
        							pdata,
        							portion,
        							EEPROM_TIMEOUT);
        }

        len     -= portion;
        address += portion;
        pdata   += portion;

        max_portion = EEPROM_PAGESIZE;

        if(write)
        	HAL_Delay(EEPROM_WRITE);
        else
        	HAL_Delay(EEPROM_WRITE / 2);
    }
    return result;
}

static unsigned size_to_page_end(unsigned addr)
{
    return (~addr & inpage_addr_mask) + 1;
}

void ReadID_EEPROM(void)
{
	AT24Cxx_ReadEEPROM(ADDR_ID, &EepromID, sizeof(EepromID));
	AT24Cxx_ReadEEPROM(ADDR_REV, &prg_rev, sizeof(prg_rev));
}

void Save_Default_EEPROM(void)
{
	EepromID = 0x55AA;
	prg_rev = PRG_REVISION;

	target_iron  = 30;
	target_air   = 50;
	target_speed = 10;

	AT24Cxx_WriteEEPROM(ADDR_ID,  (uint8_t*)&EepromID, sizeof(EepromID));
	AT24Cxx_WriteEEPROM(ADDR_REV, (uint8_t*)&prg_rev,  sizeof(prg_rev));

	AT24Cxx_WriteEEPROM(TARGET_IRON, (uint8_t*)&target_iron,  sizeof(target_iron));
	AT24Cxx_WriteEEPROM(TARGET_AIR,  (uint8_t*)&target_air,   sizeof(target_air));
	AT24Cxx_WriteEEPROM(TARGET_GUN,  (uint8_t*)&target_speed, sizeof(target_speed));
}

void Read_Config_EEPROM(void)
{
	AT24Cxx_ReadEEPROM(TARGET_IRON, (uint8_t*)&target_iron,  sizeof(target_iron));
	AT24Cxx_ReadEEPROM(TARGET_AIR,  (uint8_t*)&target_air,   sizeof(target_air));
	AT24Cxx_ReadEEPROM(TARGET_GUN,  (uint8_t*)&target_speed, sizeof(target_speed));
}
