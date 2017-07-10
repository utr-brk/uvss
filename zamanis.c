/*
 * zamanis.c
 *
 *  Created on: 10 Haz 2014
 *      Author: Burak
 */

//#include <dos.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "config.h"
#include "lcd.h"
#include "log.h"
#include "mesaj.h"
#include "tcp.h"
#include "ftp.h"
#include "gunisigi.h"
#include "zamanis.h"
#include "sayac.h"
#include "personel.h"
//0.0.1
//#include "watchdt.h"
#include "main.h"
#include "log.h"
#include "personel.h"
#include "tool.h"
//6.0.0
#include "minini.h"
#define sizearray(a)	  (sizeof(a) / sizeof((a)[0]))

unsigned char z_ONLINE_TEKRAR_DK=0;
unsigned char z_TS_GUNCELLE_SAAT=0;
unsigned char z_LOG_KART_SAAT=0;
unsigned char z_LOG_ALARM_SAAT=0;
unsigned char z_LOG_TERM_SAAT=0;
unsigned char z_MSJ_SURE_SN=0;

unsigned long son_Saat=0;
unsigned long son_Dakika=0;
unsigned long son_Saniye=0;

unsigned long z_syc_sn=0; //0.0.6
//2.4.0
//unsigned int  z_log_sn=10;//0.0.6
unsigned int  z_log_sn=5;//0.0.6
struct_SEANS_BILGI rec_Seans;

void ZAMANIS_Init(void)
{
    z_ONLINE_TEKRAR_DK=0;
    z_TS_GUNCELLE_SAAT=0;
    z_LOG_KART_SAAT=0;
    z_LOG_ALARM_SAAT=0;
    z_LOG_TERM_SAAT=0;
    z_MSJ_SURE_SN=0;
    son_Saat=0;
    son_Dakika=0;
    son_Saniye=0;
}

//void Is_Saniye(struct dosdate_t * d,struct dostime_t * t)
void Is_Saniye(struct tm * timeinfo)
{
//gchar status_lbl[50];
    if (MSJ_OK==0x00) z_MSJ_SURE_SN++; //Logo Gosterilmiyorsa
    if (z_MSJ_SURE_SN>rec_TERM.MSJ_SURE_SN)
    {
        MSJ_Goster(MSJ_TIP_LG);
        z_MSJ_SURE_SN=0;
        lcd_printf_ex(ALG_CENTER, 16,"%02d/%02d/%04d, %02d:%02d", timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min); //Her Dakika Zaman Goster
        lcd_printf_ex(ALG_CENTER, 17, "www.utarit.com  2017");
        lcd_flip();
        //0.50
        //sprintf(status_lbl, "%02d/%02d/%04d     www.utarit.com  ©2017     %02d:%02d", timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min); //Her Dakika Zaman Goster)
        //0.54
        //gtk_printf(label_status, "%02d/%02d/%04d     www.utarit.com  ©2017     %02d:%02d", timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min);
        gtk_printf(label_status, "%02d/%02d/%04d       www.utarit.com ©2017       %02d:%02d", timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min);
        gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_WARNING);//GTK_MESSAGE_ERROR
        //0.54
        while(gtk_events_pending())
            gtk_main_iteration();
    }
    //LCD_TarihSaat(7,d,t); //Her Saniye Zaman Goster

    //lcd_clean();//0.0.8
    //MSJ_Goster(MSJ_TIP_LG);
    //lcd_printf_ex(ALG_CENTER, 160," %02d/%02d/%04d, %02d:%02d:%02d", timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec); //Her Saniye Zaman Goster
    //lcd_flip();
    SAYAC_Show();
    z_syc_sn++;
    if((z_syc_sn%z_log_sn)==0)
    {
        switch (rec_TERM.TERM_TIP)
        {
        case TERM_PARA:
            //debug_print("SYC_SN: %d, flag_LOG_PARA: %d", z_syc_sn, flag_LOG_PARA);
            LOG_Run_Para(); //0.0.5 Para logu gonder
            if(flag_LOG_PARA)
                z_log_sn=5;//0.0.6 10 sn idi.
            else
                z_log_sn=60;
            //debug_print("log_sn: %d", z_log_sn);
            break;
        case TERM_KAPI:
            //2.4.0
            //üç log gönderir hale getirdim.
            Log_Run_Kapi();
            Log_Run_Kapi();
            Log_Run_Kapi();

            break;
        default:
            //debug_print("SYC_SN: %d, flag_LOG_KART: %d", z_syc_sn, flag_LOG_KART);
            LOG_Run();

            if(flag_LOG_KART)
                z_log_sn=5; //0.0.6 10 sn idi.
            else
                z_log_sn=60;
            //debug_print("SYC_SN: %d, flag_LOG_KART: %d", z_log_sn, flag_LOG_KART);
            break;
        }
        z_syc_sn = 0;//0.0.5
    }
}

