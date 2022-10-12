
#include <string.h>
#include "st7789.h"
#include "font.h"

static uint8_t st7789_buf[ST7789_BUF_SIZE];
static uint16_t st7789_buf_pt = 0;

static  void HAL_GPIO_WritePin(u16 gpiox_num, GPIO_PinState pin_state) {
    tls_gpio_write(gpiox_num, pin_state);
}

static u16 BACK_COLOR;
static u32 count = 0;
static bool isdata = 0;

static void ST7789_GPIO_Init(void)
{
	tls_gpio_cfg(ST7789_RES, WM_GPIO_DIR_OUTPUT, WM_GPIO_ATTR_FLOATING);
	tls_gpio_write(ST7789_RES,0);
	
	tls_gpio_cfg(ST7789_DC, WM_GPIO_DIR_OUTPUT, WM_GPIO_ATTR_FLOATING);
	tls_gpio_write(ST7789_DC,0);
	
	isdata=0;
}


static void ST7789_GPIO_CMD(void)
{
	if(isdata)
	{
		ST7789_DC_CMD;
		isdata=0;
	}
}

static void ST7789_GPIO_DATA(void)
{
	if(!isdata)
	{
		ST7789_DC_DATA;
		isdata=1;
	}
}

//SPI初始化
static int ST7789_SPI_init(void)
{
	int clk=160000000;
	int type=2;

	wm_spi_ck_config(ST7789_SPI_CLK);
	wm_spi_do_config(ST7789_SPI_MOSI);
	
    if (0 == type)
    {
        tls_spi_trans_type(0);
    }
    else
    {
        tls_spi_trans_type(2);
    }

    tls_spi_setup(TLS_SPI_MODE_0, TLS_SPI_CS_LOW, clk);
	printf("slave_spi_demo\n");
    return WM_SUCCESS;
}

//延时函数
void HAL_Delay_MS(unsigned int ms)
{                         
	//tls_os_time_delay(ms /HZ);
	usleep(ms*1000); 
}

//写数据
void ILI9341_WR_DATA(u16 dat)
{
	uint8_t data[2];
	data[0] = dat>>8;
	data[1] = dat;
	GPIO_DATA();//写数据
	//printf("-------------%x", data);
	tls_spi_write((uint8_t *)data,2);
}

//发送一个字节
static void ST7789_TransmitByte(uint8_t dat)
{
    tls_spi_write(&dat, 1);
}

static void ST7789_Transmit(const uint8_t *pData, uint32_t Size)
{
	tls_spi_write(pData, Size);
}

//写命令
static void ST7789_WriteCommand(uint8_t dat)
{
    ST7789_GPIO_CMD();
    ST7789_TransmitByte(dat);
}

//写一个自己数据
static void ST7789_WriteDataByte(const uint8_t* buff)
{
    ST7789_GPIO_DATA();
    ST7789_TransmitByte(buff);
}

static void ST7789_WriteData(const uint8_t* buff, size_t buff_size)
{
    ST7789_GPIO_DATA();
    ST7789_Transmit(buff, buff_size);
}


//设置窗口
static void ST7789_SetAddrWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	ST7789_WriteCommand(0x2A);
	ST7789_WriteDataByte(x1 >> 8);
	ST7789_WriteDataByte(x1);
	ST7789_WriteDataByte(x2 >> 8);
	ST7789_WriteDataByte(x2);

	ST7789_WriteCommand(0x2b);
	ST7789_WriteDataByte(y1 >> 8);
	ST7789_WriteDataByte(y1);
	ST7789_WriteDataByte(y2 >> 8);
	ST7789_WriteDataByte(y2);

	ST7789_WriteCommand(0x2C);
}


static void ST7789_WriteBuff(uint8_t* buff, size_t buff_size)
{
    while (buff_size--)
    {
        st7789_buf[st7789_buf_pt++] = *buff++;
        if (st7789_buf_pt == ST7789_BUF_SIZE)
        {
            ST7789_Transmit(st7789_buf, st7789_buf_pt);
            st7789_buf_pt = 0;
        }
    }
}

static void ST7789_FlushBuff(void)
{
    if (st7789_buf_pt > 0)
    {
        ST7789_Transmit(st7789_buf, st7789_buf_pt);
        st7789_buf_pt = 0;
    }
}

