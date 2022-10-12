/*****************************************************************************
*
* File Name : wm_demo_console_task.c
*
* Description: demo console task
*
* Copyright (c) 2014 Winner Micro Electronic Design Co., Ltd.
* All rights reserved.
*****************************************************************************/
#include <string.h>
#include "wm_include.h"
#include "wm_demo.h"
#include "st7789.h"
#include "ascii_fonts.h"
#include "alenlogo.h"

#if DEMO_CONSOLE
#include "wm_demo_console.h"

#define    DEMO_TASK_SIZE      2048
static tls_os_queue_t 	*demo_q = NULL;

static tls_os_queue_t 	*demo_q2 = NULL;

static OS_STK 			DemoTaskStk[DEMO_TASK_SIZE];

static OS_STK 			DemoTaskStk2[DEMO_TASK_SIZE];

static Demo_Console 	gstConsole;
#define DEMO_CONSOLE_BUF_SIZE   512

extern int strtodec(int *dec, char *str);


void demo_console_malloc(void)
{
    gstConsole.rx_buf = tls_mem_alloc(DEMO_CONSOLE_BUF_SIZE + 1);
    if(NULL == gstConsole.rx_buf)
    {
        printf("\nmalloc rx fail\n");
        return;
    }
    memset(gstConsole.rx_buf, 0, DEMO_CONSOLE_BUF_SIZE + 1);
}

s16 demo_console_rx(u16 len, void* user_data)
{
    gstConsole.rx_data_len += len;

    if (gstConsole.MsgNum < 3)
    {
        gstConsole.MsgNum ++;
        tls_os_queue_send(demo_q, (void *)1, 0);
    }

    return 0;
}

char *demo_cmd_get_first_comma(char *buf, int len)
{
    char prec = '\0', curc;
    int n = 0;
    if(len <= 0)
        return NULL;
    if(*buf == '"')
    {
        for(n = 1; n < len; n++)
        {
            curc = *(buf + n);
            if(curc == ',' && prec == '"')
            {
                if(n < 3 || *(buf + n - 2) != '\\')
                {
                    return buf + n;
                }
            }
            prec = curc;
        }
        return NULL;
    }
    else
        return strchr(buf, ',');
}

int demo_cmd_param(void *param, int type)
{
    if (type == 0)
    {
        return (int)((char *)param);
    }
    else
    {
        return *(int *)param;
    }
}

int demo_cmd_quotation_filter(u8 **keyInfo, u8 *inbuf)
{
    u8 len = strlen((char *)inbuf);
    int i;
    if (*inbuf == '"')
    {
        /* argument such as  "xxxx" */
        inbuf++; /* skip 1st <"> */
        len -= 1;
        *keyInfo = inbuf;
        if((*(inbuf + len - 1) == '"') && (*(inbuf + len) == '\0'))
        {
            *(inbuf + len - 1) = '\0';
            len -= 1;
            for(i = 0; i < len; i++)
            {
                if(inbuf[i] == '\\')
                {
                    len -= 1;
                    memcpy(inbuf + i, inbuf + i + 1, len - i);
                }
            }
            inbuf[len] = '\0';
        }
        else
        {
            return 1;
        }
    }
    else
    {
        *keyInfo = inbuf;
    }

    return 0;
}

static int demo_call_fn(int (*fn)(), int *param, int count)
{
    int ret;

    if(NULL == fn || NULL == param)
    {
        return WM_FAILED;
    }

    switch(count)
    {
    case 0:
        ret = (int)fn();
        break;
    case 1:
        ret = (int)fn(param[0]);
        break;
    case 2:
        ret = (int)fn(param[0], param[1]);
        break;
    case 3:
        ret = (int)fn(param[0], param[1], param[2]);
        break;
    case 4:
        ret = (int)fn(param[0], param[1], param[2], param[3]);
        break;
    case 5:
        ret = (int)fn(param[0], param[1], param[2], param[3], param[4]);
        break;
    case 6:
        ret = (int)fn(param[0], param[1], param[2], param[3], param[4], param[5]);
        break;
    case 7:
        ret = (int)fn(param[0], param[1], param[2], param[3], param[4], param[5], param[6]);
        break;
    case 8:
        ret = (int)fn(param[0], param[1], param[2], param[3], param[4], param[5], param[6], param[7]);
        break;
    case 9:
        ret = (int)fn(param[0], param[1], param[2], param[3], param[4], param[5], param[6], param[7], param[8]);
        break;
    case 10:
        ret = (int)fn(param[0], param[1], param[2], param[3], param[4], param[5], param[6], param[7], param[8], param[9]);
        break;
    default:
        ret = (int)fn( );
        break;
    }

    return ret;
}