//void Is_Dakika(dosdate_t * d, dostime_t * t)
void Is_Dakika(struct tm * timeinfo)
{
    char inifile[13];
    char dd;

    int s;
    char sect[40], grup_kod[256];
//unsigned char chr_suan[5], chr_start[5], chr_end[5]; //m8210
    char chr_suan[5], chr_start[5], chr_end[5];
    int  i;
    int start, end;

    int suan;
    int found = 0;
    char cinsiyet[10];
    char *ptr;
//gchar status_lbl[50];

    lcd_clean();
    //0.50
    gtk_clean();
    MSJ_Goster(MSJ_TIP_LG);
    lcd_printf_ex(ALG_CENTER, 16,"%02d/%02d/%04d, %02d:%02d", timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min); //Her Dakika Zaman Goster
    lcd_printf_ex(ALG_CENTER, 17, "www.utarit.com  2017");
    lcd_flip();
    //0.50
    //sprintf(status_lbl, "%02d/%02d/%04d     www.utarit.com  ©2017     %02d:%02d", timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min); //Her Dakika Zaman Goster)
    //0.54
    //gtk_printf(label_status, "%02d/%02d/%04d     www.utarit.com  ©2017     %02d:%02d", timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min);
    gtk_printf(label_status, "%02d/%02d/%04d       www.utarit.com ©2017       %02d:%02d", timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min);
    gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_WARNING);//GTK_MESSAGE_ERROR
    //0.54
    while(gtk_events_pending())
        gtk_main_iteration();
    if(rec_HAVUZ.AKTIF) //6.1.0
    {
        memset(inifile, 0, sizeof(inifile));
        memset(cinsiyet, 0, sizeof(cinsiyet));

        //dd = d->dayofweek;
        dd = timeinfo->tm_wday;

        //itoa((int) dd, inifile, 10);
        sprintf(inifile, "%d", dd);
        strcat(inifile, ".INI");

        //debug_print("inifile: %s", inifile);

        //sprintf(chr_suan, "%02d%02d", t->hour, t->minute);
        sprintf(chr_suan, "%02d%02d", timeinfo->tm_hour, timeinfo->tm_min);
        suan = atoi(chr_suan);

        //aslinda bunu her dk yapmanin anlami yok,
        //bastan bir kere yapilsa, found = 1 olsa,
        //sonra dersin suresi ve yoklama suresi global bir degiskende tutularak
        //sadece gecen zaman kontrol edilse,

        //debug_print("suan: %d", suan);
        //for(s=0; ini_getsection(s, sect, sizeof(sect), inifile) > 0; s++){
        for(s=0; ini_getsection(s, sect, 40, inifile) > 0; s++)
        {
            //debug_print("for icinde");
            //debug_print("sect: [%s]", sect);
            strncpy(chr_start, sect, 4);
            //0.1.1
            //04.03.2015
            //yukarıda size'ı 5 olarak tanımlı, ama 6.ya yazmaya uğraşıyoruz.
            //chr_start[5] = '\x0';
            chr_start[4] = '\x0';
            strncpy(chr_end, sect+5, 4);
            //0.1.1
            //04.03.2015
            //yukarıda size'ı 5 olarak tanımlı, ama 6.ya yazmaya uğraşıyoruz.
            //chr_end[5] = '\x0';
            chr_end[4] = '\x0';
            start = atoi(chr_start);
            end = atoi(chr_end);
            //debug_print("suan: %d, start: %d, end: %d", suan, start, end);

            if((suan >= start) && (suan <=end))
            {
                rec_Seans.ID  = (int) ini_getl(sect, "ID", 0, inifile);
                ini_gets(sect, "GRUP_KODLARI", "xxx DERS YOK xxx", grup_kod, sizearray(grup_kod), inifile);
                //			ini_gets(sect, "CINSIYET", "xxxx", cinsiyet, sizearray(cinsiyet), inifile);
                ini_gets(sect, "CINSIYET", "xxxx", rec_Seans.CINSIYET, sizearray(rec_Seans.CINSIYET), inifile);
                //rec_HAVUZ.FIYAT = (int) ini_getl(sect, "FIYAT", -1, inifile); //yoksa fiyat -1 olacak varsa ne yaziyorsa o olacak.
                rec_Seans.FIYAT = (int) ini_getl(sect, "FIYAT", -1, inifile); //yoksa fiyat -1 olacak varsa ne yaziyorsa o olacak.

                //debug_print("[%s], ID: %d", sect, rec_Seans.ID);
                found = 1;
                break;
            }
        }
        //debug_print("for disinda s: %d",s);

        if(found)
        {
            ptr = strtok(grup_kod, " -,");
            i =0;
            while (ptr != NULL)
            {
                //printf("grup kodlari: %s\n", ptr);
                rec_Seans.GRUP[i] = atoi(ptr);
                ptr = strtok(NULL, " -,");
                rec_Seans.GRUP_FIYAT[i][0] = atoi(ptr); //6.4.0 ilk fiyat
                ptr = strtok(NULL, " -,");
                rec_Seans.GRUP_FIYAT[i++][1] = atoi(ptr); //6.4.0 ikinci fiyat
                ptr = strtok(NULL, " -,");
            }

            //printf("fiyat: %d\n", rec_Seans.FIYAT);
            //printf("grup kodlari: %s\n", grup_kod);
            //printf("cinsiyet: %s\n", cinsiyet);

            //for(s=0; s<i; s++)
            //	printf("GRUP: %d, FIYAT: %d\n", rec_Seans.GRUP[s],rec_Seans.GRUP_FIYAT[s]);
        }

    }//6.1.0
    if(rec_TERM.TERM_TIP == TERM_YEMEKHANE)//0.0.7
        if(PERSONEL_YemekOgun_EkranAc()>0L)//0.0.6
            //odroid
            //buraya ne koyacağımı bilemedim ondan i = 0 yaptım
            //sys_backlight_set_time(60);
            i = 0;

    //0.0.7 burası acaba sadece PARA term ise mi yapılmalı.
    if (net_ONLINE_PARA  == 0x00) z_ONLINE_TEKRAR_DK++; //Offline Ise
    if (z_ONLINE_TEKRAR_DK>rec_TERM.ONLINE_TEKRAR_DK)
    {
        net_ONLINE_PARA =0x01;
        z_ONLINE_TEKRAR_DK=0;
    }
}


