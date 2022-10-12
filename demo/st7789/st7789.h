#ifndef __ILI9341_H
#define __ILI9341_H

#include "wm_include.h"
#include "ascii_fonts.h"


#define ST7789_RES     WM_IO_PB_00
#define ST7789_DC      WM_IO_PB_01

#define ST7789_DC_CMD           HAL_GPIO_WritePin(ST7789_DC, GPIO_PIN_RESET)
#define ST7789_DC_DATA          HAL_GPIO_WritePin(ST7789_DC, GPIO_PIN_SET)
#define ST7789_RESET_LOW        HAL_GPIO_WritePin(ST7789_RES, GPIO_PIN_RESET)
#define ST7789_RESET_HIGH       HAL_GPIO_WritePin(ST7789_RES, GPIO_PIN_SET)

#define usleep(a)		tls_os_time_delay(a/10000)

#define ST7789_BUF_SIZE         1152
#define ST7789_TOTAL_BUF_SIZE	(240*240*2)

typedef enum
{
    GPIO_PIN_RESET = 0,
    GPIO_PIN_SET
} GPIO_PinState;


#define ST7789_SPI_CLK		WM_IO_PB_02
#define ST7789_SPI_MOSI		WM_IO_PB_05

#define ST7789_W 240
#define ST7789_H 240

//画笔颜色
#define WHITE			0xFFFF
#define BLACK			0x0000	  
#define BLUE			0x001F
#define BRED			0XF81F
#define GRED			0XFFE0
#define GBLUE			0X07FF
#define RED				0xF800
#define MAGENTA			0xF81F
#define GREEN			0x07E0
#define CYAN			0x7FFF
#define YELLOW			0xFFE0
#define BROWN			0XBC40 //棕色
#define BRRED			0XFC07 //棕红色
#define GRAY			0X8430 //灰色
//GUI颜色

#define DARKBLUE		0X01CF	//深蓝色
#define LIGHTBLUE		0X7D7C	//浅蓝色  
#define GRAYBLUE		0X5458 //灰蓝色
//以上三色为PANEL的颜色 
 
#define LIGHTGREEN		0X841F //浅绿色
#define LGRAY			0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE		0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE			0X2B12 //浅棕蓝色(选择条目的反色)


void ST7789_Init(void);
void ST7789_Fill(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color);
void ST7789_Show_Image(u16 x, u16 y, u16 width, u16 height, const u8 *p);
void ST7789_DrawChar(uint16_t x, uint16_t y, char ch, FontDef_t* font, uint16_t color, uint16_t bgcolor);
void ST7789_DrawString(uint16_t x, uint16_t y, const char *p, FontDef_t* font, uint16_t color, uint16_t bgcolor);
void ST7789_Clear(u16 color);
void ST7789_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *data);
// void ILI9341_DrawPoint(u16 x,u16 y,u16 color);
// void ILI9341_DrawLineOne(u16 y,u16* data);
// void ILI9341_DrawLine(u16 y,u16* data);
// void ILI9341_delay_ms(unsigned int ms);
// void ILI9341_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color);
// void ILI9341_Clear(u16 Color);

#endif