int demo_cmd_execute(Demo_Console *sys)
{
    int ifcmd = 0;
    int i = 0;
    int j = 0;
    int remain_len;
    int ret = 0;

#define   MAX_DEMO_ARG 20
    int   param[MAX_DEMO_ARG];
    int   arg_count = 0;

    u8 *ptmp_param = NULL;

    u8 *buf = NULL;
	u8 *pparam_equal = NULL;
    u8 *pparam_begin = NULL;
    u8 *pparam_end = NULL;
    u8 *comma;
    u8 *arg[MAX_DEMO_ARG] = {NULL};
    int len;
	u8 *strfirst = NULL;
	u8 *str_r = NULL;
	u8 *str_n = NULL;

    for(i = 0; ; i++)
    {
    	strfirst = (u8 *)strstr((char *)sys->rx_buf, console_tbl[i].cmd);	
        if (strfirst != NULL)
        {
			/*remove \r\n from input string*/
			str_r = (u8 *)strchr((char *)strfirst, '\r');
			str_n = (u8 *)strchr((char *)strfirst, '\n');			
			if (str_r&&(str_n == NULL))
			{
				if (str_r > strfirst)
				{
					strfirst[str_r - strfirst] = '\0';
				}
			}
			else if ((str_r == NULL)&&str_n)
			{
				if (str_n > strfirst)
				{
					strfirst[str_n - strfirst] = '\0';
				}				
			}
			else if (str_r && str_n)
			{
				if (((str_r > str_n) && (str_r > strfirst))
					||((str_r < str_n) && (str_n > strfirst)))
				{
					strfirst[str_n - strfirst] = '\0';
				}
			}

            /*parser()*/
			pparam_equal = (u8 *)strchr((char *)(sys->rx_buf + strlen(console_tbl[i].cmd)), '=');
			pparam_begin = (u8 *)strchr((char *)(sys->rx_buf + strlen(console_tbl[i].cmd)), '(');
			if (pparam_equal)
			{
				if (pparam_begin && (pparam_begin > pparam_equal))
				{
					if (pparam_equal - strfirst > strlen(console_tbl[i].cmd))
					{
						continue;
					}
				}
			}
			else
			{
				if (pparam_begin && (pparam_begin - strfirst > strlen(console_tbl[i].cmd)))
				{
					continue;
				}

				/*if no '(', compare the cmd string with input string*/
				if (!pparam_begin && (strlen((char *)strfirst) != strlen(console_tbl[i].cmd)))
				{
					continue;
				}
			}

            pparam_end = (u8 *)strchr((char *)(pparam_begin + 1), ')');
            if (!pparam_begin && !pparam_end)
            {
                /*No Parameter,use default parameter to execute*/
                printf("[CMD]%s", console_tbl[i].cmd);
                for (j = 0; j < console_tbl[i].param_cnt; j++)
                {
                    if (!((console_tbl[i].type >> j) & 0x1))
                    {
                        param[j] = (int)NULL;
                    }
                    else
                    {
                        param[j] = -1;
                    }
                }
                ret = demo_call_fn((int (*)())console_tbl[i].callfn, param, console_tbl[i].param_cnt);
                if(WM_FAILED == ret)
                {
                    printf("\nrun demo failed\n");
                }
                return DEMO_CONSOLE_CMD;
            }
            else if (pparam_begin && pparam_end && ((pparam_end - pparam_begin) > 0))
            {
                remain_len =  pparam_end - pparam_begin;
                buf = pparam_begin + 1;
                arg[0] = buf;
                arg_count = 0;
                *(u8 *)pparam_end = '\0';
                while (remain_len > 0)
                {
                    comma = (u8 *)demo_cmd_get_first_comma((char *)buf, remain_len);
                    if (pparam_end && !comma)
                    {
                        if (arg_count >= (console_tbl[i].param_cnt - 1))
                            break;
                        /* last parameter */
                        *(u8 *)pparam_end = '\0';
                        remain_len -= (pparam_end - buf);
                        if(remain_len <= 1)
                            break;
                        if (pparam_end != buf)
                            arg_count++;
                        arg[arg_count] = pparam_end + 1;
                    }
                    else
                    {
                        *(u8 *)comma = '\0';
                        if (arg_count >= (console_tbl[i].param_cnt - 1))
                            break;
                        arg_count++;
                        arg[arg_count] = comma + 1;
                        remain_len -= (comma - buf + 1);
                        buf = comma + 1;
                    }
                }
                for (j = 0; j <= arg_count; j++)
                {
                    while(' ' == *(arg[j]))
                    {
                        arg[j] ++;
                    }
                    len = strlen((char *)arg[j]);
                    while(len > 0 && ' ' == *(arg[j] + len - 1))
                    {
                        *(arg[j] + len - 1) = 0;
                        len --;
                    }
                    if (!((console_tbl[i].type >> j) & 0x1))
                    {
                        if(0 == len)
                            param[j] = (int)NULL;
                        else
                        {
                            demo_cmd_quotation_filter(&ptmp_param, arg[j]);
                            param[j] = (int)ptmp_param;
                        }
                    }
                    else
                    {
                        if(0 == len)
                            param[j] = -1;
                        else
                        {
                            if (strtodec(&param[j], (char *)arg[j]) < 0)
                            {
                                printf("parameter err\r\n");
                                return DEMO_CONSOLE_WRONG_CMD;
                            }
                        }
                    }
                }

                for (j = arg_count + 1; j < console_tbl[i].param_cnt; j++)
                {
                    if (!((console_tbl[i].type >> j) & 0x1))
                    {
                        param[j] = (int)NULL;
                    }
                    else
                    {
                        param[j] = -1;
                    }
                }

                ret = demo_call_fn((int (*)())console_tbl[i].callfn, param, console_tbl[i].param_cnt);
                if(WM_FAILED == ret)
                {
                    printf("\nrun demo failed\n");
                }

                /*Use input param to execute function*/
                ifcmd = DEMO_CONSOLE_CMD;
                break;
            }
            else if (pparam_begin && !pparam_end)
            {
                return DEMO_CONSOLE_SHORT_CMD;
            }
            else
            {
                /*wrong cmd parameter,discard this cmd*/
                printf("\nwrong cmd param\n");
                //demo_console_show_help(NULL);
                return DEMO_CONSOLE_WRONG_CMD;
            }
        }

        if(strstr(console_tbl[i].cmd, "lastcmd") != NULL)	//last command
        {
            /*wrong cmd parameter,discard this cmd*/
            //demo_console_show_help(NULL);
            return DEMO_CONSOLE_WRONG_CMD;
        }
    }

    return ifcmd;
}