//void Is_Saat(struct dosdate_t * d,struct dostime_t * t)
void Is_Saat(struct tm * timeinfo)
{

    struct_date dd;
    struct_time tt;
    time_t rawtime;
    //7.0.0
    char rez_file[13];//gunluk rezervasyon dosyasi
    struct_date dq;
    //7.0.0
    get_date(&dq);
    sprintf(rez_file, "%02d%02d%04d.rez", dq.da_day, dq.da_mon, dq.da_year);
    if(access(rez_file, 0) == 0)//rezervasyon dosyasi var
        rezervasyon = 1;
    else
        rezervasyon = 0;

    //if (GUNISIGI_Kontrol(d,t)==0x01) son_Saat=0;
    if (GUNISIGI_Kontrol(timeinfo)==0x01) son_Saat=0;

    z_TS_GUNCELLE_SAAT++;
    z_LOG_KART_SAAT++;
    z_LOG_ALARM_SAAT++;
    z_LOG_TERM_SAAT++;

    if (z_TS_GUNCELLE_SAAT>rec_TERM.TS_GUNCELLE_SAAT)
    {
        if (TCP_GetDateTime(&dd,&tt)==0x01)
        {
            //setdate(&dd);
            //settime(&tt);

            timeinfo->tm_year = (int) (dd.da_year - 1900);
            timeinfo->tm_mon = (int) (dd.da_mon - 1);
            timeinfo->tm_mday = (int) (dd.da_day);
            timeinfo->tm_hour = (int) (tt.ti_hour);
            timeinfo->tm_min = (int) (tt.ti_min);
            timeinfo->tm_sec = (int)(tt.ti_sec);
            rawtime = mktime ( timeinfo );
            //if(stime(&rawtime))
            //odroid
            //root olmadan çalısır mı acaba bir de hwclock konusu var unutulmaması gereken
            //if(sys_set_time(timeinfo))
            if(stime(&rawtime))
                printf("error setting date-time: %s\n", strerror(errno));
            else
            {
                //2.3.1
                //tarih/saat değişti, zamanlirole'yi yeniden başlat
                system("sudo pkill -9 -f zamanlirole");
                system("/home/odroid/Solidus/zamanlirole&");

                son_Dakika=0;
                son_Saniye=0;
                lcd_printf(ALG_CENTER, "Date-Time Set: %02d/%02d/%04d, %02d:%02d:%02d", dd.da_day, dd.da_mon, dd.da_year, tt.ti_hour, tt.ti_min, tt.ti_sec);
                lcd_flip();
            }

        }

        //if (TCP_GetDateTime(&dd,&tt)==0x01)
        //{
        //	setdate(&dd);
        //	settime(&tt);
        //	son_Dakika=0;
        //	son_Saniye=0;
        //}
        z_TS_GUNCELLE_SAAT=0;
    }
    if (z_LOG_KART_SAAT>rec_TERM.LOG_KART_SAAT)
    {
        if(rec_TERM.TERM_TIP == TERM_PARA)
            flag_LOG_PARA=1;
        else
            flag_LOG_KART=1;
        //flag_LOG_KART=1;
        z_LOG_KART_SAAT=0;
    }
    if (z_LOG_ALARM_SAAT>rec_TERM.LOG_ALARM_SAAT)
    {
        z_LOG_ALARM_SAAT=0;
    }
    if (z_LOG_TERM_SAAT>rec_TERM.LOG_TERM_SAAT)
    {
        z_LOG_TERM_SAAT=0;
    }

}