//填充一块区域
void ST7789_Fill(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color)
{
	u16 i = 0;
    u32 size = 0, size_remain = 0;

    size = (x_end - x_start + 1) * (y_end - y_start + 1) * 2;

    if(size > ST7789_BUF_SIZE)
    {
        size_remain = size - ST7789_BUF_SIZE;
        size = ST7789_BUF_SIZE;
    }

    ST7789_SetAddrWindow(x_start, y_start, x_end, y_end);

    while(1)
    {
        for(i = 0; i < size / 2; i++)
        {
            st7789_buf[2 * i] = color >> 8;
            st7789_buf[2 * i + 1] = color;
        }
        ST7789_GPIO_DATA();
        ST7789_WriteBuff(&st7789_buf, size);

        if(size_remain == 0)
            break;

        if(size_remain > ST7789_BUF_SIZE)
        {
            size_remain = size_remain - ST7789_BUF_SIZE;
        }
        else
        {
            size = size_remain;
            size_remain = 0;
        }
    }
}


//画图片
// void ST7789_Show_Image(u16 x, u16 y, u16 width, u16 height, const u8 *p)
// {
//     if(x + width > ST7789_W || y + height > ST7789_H)
//     {
//         return;
//     }

//     ST7789_SetAddrWindow(x, y, x + width - 1, y + height - 1);
//    	ST7789_GPIO_DATA();
//     ST7789_WriteBuff((u8 *)p, width * height * 2);

// }


void ST7789_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *data)
{
    if ((x >= ST7789_W) || (y >= ST7789_H))
        return;
    if ((x + w - 1) >= ST7789_W)
        return;
    if ((y + h - 1) >= ST7789_H)
        return;

    ST7789_SetAddrWindow(x, y, x + w - 1, y + h - 1);
    ST7789_WriteBuff((uint8_t *)data, sizeof(uint16_t) * w * h);
    ST7789_FlushBuff();
}

//画
void ST7789_DrawChar(uint16_t x, uint16_t y, char ch, FontDef_t* font, uint16_t color, uint16_t bgcolor)
{
    uint8_t b, i, j, k, bytes;

    ST7789_SetAddrWindow(x, y, x + font->width - 1, y + font->height - 1);
    ST7789_GPIO_DATA();
    bytes = font->width / 8 + ((font->width % 8)? 1 : 0);

    for (i = 0; i < font->height; i++)
    {
        for (j = 0; j < bytes; j++)
        {
            b = font->data[((ch - 32) * font->height + i) * bytes + j];
            if (font->order == 0)
            {
                for (k = 0; k < 8 && k < font->width - j * 8; k++)
                {
                    if ((b << k) & 0x80)
                    {
                        ST7789_WriteBuff((uint8_t *)&color, 2);
                    }
                    else
                    {
                        ST7789_WriteBuff((uint8_t *)&bgcolor, 2);
                    }
                }
            }
            else
            {
                for (k = 0; k < 8 && k < font->width - j * 8; k++)
                {
                    if (b & (0x0001 << k))
                    {
                        ST7789_WriteBuff((uint8_t *)&color, 2);
                    }
                    else
                    {
                        ST7789_WriteBuff((uint8_t *)&bgcolor, 2);
                    }
                }
            }
        }
    }
    ST7789_FlushBuff();
}

void ST7789_DrawString(uint16_t x, uint16_t y, const char *p, FontDef_t* font, uint16_t color, uint16_t bgcolor)
{
    while (*p != '\0')
    {
        ST7789_DrawChar(x, y, *p, font, color, bgcolor);
        x += font->width;
        p++;
    }
}

void ST7789_Clear(u16 color)
{
    u16 i, j;
    u8 data[2] = {0};

    data[0] = color >> 8;
    data[1] = color;

    ST7789_SetAddrWindow(0, 0, ST7789_W - 1, ST7789_H - 1);

    for(j = 0; j < ST7789_BUF_SIZE / 2; j++)
    {
        st7789_buf[j * 2] =  data[0];
        st7789_buf[j * 2 + 1] =  data[1];
    }

    ST7789_GPIO_DATA();

    for(i = 0; i < (ST7789_TOTAL_BUF_SIZE / ST7789_BUF_SIZE); i++)
    {
        ST7789_WriteBuff(st7789_buf, ST7789_BUF_SIZE);
    }
}

