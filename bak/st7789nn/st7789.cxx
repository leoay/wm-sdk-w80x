/******************************************************************************
** 
 * \file        st7789.c
 * \author      IOsetting | iosetting@outlook.com
 * \date        
 * \brief       Library of ST7789/ST7789V TFT LCD on W806
 * \note        
 * \version     v0.1
 * \ingroup     demo
 * \remarks     test-board: HLK-W806-KIT-V1.0
 *
******************************************************************************/

#include "st7789.h"

static uint8_t st7789_buf[ST7789_BUF_SIZE];
static uint16_t st7789_buf_pt = 0;

static bool isdata = 0;

void HAL_GPIO_WritePin(u16 gpiox_num, GPIO_PinState pin_state) {
    //tls_gpio_cfg(gpiox_num, WM_GPIO_DIR_OUTPUT, WM_GPIO_ATTR_PULLHIGH);
    tls_gpio_write(gpiox_num, pin_state);
}

void HAL_Delay(int ms)
{
    usleep(ms*1000);
}

static int ST7789_SPI_init()
{
	int clk=40000000;
	int type=2;

	wm_spi_ck_config(ST7789_CK);
	wm_spi_do_config(ST7789_MO);

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


void ST7789_Reset(void)
{
    ST7789_RESET_LOW;
    HAL_Delay(20);
    ST7789_RESET_HIGH;
    HAL_Delay(20);
}


static void ST7789_GPIO_CMD(void)
{
	if(isdata)
	{
		ST7789_DC_LOW;
		isdata=0;
	}
}

static void ST7789_GPIO_DATA(void)
{
	if(!isdata)
	{
		ST7789_DC_HIGH;
		isdata=1;
	}
}


void ST7789_GPIO_Init()
{
    HAL_GPIO_WritePin(ST7789_RES, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ST7789_DC, GPIO_PIN_RESET);
    ST7789_Reset();
    ST7789_SPI_init();
}

static void ST7789_TransmitByte(uint8_t dat)
{
    tls_spi_write(&dat, 1);
}

static void ST7789_Transmit(const uint8_t *pData, uint32_t Size)
{
    tls_spi_write(&pData, Size);
}

static void ST7789_WriteCommand(uint8_t dat)
{
    ST7789_DC_LOW;
    ST7789_TransmitByte(dat);
}

void ST7789_WR_REG(unsigned char dat)
{
	ST7789_GPIO_CMD();//写命令
	tls_spi_write(&dat,1);
}

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

void ST7789_WR_DATA8(unsigned char dat)
{
	ST7789_GPIO_DATA();//写数据
	tls_spi_write(&dat,1);
}

static void ST7789_SetAddrWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    ST7789_WriteCommand(0x2a);
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

void ST7789_Fill(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color)
{
    uint16_t i,j;
    ST7789_SetAddrWindow(x_start, y_start, x_end - 1, y_end - 1);
    for(i = y_start; i < y_end; i++)
    {
        for( j = x_start; j < x_end; j++)
        {
            ST7789_WriteBuff((uint8_t *)&color, 2);
        }
    }
    ST7789_FlushBuff();
}


void ST7789_LCD_Init() {
    ST7789_GPIO_Init();

    HAL_Delay(120);
    /* Sleep Out */

        /* Positive Voltage Gamma Control */
    ST7789_WR_REG(0xE0);

    ST7789_WR_DATA8(0x0F);
    ST7789_WR_DATA8(0x22);
    ST7789_WR_DATA8(0x1C);
    ST7789_WR_DATA8(0x1B);
    ST7789_WR_DATA8(0x08);
    ST7789_WR_DATA8(0x0F);
    ST7789_WR_DATA8(0x48);
    ST7789_WR_DATA8(0xB8);
    ST7789_WR_DATA8(0x34);
    ST7789_WR_DATA8(0x05);
    ST7789_WR_DATA8(0x0C);
    ST7789_WR_DATA8(0x09);
    ST7789_WR_DATA8(0x0F);
    ST7789_WR_DATA8(0x07);
    ST7789_WR_DATA8(0x00);

    /* Negative Voltage Gamma Control */
    ST7789_WR_REG(0xE1);

    ST7789_WR_DATA8(0x00);
    ST7789_WR_DATA8(0x23);
    ST7789_WR_DATA8(0x24);
    ST7789_WR_DATA8(0x07);
    ST7789_WR_DATA8(0x10);
    ST7789_WR_DATA8(0x07);
    ST7789_WR_DATA8(0x38);
    ST7789_WR_DATA8(0x47);
    ST7789_WR_DATA8(0x4B);
    ST7789_WR_DATA8(0x0A);
    ST7789_WR_DATA8(0x13);
    ST7789_WR_DATA8(0x06);
    ST7789_WR_DATA8(0x30);
    ST7789_WR_DATA8(0x38);
    ST7789_WR_DATA8(0x0F);

    ST7789_WR_REG(0x11);
    /* wait for power stability */
    HAL_Delay(120);

    /* Memory Data Access Control */
    ST7789_WR_REG(0x36);
    ST7789_WR_DATA8(0x00);

    /* RGB 5-6-5-bit  */
    ST7789_WR_REG(0x3A);
    ST7789_WR_DATA8(0x65);

    /* Porch Setting */
    ST7789_WR_REG(0xB2);
    ST7789_WR_DATA8(0x0C);
    ST7789_WR_DATA8(0x0C);
    ST7789_WR_DATA8(0x00);
    ST7789_WR_DATA8(0x33);
    ST7789_WR_DATA8(0x33);

    /*  Gate Control */
    ST7789_WR_REG(0xB7);
    ST7789_WR_DATA8(0x72);

    /* VCOM Setting */
    ST7789_WR_REG(0xBB);
    ST7789_WR_DATA8(0x3D);   //Vcom=1.625V

    /* LCM Control */
    ST7789_WR_REG(0xC0);
    ST7789_WR_DATA8(0x2C);

    /* VDV and VRH Command Enable */
    ST7789_WR_REG(0xC2);
    ST7789_WR_DATA8(0x01);

    /* VRH Set */
    ST7789_WR_REG(0xC3);
    ST7789_WR_DATA8(0x19);

    /* VDV Set */
    ST7789_WR_REG(0xC4);
    ST7789_WR_DATA8(0x20);

    /* Frame Rate Control in Normal Mode */
    ST7789_WR_REG(0xC6);
    ST7789_WR_DATA8(0x0F);	//60MHZ

    /* Power Control 1 */
    ST7789_WR_REG(0xD0);
    ST7789_WR_DATA8(0xA4);
    ST7789_WR_DATA8(0xA1);



    /* Display Inversion On */
    ST7789_WR_REG(0x21);

    ST7789_WR_REG(0x29);

    ST7789_SetAddrWindow(0, 0, ST7789_WIDTH - 1, ST7789_HEIGHT - 1);
    
    //LCD_Clear(WHITE);
}
