#include <string.h>
#include "wm_include.h"
#include "wm_demo.h"
#include "wm_rtc.h"

#include "st7789.h"
#include "ascii_fonts.h"

#if DEMO_RTC

static void demo_rtc_clock_irq(void *arg)
{
    struct tm tblock;
    tls_get_rtc(&tblock);
    printf("rtc clock, sec=%d,min=%d,hour=%d,mon=%d,year=%d\n", tblock.tm_sec, tblock.tm_min, tblock.tm_hour, tblock.tm_mon, tblock.tm_year);
}


int rtc_demo(void)
{
    struct tm tblock2;
    unsigned int t;	//used to save time relative to 1970
    struct tm *tblock;
    struct tm tblock1;
    // tblock.tm_year = 17;
    // tblock.tm_mon = 11;
    // tblock.tm_mday = 20;
    // tblock.tm_hour = 14;
    // tblock.tm_min = 30;
    // tblock.tm_sec = 0;
    t = tls_ntp_client();
    t = tls_ntp_client();
    t = tls_ntp_client();

    printf("now Time :   %s\n", ctime((const time_t *)&t));
    tblock = localtime((const time_t *)&t);	//switch to local time
    printf(" sec=%d,min=%d,hour=%d,mon=%d,year=%d\n",tblock->tm_sec,tblock->tm_min,tblock->tm_hour,tblock->tm_mon,tblock->tm_year);

    printf("now Time :   %s\n", gmtime((const time_t *)&t));
    // tblock = localtime((const time_t *)&t);	//switch to local time
    // printf(" sec=%d,min=%d,hour=%d,mon=%d,year=%d\n",tblock->tm_sec,tblock->tm_min,tblock->tm_hour,tblock->tm_mon,tblock->tm_year);
     char buffer[80];
    tblock = localtime((const time_t *)&t);
    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", tblock);
    printf("格式化的日期 & 时间 : |%s|\n", buffer );

    //char year[4];
    //year[3] = buffer[0];

    char year[10];
    (char*)memcpy(year, buffer, 4);

    char month[10];
    (char*)memcpy(month, &buffer[5], 4);

    char day[10];
    (char*)memcpy(day, &buffer[8], 4);

    char hour[10];
    (char*)memcpy(hour, &buffer[11], 4);

    char min[10];
    (char*)memcpy(min, &buffer[14], 4);

     char sec[10];
    (char*)memcpy(sec, &buffer[17], 4);

    int yearint = atoi(year);
    int monint = atoi(month);
    int mdayint = atoi(day);
    int hourrint = atoi(hour);
    int minint = atoi(min);
    int secint = atoi(sec);


    printf("=================%d, %02d, %02d, %02d, %02d, %02d\n",  yearint, monint, mdayint, hourrint, minint, secint);

    tblock1.tm_year =  yearint - 1900;
    tblock1.tm_mon =   monint;
    tblock1.tm_mday =  atoi(day);
    tblock1.tm_hour =  atoi(hour);
    tblock1.tm_min =   atoi(min);
    tblock1.tm_sec =   atoi(sec);

    tls_set_rtc(&tblock1);
    tls_rtc_isr_register(demo_rtc_clock_irq, NULL);
    tls_rtc_timer_start(&tblock1);

    char ch[10];

    while(1)
    {
        tls_os_time_delay(500);
        tls_get_rtc(&tblock2);
        printf("rtc cnt, sec=%02d,min=%02d,hour=%02d, day=%02d, mon=%02d,year=%d\n", tblock2.tm_sec, tblock2.tm_min, tblock2.tm_hour, tblock2.tm_mday, tblock2.tm_mon, tblock2.tm_year+1900);
        
        snprintf(year, sizeof(year), "%d", tblock2.tm_year+1900);
        snprintf(month, sizeof(month), "%02d", tblock2.tm_mon);
        snprintf(day, sizeof(day), "%02d", tblock2.tm_mday);
        snprintf(hour, sizeof(hour), "%02d", tblock2.tm_hour);
        snprintf(min, sizeof(min), "%02d", tblock2.tm_min);
        snprintf(sec, sizeof(sec), "%02d", tblock2.tm_sec);

        ST7789_DrawString(0,   0, year, &Font_11x18, WHITE, BLACK);
        ST7789_DrawString(45,   0, "-", &Font_11x18, WHITE, BLACK);
        ST7789_DrawString(57,  0, month, &Font_11x18, WHITE, BLACK);
        ST7789_DrawString(80,   0, "-", &Font_11x18, WHITE, BLACK);
        ST7789_DrawString(103, 0, day, &Font_11x18, WHITE, BLACK);

        ST7789_DrawString(108, 0, hour, &Font_11x18, WHITE, BLACK);
        ST7789_DrawString(131, 0, min, &Font_11x18, WHITE, BLACK);
        ST7789_DrawString(154, 0, sec, &Font_11x18, WHITE, BLACK);
    }
    return WM_SUCCESS;
}

#endif



