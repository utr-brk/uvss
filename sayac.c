/*
 * sayac.c
 *
 *  Created on: 10 Haz 2014
 *      Author: Burak
 */


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
//#include <dos.h>
#include "sayac.h"
//#include "grlcm.h"
#include "lcd.h"
#include "config.h"
#include "time.h"
#include "main.h"

struct_SAYAC SAYAC;


void SAYAC_Kontrol(void)
{
    //struct date d;
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
//    getdate(&d);

    if ((timeinfo->tm_year!=SAYAC.ZAMAN.da_year) ||
            (timeinfo->tm_mon!=SAYAC.ZAMAN.da_mon) ||
            (timeinfo->tm_mday!=SAYAC.ZAMAN.da_day) )
        //if ((d.da_year!=SAYAC.ZAMAN.da_year) ||
        //    (d.da_mon!=SAYAC.ZAMAN.da_mon) ||
        //    (d.da_day!=SAYAC.ZAMAN.da_day) )
    {
        SAYAC.SAYI=0;
    }
}

void SAYAC_Arti(void)
{
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    SAYAC_Kontrol();
    SAYAC.SAYI++;
    //getdate(&SAYAC.ZAMAN);
    SAYAC.ZAMAN.da_day = timeinfo->tm_mday;
    SAYAC.ZAMAN.da_mon = timeinfo->tm_mon;
    SAYAC.ZAMAN.da_year = timeinfo->tm_year;
    SAYAC_Save();
}

void SAYAC_Load(void)
{
    FILE *inp;
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    //1.1.2
    //LOG dosyaları ve sayac dosyası oluşturulurken absolute path girilmemişti,
    //hepsinin başına /home/odroid/Solidus/ eklendi
    inp=fopen("/home/odroid/Solidus/syc.dat","rb");
    if  (inp==NULL)
    {
        SAYAC.SAYI = 0;
        //getdate(&SAYAC.ZAMAN);
        SAYAC.ZAMAN.da_day = timeinfo->tm_mday;
        SAYAC.ZAMAN.da_mon = timeinfo->tm_mon;
        SAYAC.ZAMAN.da_year = timeinfo->tm_year;
        SAYAC_Save();
    }
    else
    {
        fread(&SAYAC,sizeof(SAYAC),1,inp);
        SAYAC_Kontrol();
        fclose(inp);
    }

}

void SAYAC_Save(void)
{
    FILE *inp;
    //remove("syc.dat");
    inp=fopen("/home/odroid/Solidus/syc.dat","w+b");
    if (inp!=NULL)
    {
        fseek(inp,0L,SEEK_SET);
        fwrite(&SAYAC,sizeof(SAYAC),1,inp);
        fclose(inp);
    }
}

void SAYAC_Show(void)
{
    //unsigned char *szSayac; //m8210
    char *szSayac;

    if ( (rec_TERM.TERM_TIP==TERM_YEMEKHANE)&&
            (rec_TERM.LOG_ALARM_SAAT==3) )
    {
        //szSayac  = (unsigned char*) calloc(10,1); //m8210
        szSayac  = (char*) calloc(10,1);
        sprintf(szSayac,"%u",SAYAC.SAYI);
        lcd_printf_ex(ALG_LEFT, 1, "%s", szSayac);
        lcd_flip();
        //m8210 burasý için baþka birþey yapýlmalý
        //GrLcm_ClearRect(0,0,strlen(szSayac)*6,7);
        //GrLcm_printf(0,0,"%s",szSayac);
        free(szSayac);
    }
}