void ZAMANIS_Kontrol(void)
{
    //time_t zaman;
    //struct dosdate_t d;
    //struct dostime_t t;
    unsigned long Saat;
    unsigned long Dakika;
    unsigned long Saniye;
    time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    //_dos_getdate(&d);
    //_dos_gettime(&t);

    /*
    	Saat = ((d.year-2000)*64L*64L*64L) +
    				 (d.month*64L*64L) +
    				 (d.day*64L) +
    					t.hour;
    	Dakika = ((d.year-2000)*64L*64L*64L*64L) +
    				 (d.month*64L*64L*64L) +
    				 (d.day*64L*64L) +
    				 (t.hour*64L) +
    					t.minute;
    	Saniye = ((d.year-2000)*64L*64L*64L*64L*64L) +
    				 (d.month*64L*64L*64L*64L) +
    				 (d.day*64L*64L*64L) +
    				 (t.hour*64L*64L) +
    				 (t.minute*64L) +
    					t.second;
    */

    Saniye = time(NULL);
    Dakika = (Saniye-(Saniye%60L)) / 60L;
    Saat   = (Dakika-(Dakika%60L)) / 60L;

    if (son_Saat<Saat)
    {
        son_Saat=Saat;
        //Is_Saat(&d,&t);
        Is_Saat(timeinfo);
    }

    if (son_Dakika<Dakika)
    {
        son_Dakika=Dakika;
        //Is_Dakika(&d,&t);
        Is_Dakika(timeinfo);
        //if(t.hour == 7 && t.minute == 0)
        /*if(timeinfo->tm_hour == 7 && timeinfo->tm_min== 0)
        	reboot = 1;
        if(reboot && !FTP_RUN && !Hata.HATA && !TopUP_ERR.HATA){
        	//printf("time to reset\n");
        	//WDT_Init(0L);
        	//delay(1000);
        }*/ //m8110
    }

    if (son_Saniye<Saniye)
    {
        son_Saniye=Saniye;
        //Is_Saniye(&d,&t);
        Is_Saniye(timeinfo);
    }
}