//LCD初始化
void ST7789_LCD_Init(void)
{
	ST7789_RESET_LOW;
	HAL_Delay_MS(20);
	ST7789_RESET_HIGH;

	HAL_Delay_MS(20);

	ST7789_WriteCommand(0xE0); //Set Gamma
	ST7789_WriteDataByte(0x0F);
	ST7789_WriteDataByte(0x22);
	ST7789_WriteDataByte(0x1C);
	ST7789_WriteDataByte(0x1B);
	ST7789_WriteDataByte(0x08);
	ST7789_WriteDataByte(0x0F);
	ST7789_WriteDataByte(0x48);
	ST7789_WriteDataByte(0xB8);
	ST7789_WriteDataByte(0x34);
	ST7789_WriteDataByte(0x05);
	ST7789_WriteDataByte(0x0C);
	ST7789_WriteDataByte(0x09);
	ST7789_WriteDataByte(0x0F);
	ST7789_WriteDataByte(0x07);
	ST7789_WriteDataByte(0x00);

	ST7789_WriteCommand(0XE1); //Set Gamma
	ST7789_WriteDataByte(0x00);
	ST7789_WriteDataByte(0x23);
	ST7789_WriteDataByte(0x24);
	ST7789_WriteDataByte(0x07);
	ST7789_WriteDataByte(0x10);
	ST7789_WriteDataByte(0x07);
	ST7789_WriteDataByte(0x38);
	ST7789_WriteDataByte(0x47);
	ST7789_WriteDataByte(0x4B);
	ST7789_WriteDataByte(0x0A);
	ST7789_WriteDataByte(0x13);
	ST7789_WriteDataByte(0x06);
	ST7789_WriteDataByte(0x30);
	ST7789_WriteDataByte(0x38);
	ST7789_WriteDataByte(0x0F);

	ST7789_WriteCommand(0x11);
    /* wait for power stability */

    HAL_Delay_MS(120);

    /* Memory Data Access Control */
    ST7789_WriteCommand(0x36);
    ST7789_WriteDataByte(0x00);

    /* RGB 5-6-5-bit  */
    ST7789_WriteCommand(0x3A);
    ST7789_WriteDataByte(0x65);

    /* Porch Setting */
    ST7789_WriteCommand(0xB2);
    ST7789_WriteDataByte(0x0C);
    ST7789_WriteDataByte(0x0C);
    ST7789_WriteDataByte(0x00);
    ST7789_WriteDataByte(0x33);
    ST7789_WriteDataByte(0x33);

    /*  Gate Control */
    ST7789_WriteCommand(0xB7);
    ST7789_WriteDataByte(0x72);

    /* VCOM Setting */
    ST7789_WriteCommand(0xBB);
    ST7789_WriteDataByte(0x3D);   //Vcom=1.625V

    /* LCM Control */
    ST7789_WriteCommand(0xC0);
    ST7789_WriteDataByte(0x2C);

    /* VDV and VRH Command Enable */
    ST7789_WriteCommand(0xC2);
    ST7789_WriteDataByte(0x01);

    /* VRH Set */
    ST7789_WriteCommand(0xC3);
    ST7789_WriteDataByte(0x19);

    /* VDV Set */
    ST7789_WriteCommand(0xC4);
    ST7789_WriteDataByte(0x20);

    /* Frame Rate Control in Normal Mode */
    ST7789_WriteCommand(0xC6);
    ST7789_WriteDataByte(0x0F);	//60MHZ

    /* Power Control 1 */
    ST7789_WriteCommand(0xD0);
    ST7789_WriteDataByte(0xA4);
    ST7789_WriteDataByte(0xA1);

    /* Display Inversion On */
    ST7789_WriteCommand(0x21);

    ST7789_WriteCommand(0x29);

	ST7789_SetAddrWindow(0, 0, 239, 239);
	ST7789_GPIO_DATA();
	isdata=1;
} 

void ST7789_Init(void)
{
	ST7789_GPIO_Init();
	ST7789_SPI_init();
	ST7789_LCD_Init();

	// ILI9341_WR_REG(0x36);	   //set the model of scanning
	// ILI9341_WR_DATA8((1<<5)|(0<<6)|(1<<7)|(1<<3)); //左横屏
	
	// //ILI9341_WR_DATA8((1<<5)|(1<<6));右横屏幕
	// //ILI9341_WR_DATA8(0x08); 竖屏幕
	// ILI9341_WR_REG(0x2B); 	//set the page address 横屏幕设置 Y轴到239
	// ILI9341_WR_DATA8(0x00);
	// ILI9341_WR_DATA8(0x00);
	// ILI9341_WR_DATA8(0x00);
	// ILI9341_WR_DATA8(0xDF);

	// ILI9341_WR_REG(0x2A);    //set the column address X轴到319 改为255
	// ILI9341_WR_DATA8(0x00);
	// ILI9341_WR_DATA8(0x00);
	// //ILI9341_WR_DATA8(0x01);
	// //ILI9341_WR_DATA8(0x3F);
	// ILI9341_WR_DATA8(0x00);
	// ILI9341_WR_DATA8(0xFF);
	
	// ILI9341_WR_REG(0x2c);//储存器写
}