/*
 * ILI9341.h
 *
 *  Created on: Aug 14, 2023
 *      Author: rinaldo.santos
 */

#ifndef ILI9341_H_
#define ILI9341_H_

#include "stm32f4xx_hal.h"
#include "main.h"
#include "spi.h"
#include "../lvgl/lvgl.h"

#define ILI9341_SCREEN_HEIGHT 					240
#define ILI9341_SCREEN_WIDTH 					320

//SPI INSTANCE
#define HSPI_INSTANCE							&hspi1

//CHIP SELECT PIN AND PORT, STANDARD GPIO
#define LCD_CS_PORT								TFT_CS_GPIO_Port
#define LCD_CS_PIN								TFT_CS_Pin

//DATA COMMAND PIN AND PORT, STANDARD GPIO
#define LCD_DC_PORT								TFT_DC_GPIO_Port
#define LCD_DC_PIN								TFT_DC_Pin

//RESET PIN AND PORT, STANDARD GPIO
#define	LCD_RST_PORT							TFT_RST_GPIO_Port
#define	LCD_RST_PIN								TFT_RST_Pin

#define BURST_MAX_SIZE 	500

#define BLACK       0x0000
#define NAVY        0x000F
#define DARKGREEN   0x03E0
#define DARKCYAN    0x03EF
#define MAROON      0x7800
#define PURPLE      0x780F
#define OLIVE       0x7BE0
#define LIGHTGREY   0xC618
#define DARKGREY    0x7BEF
#define BLUE        0x001F
#define GREEN       0x07E0
#define CYAN        0x07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define YELLOW      0xFFE0
#define WHITE       0xFFFF
#define ORANGE      0xFD20
#define GREENYELLOW 0xAFE5
#define PINK        0xF81F

#define SCREEN_VERTICAL_1			0
#define SCREEN_HORIZONTAL_1			1
#define SCREEN_VERTICAL_2			2
#define SCREEN_HORIZONTAL_2			3

void ILI9341_SPI_Init(void);
void ILI9341_SPI_Send(unsigned char SPI_Data);
void ILI9341_Write_Command(uint8_t Command);
void ILI9341_Write_Data(uint8_t Data);
void ILI9341_Set_Address(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2);
void ILI9341_Reset(void);
void ILI9341_Set_Rotation(uint8_t Rotation);
void ILI9341_Enable(void);
void ILI9341_Init(void);
void ILI9341_Fill_Screen(uint16_t Colour);
void ILI9341_Draw_Colour(uint16_t Colour);
void ILI9341_Draw_Pixel(uint16_t X,uint16_t Y,uint16_t Colour);
void ILI9341_Draw_Colour_Burst(uint16_t Colour, uint32_t Size);


void ILI9341_Draw_Rectangle(uint16_t X, uint16_t Y, uint16_t Width, uint16_t Height, uint16_t Colour);
void ILI9341_Draw_Horizontal_Line(uint16_t X, uint16_t Y, uint16_t Width, uint16_t Colour);
void ILI9341_Draw_Vertical_Line(uint16_t X, uint16_t Y, uint16_t Height, uint16_t Colour);

void ILI9341_Flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
void ILI9341_Flush_DMA(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
void ILI9341_Flush_IT(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
void ILI9341_End_Flush(lv_disp_drv_t * disp_drv);

#endif /* ILI9341_H_ */
