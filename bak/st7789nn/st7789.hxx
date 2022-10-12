#ifndef __ST7789_H
#define __ST7789_H

#include "wm_include.h"
#include "ascii_fonts.h"

#define ST7789_BUF_SIZE         1024
#define ST7789_HARDWARE_SPI     1

typedef enum
{
    GPIO_PIN_RESET = 0,
    GPIO_PIN_SET
} GPIO_PinState;

#define usleep(a)		tls_os_time_delay(a/10000)

void HAL_Delay(int);

#define ST7789_RES     WM_IO_PB_01
#define ST7789_DC      WM_IO_PB_00

#define ST7789_CK      WM_IO_PB_02
#define ST7789_MO      WM_IO_PB_05

#define ST7789_DC_LOW           HAL_GPIO_WritePin(ST7789_DC, GPIO_PIN_RESET)
#define ST7789_DC_HIGH          HAL_GPIO_WritePin(ST7789_DC, GPIO_PIN_SET)
#define ST7789_RESET_LOW        HAL_GPIO_WritePin(ST7789_RES, GPIO_PIN_RESET)
#define ST7789_RESET_HIGH       HAL_GPIO_WritePin(ST7789_RES, GPIO_PIN_SET)


#define ST7789_WIDTH  240
#define ST7789_HEIGHT 240
#define ST7789_XSTART 0
#define ST7789_YSTART 0
#define ST7789_CASET     0x2A
#define ST7789_RASET     0x2B
#define ST7789_RAMWR     0x2C
#define ST7789_RAMRD     0x2E
#define ST7789_ROTATION (ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB)


#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40
#define BRRED 			 0XFC07
#define GRAY  			 0X8430

#define DARKBLUE      	 0X01CF
#define LIGHTBLUE      	 0X7D7C
#define GRAYBLUE       	 0X5458

#define LIGHTGREEN     	 0X841F

#define LGRAY 			 0XC618

#define LGRAYBLUE        0XA651
#define LBBLUE           0X2B12


//void ST7789_Reset(void);
// void ST7789_DrawPoint(uint16_t x, uint16_t y, uint16_t color);
// void ST7789_DrawLine(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color);
// void ST7789_DrawRectangle(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color);
// void ST7789_DrawCircle(uint16_t x, uint16_t y, uint8_t radius, uint16_t color);
// void ST7789_DrawChar(uint16_t x, uint16_t y, const char ch, FontDef_t* font, uint16_t color, uint16_t bgcolor);
// void ST7789_DrawString(uint16_t x, uint16_t y, const char *p, FontDef_t* font, uint16_t color, uint16_t bgcolor);
// void ST7789_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data);
// void ST7789_Fill(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color);
// void ST7789_LCD_Init(void);
// void ST7789_Init(void);

#endif