//console task use UART0 as communication port with PC
void demo_console_task(void *sdata)
{
    void *msg;
    int ret = 0;

    demo_console_show_help(NULL);
    demo_console_malloc();
    gstConsole.rptr = 0;
    tls_uart_set_baud_rate(TLS_UART_0, 115200);
	tls_uart_rx_callback_register(TLS_UART_0, demo_console_rx, NULL);


    //  ST7789_LCD_Init();
	//  ST7789_Fill(0, 0, 240, 240, BRED);

    ST7789_Init();
    // ILI9341_Clear(WHITE);
    int i =0;
    // for (i=0;i<2;i++) {
    //     ST7789_Fill(0,0,119,119,BLACK);
    //     ST7789_Fill(120,120,239,239,WHITE);
    //     ST7789_Fill(0,119,119,239,RED);
    //     ST7789_Fill(120,0,239,199,BLUE);

    //     ST7789_Fill(0,119,119,239,BLACK);
    //     ST7789_Fill(120,0,239,199,WHITE);
    //     ST7789_Fill(0,0,119,119,RED);
    //     ST7789_Fill(120,120,239,239,BLUE);
    // }

    ST7789_Fill(0,0, 240, 240, BLUE);

    //ST7789_Show_Image(0, 0, 240, 82, ALIENTEK_LOGO);
    // ST7789_Show_Image(0, 0, 240, 150, gImage_AA);
    
    //初始化，连结WIFI
	#if DEMO_CONNECT_NET
		demo_connect_net("MyGirl666", "yyyys215615@...");
	#endif
    ST7789_Fill(0,0, 240, 240, WHITE);

    ST7789_Clear(BLACK);

    // ST7789_DrawChar(0, 2, 'A', &Font_16x32, WHITE, BLACK);
    // ST7789_DrawChar(0, 34, 'A', &Font_11x18, WHITE, BLACK);
    // ST7789_DrawChar(0, 56, 'A', &Font_12x24, WHITE, BLACK);
    ST7789_DrawString(0, 70, "12345678", &Font_11x18, WHITE, BLACK);
    //ST7789_DrawImage(5, 5, 240, 150, gImage_AA);
    //LCD_ShowChar(0, 110, 'A', 24);
    for(;;)
    {   
        //t-httpfwup=(http://192.168.8.122:1234/assets/W800_ota.img)
            //ST77XX_DrawChar(0, 0, 'A', &Font_8x16, BLACK, WHITE);
            //ST77XX_DrawString(0, 40, "ShuaiAn ECBC", &Font_8x16, RED, WHITE);
            // ST7789_Fill(0,0, 240, 240, BLUE);
            // ST7789_Fill(0,0, 240, 240, BLACK);
            // ST7789_Fill(0,0, 240, 240, BRED);
            // ST7789_Fill(0,0, 240, 240, GRED);
            // ST7789_Fill(0,0, 240, 240, GBLUE);
            // ST7789_Fill(0,0, 240, 240, RED);
            // ST7789_Fill(0,0, 240, 240, MAGENTA);
            // ST7789_Fill(0,0, 240, 240, GREEN);
            // ST7789_Fill(0,0, 240, 240, CYAN);
            // ST7789_Fill(0,0, 240, 240, YELLOW);
            // ST7789_Fill(0,0, 240, 240, BRRED);

            // BLACK	
            // BLUE	
            // BRED	
            // GRED	
            // GBLUE	
            // RED		
            // MAGENTA	
            // GREEN	
            // CYAN	
            // YELLOW	
            // BROWN	
            // BRRED	
            // GRAY	

        tls_os_queue_receive(demo_q, (void **)&msg, 0, 0);

        ST7789_Clear(BLACK);
       // ST7789_DrawString(0, 10, "78787878", &Font_11x18, WHITE, BLACK);

        switch((u32)msg)
        {
        case 1:
            ret = tls_uart_read(TLS_UART_0, gstConsole.rx_buf + gstConsole.rptr, gstConsole.rx_data_len);

            //ST7789_Clear(*gstConsole.rx_buf);
            //ST7789_DrawString(0, 20, gstConsole.rx_buf, &Font_12x24, WHITE, BLACK);

            if(ret <= 0)
                break;
            gstConsole.rx_data_len -= ret;
            gstConsole.rptr += ret;
            ret = demo_cmd_execute(&gstConsole);	//parse command and execute if needed
            if((DEMO_CONSOLE_CMD == ret) || (DEMO_CONSOLE_WRONG_CMD == ret))	
            {
                /*modify*/
                memset(gstConsole.rx_buf, 0, DEMO_CONSOLE_BUF_SIZE);	/*After command finished transfering, clear buffer*/
                gstConsole.rptr = 0;
            }
            else if(DEMO_CONSOLE_SHORT_CMD == ret)
            {
                //param not passed all, do nothing.
            }

            if(gstConsole.MsgNum)
                gstConsole.MsgNum --;
            break;
        
        default:
            break;
        }
    }
}

//t-httpfwup=(http://192.168.8.122:1234/assets/W800_ota.img)
void CreateDemoTask(void)
{
    tls_os_queue_create(&demo_q, DEMO_QUEUE_SIZE);
    tls_os_task_create(NULL, NULL,
                       demo_console_task,
                       NULL,
                       (void *)DemoTaskStk,          /* task's stack start address */
                       DEMO_TASK_SIZE * sizeof(u32), /* task's stack size, unit:byte */
                       DEMO_TASK_PRIO,
                       0);
}

#endif	//DEMO_CONSOLE

