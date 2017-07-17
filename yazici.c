#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wiringPi.h>
#include <wiringSerial.h>

#include "main.h"
#include "config.h"
#include "lcd.h"
#include "mesaj.h"
#include "personel.h"
#include "sayac.h"
#include "tool.h"

#define 	DEVICE_NAME 	"/dev/ttyS2"

#define		NEAR_END		0x1
#define		PAPER_END		0x2
#define		PAPER_OK		0x0


struct_PERSONEL		rec_Personel;

//0.0.91
//custom yazıcının durumunu kontrol eder
int check_paper(int prn)
{
struct timeval	t;
fd_set			rfds;
unsigned char 	bTemp[256];
int           	iTemp;
int 			ret = -1;

	t.tv_sec  = 0;
	t.tv_usec = 500000;

	FD_ZERO(&rfds);
	FD_SET(prn, &rfds);

	//do{
	//odroid
	//write(prn, "\x1b\x76", 2);
    serialPuts(prn, "\x1b\x76");
	if ((select(prn + 1, &rfds, NULL, NULL, &t) > 0)&& FD_ISSET(prn, &rfds))
	{
	    //odroid
		iTemp = read(prn, bTemp, sizeof(bTemp));
		//serialGetchar(prn);
		//debug_print("iTemp: %d, bTemp: %d", iTemp, bTemp[0]);
		if (iTemp > 0){//0.0.91
			//write(ifd_prn, bTemp, iTemp);
			//lcd_printf(ALG_LEFT, "Recived %d bytes...", iTemp);
			//lcd_flip();
			switch(bTemp[0]){
				case 0x03:
					ret = NEAR_END;
					//debug_print("KAGIT AZALMIS");
					break;
				case 0x0C:
					ret = PAPER_END;
					//debug_print("KAGIT BITMIS");
					break;
				default:
					ret = PAPER_OK;
					break;
			}

		}
	}
	return ret;

}

void YAZICI_FisYukleme_Bixolon(unsigned long Tutar,char Y)
{
char *szTmp;
char CRLF[] ="\r\n";
struct_date d;
struct_time t;
int ifd_prn = -1;


    ifd_prn = serialOpen(DEVICE_NAME, 115200);
 	if (ifd_prn <0){
        lcd_printf(ALG_CENTER, "OPEN UART Failed.");
		lcd_flip();
		getch();//odroid//kb_getkey();
		return;
 	}

    szTmp = calloc(100,1);

    get_date(&d);
    get_time(&t);

    memset(szTmp,0x00,100);
    memset(szTmp,'=',32);
    serialPuts(ifd_prn, szTmp);
    serialPuts(ifd_prn, CRLF);
    //6.5.2 unv ismini yazici.ini'den alir
    // SerPort_SendStr (1,"  NAMIK KEMAL UNVERSTES\r\n");

    //2.6.0
    //buffer overflow olmasın diye koyuldu bu kontrol
    if(strlen(rec_YAZICI.ACIKLAMA1)<32){
        memset(szTmp,0x00,100);
        memset(szTmp,' ',(32-strlen(rec_YAZICI.ACIKLAMA1))/2);
        serialPuts(ifd_prn, szTmp);
    }
    serialPuts(ifd_prn, rec_YAZICI.ACIKLAMA1);

    //7.5.2
    //memset(szTmp,0x00,100);
    //2.6.0
    //buffer overflow olmasın diye koyuldu bu kontrol
    if(strlen(rec_YAZICI.ACIKLAMA2)<32){
        memset(szTmp,0x00,100);
        memset(szTmp,' ',(32-strlen(rec_YAZICI.ACIKLAMA2))/2);
        serialPuts(ifd_prn, szTmp);
    }
    serialPuts(ifd_prn, rec_YAZICI.ACIKLAMA2);

    //memset(szTmp,0x00,100);
    //2.6.0
    //buffer overflow olmasın diye koyuldu bu kontrol
    if(strlen(rec_YAZICI.ACIKLAMA3)<32){
        memset(szTmp,0x00,100);
        memset(szTmp,' ',(32-strlen(rec_YAZICI.ACIKLAMA3))/2);
        serialPuts(ifd_prn, szTmp);
    }
    serialPuts(ifd_prn, rec_YAZICI.ACIKLAMA3);



    serialPuts(ifd_prn, CRLF);
    sprintf(szTmp,"         TERMİNAL : %s\r\n",rec_TERM.KOD);
    serialPuts(ifd_prn, szTmp);
    sprintf(szTmp,"        %02d-%02d-%04d %02d:%02d\r\n",
                                d.da_day,d.da_mon,d.da_year,
                                t.ti_hour,t.ti_min);
    serialPuts(ifd_prn, szTmp);

    memset(szTmp,0x00,100);
    memset(szTmp,'=',32);
    serialPuts(ifd_prn, szTmp);
    serialPuts(ifd_prn, CRLF);

    if (Y)
    {
     serialPuts(ifd_prn, "       PARA YÜKLEME İŞLEMİ\r\n");
    } else {
     serialPuts(ifd_prn, "        PARA DÜŞME İŞLEMİ\r\n");
    }
    memset(szTmp,0x00,100);
    memset(szTmp,'-',32);
    serialPuts(ifd_prn, szTmp);
    serialPuts(ifd_prn, CRLF);
    //7.5.2
    if(rec_YAZICI.KART_NO_BAS != 0x0){
     serialPuts(ifd_prn, rec_Personel.KARTNO_HEX);
     serialPuts(ifd_prn, CRLF);
    }
    strcpy(szTmp,rec_Personel.ADI);
    //aWinToDos(szTmp);
    serialPuts(ifd_prn, szTmp);
    serialPuts(ifd_prn, " ");
    strcpy(szTmp,rec_Personel.SOYAD);
    //aWinToDos(szTmp);
    serialPuts(ifd_prn, szTmp);
    serialPuts(ifd_prn, CRLF);

    memset(szTmp,0x00,100);
    memset(szTmp,'-',32);
    serialPuts(ifd_prn, szTmp);
    serialPuts(ifd_prn, CRLF);

    if (Y)
    {
     rec_Personel.BAKIYE -=Tutar;
    } else {
     rec_Personel.BAKIYE +=Tutar;
    }
    sprintf(szTmp,"ESKİ BAKİYE : %8ld.%02ld TL",(unsigned long) (rec_Personel.BAKIYE/100),(unsigned long)rec_Personel.BAKIYE%100);
    serialPuts(ifd_prn, szTmp);
    serialPuts(ifd_prn, CRLF);
    if (Y)
    {
     sprintf(szTmp,"YÜKLEME     : %8ld.%02ld TL",(unsigned long) (Tutar/100),(unsigned long)Tutar%100);
    } else {
     sprintf(szTmp,"DÜŞME       : %8ld.%02ld TL",(unsigned long) (Tutar/100),(unsigned long)Tutar%100);
    }
    serialPuts(ifd_prn, szTmp);
    serialPuts(ifd_prn, CRLF);
    if (Y)
    {
     rec_Personel.BAKIYE +=Tutar;
    } else {
     rec_Personel.BAKIYE -=Tutar;
    }
    sprintf(szTmp,"YENİ BAKİYE : %8ld.%02ld TL",(unsigned long) (rec_Personel.BAKIYE/100),(unsigned long)rec_Personel.BAKIYE%100);
    serialPuts(ifd_prn, szTmp);
    serialPuts(ifd_prn, CRLF);

    memset(szTmp,0x00,100);
    memset(szTmp,'-',32);
    serialPuts(ifd_prn, szTmp);
    serialPuts(ifd_prn, CRLF);
    serialPuts(ifd_prn, CRLF);
    serialPuts(ifd_prn, CRLF);
    serialPuts(ifd_prn, CRLF);
    serialPuts(ifd_prn, CRLF);

    //6.5.3
    //Partial Cut (one point left uncut)
    serialPuts(ifd_prn, "\x1b\x69");

    //2.5.2
    //kapatılmıyordu, kapattım artık
    serialClose(ifd_prn);

    free(szTmp);
}


//Harcama Terminali icin Harici Yazicidan Fis Basar
//25.06.2014, 01:04
void YAZICI_FisYukleme_Custom(unsigned long Tutar, char Y)
{
//unsigned char *szTmp; m8110
char *szTmp;
const char CRLF[] ="\r\n";
int ifd_prn = -1;


//unsigned char 	bTemp[1024];
//int           	iTemp;
struct_date 	dd;
struct_time 	tt;
struct tm 		*timeinfo;
time_t 			rawtime;

//struct timeval	t;
//fd_set			rfds;
int printer_ok;//0.0.91
int prn_status;
int check_again;

	//lcd_clean();

	//odroid
	//burada seri port acilacak
 	//ifd_prn = open(DEVICE_NAME,  O_RDWR);
 	ifd_prn = serialOpen(DEVICE_NAME, 115200);
 	if (ifd_prn >=0){
 		szTmp = calloc(100,1);


		time(&rawtime);
		timeinfo = localtime(&rawtime);

		dd.da_day = (char) timeinfo->tm_mday;
		dd.da_mon = (char) timeinfo->tm_mon+1;
		dd.da_year = (uint16_t)timeinfo->tm_year + 1900;

		tt.ti_hour = (unsigned char) timeinfo->tm_hour;
		tt.ti_min = (unsigned char) timeinfo->tm_min;
		tt.ti_sec = (unsigned char) timeinfo->tm_sec;
		tt.ti_hund = 0;

		//lcd_printf(ALG_CENTER, "UART %s", DEVICE_NAME);
		//lcd_printf(ALG_CENTER, "SETTING: 115200,8,n,1");

		//lcd_flip();

		//odroid
		//burada seri port ayarlanmalı
		//tty_property_config(ifd_prn, 115200, 8,'n',1,'n');

		do{
			prn_status = check_paper(ifd_prn);
			switch(prn_status)
			{
				case NEAR_END:
					debug_print("YAZICI KAGIDI BITMEK UZERE");
					printer_ok = 1;
					check_again = 0;
					break;
				case PAPER_END:
					//lcd_set_bk_color(COLOR_RED);
					//lcd_set_font_color(COLOR_YELLOW);
					lcd_flip();
					lcd_printf(ALG_CENTER, "YAZICI DURUMU => KAGIT BITMIS");//0.0.8
					lcd_printf(ALG_CENTER, "YENI RULO TAKIP BIR TUSA BASIN");//0.0.9
					lcd_printf(ALG_CENTER, "CANCEL TUSUNA BASARSANIZ");
					lcd_printf(ALG_CENTER, "BASKI ALMADAN DEVAM EDILIR");
					lcd_flip();
					MSJ_Ses("5343332313");
					//odroid
					//if(kb_getkey() == DIKS_ESCAPE){
					if(getch() == 0x1B){
						printer_ok = 0;
						check_again = 0;
						break;
					}
					else{
						printer_ok = 0;
						check_again = 1;
					}
					break;
				case PAPER_OK:
					//debug_print("YAZICI OK");
					printer_ok = 1;
					check_again = 0;
					break;
				default:
					//lcd_set_bk_color(COLOR_RED);
					//lcd_set_font_color(COLOR_YELLOW);
					lcd_flip();
					lcd_printf(ALG_CENTER, "YAZICI PROBLEMLI");
					lcd_printf(ALG_CENTER, "SORUNU COZUP BIR TUSA BASIN");
					lcd_printf(ALG_CENTER, "CANCEL TUSUNA BASARSANIZ");
					lcd_printf(ALG_CENTER, "BASKI ALMADAN DEVAM EDILIR");
					lcd_flip();
					MSJ_Ses("5343332313");
					//odroid
					//if(kb_getkey() == DIKS_ESCAPE){
					if(getch() == 0x1B){
						printer_ok = 0;
						check_again = 0;
						break;
					}
					else{
						printer_ok = 0;
						check_again = 1;
					}

					break;
			}
		}while(check_again);
		/*
		t.tv_sec  = 0;
		t.tv_usec = 500000;

		FD_ZERO(&rfds);
		FD_SET(ifd_prn, &rfds);

		if ((select(ifd_prn + 1, &rfds, NULL, NULL, &t) > 0)&&
			FD_ISSET(ifd_prn, &rfds))
		{
			iTemp = read(ifd_prn, bTemp, sizeof(bTemp));
			if (iTemp > 0){
				write(ifd_prn, bTemp, iTemp);
				lcd_printf(ALG_LEFT, "Recived %d bytes...", iTemp);
				lcd_flip();
			}
		}*/
		//else {
		if(printer_ok){
			//memcpy(szTmp, "\x18\x1b\x21\x08\x1b\x2d\x0\x1b\x32\x1d\x21\x11\x1b\x61\x01\x1b\x45\x01",18);
			memcpy(szTmp, "\x18\x1b\x21\x08\x1b\x2d\x0\x1b\x32\x1d\x21\x11\x1b\x61\x01\x1b\x74\x02\x1b\x45\x01",18);

			//odroid
			//iTemp = strlen((char*)szTmp);
			//write(ifd_prn, szTmp, iTemp);
            serialPuts(ifd_prn, szTmp);

			memset(szTmp,0x00,100);
			memset(szTmp,'=',24);
			//odroid
			//iTemp = strlen((char*)szTmp);
			//write(ifd_prn, szTmp, iTemp);
			//write(ifd_prn, CRLF, 2);
			serialPuts(ifd_prn, szTmp);
			serialPuts(ifd_prn, CRLF);

			//memset(szTmp,0x00,100);
			//2.6.0
            //buffer overflow olmasın diye koyuldu bu kontrol
            if(strlen(rec_YAZICI.ACIKLAMA1)<24){
                memset(szTmp,0x00,100);
                memset(szTmp,' ',(24-strlen(rec_YAZICI.ACIKLAMA1))/2);
                //odroid
                //iTemp = strlen((char*)szTmp);
                //write(ifd_prn, szTmp, iTemp);
                serialPuts(ifd_prn, szTmp);
            }
			//odroid
			//iTemp = strlen((char*)rec_YAZICI.ACIKLAMA1);
			//write(ifd_prn, rec_YAZICI.ACIKLAMA1, iTemp);
			//write(ifd_prn, CRLF, 2);
			serialPuts(ifd_prn, rec_YAZICI.ACIKLAMA1);
            serialPuts(ifd_prn, CRLF);

			if(strlen(rec_YAZICI.ACIKLAMA2)>0){
				//memset(szTmp,0x00,100);
				//2.6.0
                //buffer overflow olmasın diye koyuldu bu kontrol
                if(strlen(rec_YAZICI.ACIKLAMA2)<24){
                    memset(szTmp,0x00,100);
                    memset(szTmp,' ',(24-strlen(rec_YAZICI.ACIKLAMA2))/2);
                    //odroid
                    //iTemp = strlen((char*)szTmp);
                    //write(ifd_prn, szTmp, iTemp);
                    serialPuts(ifd_prn, szTmp);
                }
				//odroid
				//iTemp = strlen((char*)rec_YAZICI.ACIKLAMA2);
				//write(ifd_prn, rec_YAZICI.ACIKLAMA2, iTemp);
				//write(ifd_prn, CRLF, 2);
				serialPuts(ifd_prn, szTmp);
				serialPuts(ifd_prn, CRLF);
			}

			if(strlen(rec_YAZICI.ACIKLAMA3)>0){
				//memset(szTmp,0x00,100);
				//2.6.0
                //buffer overflow olmasın diye koyuldu bu kontrol
                if(strlen(rec_YAZICI.ACIKLAMA3)<24){
                    memset(szTmp,0x00,100);
                    memset(szTmp,' ',(24-strlen(rec_YAZICI.ACIKLAMA3))/2);
                    //odroid
                    //iTemp = strlen((char*)szTmp);
                    //write(ifd_prn, szTmp, iTemp);
                    serialPuts(ifd_prn, szTmp);
                }
				//iTemp = strlen((char*)rec_YAZICI.ACIKLAMA3);
				//write(ifd_prn, rec_YAZICI.ACIKLAMA3, iTemp);
				//write(ifd_prn, CRLF, 2);
				serialPuts(ifd_prn, szTmp);
				serialPuts(ifd_prn, CRLF);
			}

			//memset(szTmp,0x00,100);
			//2.6.0
            //buffer overflow olmasın diye koyuldu bu kontrol
            if(strlen(rec_TERM.ACIKLAMA)<24){
                memset(szTmp,0x00,100);
                memset(szTmp,' ',(24-strlen(rec_TERM.ACIKLAMA))/2);
                //odro id
                //iTemp = strlen((char*)szTmp);
                //write(ifd_prn, szTmp, iTemp);
                serialPuts(ifd_prn, szTmp);
            }
			//iTemp = strlen((char*)rec_TERM.ACIKLAMA);
			//write(ifd_prn, rec_TERM.ACIKLAMA, iTemp);
			//write(ifd_prn, CRLF, 2);
			serialPuts(ifd_prn, rec_TERM.ACIKLAMA);
			serialPuts(ifd_prn, CRLF);
			sprintf(szTmp,"    %02d-%02d-%04d %02d:%02d\r\n",
											dd.da_day,dd.da_mon,dd.da_year,
											tt.ti_hour,tt.ti_min);
			//iTemp = strlen((char*)szTmp);
			//write(ifd_prn, szTmp, iTemp);
			serialPuts(ifd_prn, szTmp);

			memset(szTmp,0x00,100);
			memset(szTmp,'=',24);
			//iTemp = strlen((char*)szTmp);
			//write(ifd_prn, szTmp, iTemp);
			serialPuts(ifd_prn, szTmp);

			//write(ifd_prn, CRLF, 2);
			serialPuts(ifd_prn, CRLF);

            //1.0.0
			//if(strcmp("BAS", rec_YAZICI.KART_NO_BAS) ==0){
			if(rec_YAZICI.KART_NO_BAS){
				strcpy((char*)szTmp, "        ");
				//iTemp = strlen((char*)szTmp);
				//write(ifd_prn, szTmp, iTemp);
				serialPuts(ifd_prn, szTmp);
				strcpy((char*)szTmp, rec_Personel.KARTNO_HEX);
				//iTemp = strlen((char*)szTmp);
				//write(ifd_prn, szTmp, iTemp);
				serialPuts(ifd_prn, szTmp);
			}
			//write(ifd_prn, CRLF, 2);
			serialPuts(ifd_prn, CRLF);

			if (Y)
			{
				memset(szTmp,0x00,100);
				memset(szTmp,' ',(24-strlen("PARA YUKLEME ISLEMI"))/2);
				//iTemp = strlen((char*)szTmp);
				//write(ifd_prn, szTmp, iTemp);
				serialPuts(ifd_prn, szTmp);
				 //SerPort_SendStr (1,"       PARA YšKLEME ˜LEM˜\r\n");
				strcpy((char*)szTmp,"PARA YUKLEME ISLEMI");
				//iTemp = strlen((char*)szTmp);
				//write(ifd_prn, szTmp, iTemp);
				serialPuts(ifd_prn, szTmp);
			} else {
				memset(szTmp,0x00,100);
				memset(szTmp,' ',(24-strlen("PARA DUSME ISLEMI"))/2);
				//iTemp = strlen((char*)szTmp);
				//write(ifd_prn, szTmp, iTemp);
				serialPuts(ifd_prn, szTmp);
				 //SerPort_SendStr (1,"        PARA DšME ˜LEM˜\r\n");
				strcpy((char*)szTmp,"PARA DUSME ISLEMI");
				//iTemp = strlen((char*)szTmp);
				//write(ifd_prn, szTmp, iTemp);
				serialPuts(ifd_prn, szTmp);
			}
			//write(ifd_prn, CRLF, 2);
			serialPuts(ifd_prn, CRLF);
			memset(szTmp,0x00,100);
			memset(szTmp,' ',(24-strlen(rec_Personel.ADI))/2);
			//iTemp = strlen((char*)szTmp);
			//write(ifd_prn, szTmp, iTemp);
			serialPuts(ifd_prn, szTmp);
			strcpy((char*)szTmp, rec_Personel.ADI);
			//iTemp = strlen((char*)szTmp);
			//write(ifd_prn, szTmp, iTemp);
			serialPuts(ifd_prn, szTmp);
			//write(ifd_prn, CRLF, 2);
			serialPuts(ifd_prn, CRLF);
			memset(szTmp,0x00,100);
			memset(szTmp,' ',(24-strlen(rec_Personel.SOYAD))/2);
			//iTemp = strlen((char*)szTmp);
			//write(ifd_prn, szTmp, iTemp);
			serialPuts(ifd_prn, szTmp);
			strcpy((char*)szTmp, rec_Personel.SOYAD);
			//iTemp = strlen((char*)szTmp);
			//write(ifd_prn, szTmp, iTemp);
			serialPuts(ifd_prn, szTmp);

			//write(ifd_prn, CRLF, 2);
			serialPuts(ifd_prn, CRLF);

			memset(szTmp,0x00,100);
			memset(szTmp,'-',24);
			//iTemp = strlen((char*)szTmp);
			//write(ifd_prn, szTmp, iTemp);
			serialPuts(ifd_prn, szTmp);
			//write(ifd_prn, CRLF, 2);
			serialPuts(ifd_prn, CRLF);
			if (Y)
			{
				 rec_Personel.BAKIYE -=Tutar;
			} else {
				 rec_Personel.BAKIYE +=Tutar;
			}

		//sprintf(szTmp,"ESKİ BAKİYE  %8ld.%02ld TL",(unsigned long) ((rec_Personel.BAKIYE+rec_Personel.HARCANAN)/100),(unsigned long)(rec_Personel.BAKIYE+rec_Personel.HARCANAN)%100);
			sprintf(szTmp,"ESKI BAKIYE  %4ld.%02ld TL",(unsigned long) ((rec_Personel.BAKIYE)/100),(unsigned long)(rec_Personel.BAKIYE)%100);
			//iTemp = strlen((char*)szTmp);
			//write(ifd_prn, szTmp, iTemp);
			//write(ifd_prn, CRLF, 2);
			serialPuts(ifd_prn, szTmp);
			serialPuts(ifd_prn, CRLF);

			if (Y)
			 {
				 sprintf(szTmp,"YUKLEME      %4ld.%02ld TL",(unsigned long) (Tutar/100),(unsigned long)Tutar%100);
			 } else {
				 sprintf(szTmp,"DUSME        %4ld.%02ld TL",(unsigned long) (Tutar/100),(unsigned long)Tutar%100);
			 }
			//iTemp = strlen((char*)szTmp);
			//write(ifd_prn, szTmp, iTemp);
			//write(ifd_prn, CRLF, 2);
            serialPuts(ifd_prn, szTmp);
			serialPuts(ifd_prn, CRLF);

			if (Y)
			{
				 rec_Personel.BAKIYE +=Tutar;
			} else {
				 rec_Personel.BAKIYE -=Tutar;
			}
		 	sprintf(szTmp,"YENI BAKIYE  %4ld.%02ld TL",(unsigned long) (rec_Personel.BAKIYE/100),(unsigned long)rec_Personel.BAKIYE%100);
		 	//iTemp = strlen((char*)szTmp);
		 	//write(ifd_prn, szTmp, iTemp);
		 	//write(ifd_prn, CRLF, 2);
		 	serialPuts(ifd_prn, szTmp);
			serialPuts(ifd_prn, CRLF);

		 	memset(szTmp,0x00,100);
			memset(szTmp,'-',24);
			//iTemp = strlen((char*)szTmp);
			//write(ifd_prn, szTmp, iTemp);
			//write(ifd_prn, CRLF, 2);
			serialPuts(ifd_prn, szTmp);
			serialPuts(ifd_prn, CRLF);
			//Total Cut and automatic paper moving back
			//strcpy((char*)szTmp, "\x1b\x69");
			strcpy((char*)szTmp, "\x1c\xc0\x34");
			//iTemp = strlen((char*)szTmp);
			//write(ifd_prn, szTmp, iTemp);
			serialPuts(ifd_prn, szTmp);

            //odroid
			//usleep(100000);
			napms(100);
		}
		//2.3.1
		free(szTmp);
	}else{
		lcd_printf(ALG_CENTER, "OPEN UART Failed.");
		lcd_flip();
		getch();//kb_getkey();
	}


	if (ifd_prn >=0){
		//close(ifd_prn);
		serialClose(ifd_prn);
		ifd_prn = -1;
	}
	//2.3.1
	//yukarı alındı allocated edilmeden free edilme riskine karşı
	//free(szTmp);
}


//6.1.0
void YAZICI_FisYemekhane_Bixolon(void)
{
char *szTmp;
char CRLF[] ="\r\n";
struct_date d;
struct_time t;
//7.5.2
int boy;
int ifd_prn = -1;

    ifd_prn = serialOpen(DEVICE_NAME, 115200);
 	if (ifd_prn <0){
        lcd_printf(ALG_CENTER, "OPEN UART Failed.");
		lcd_flip();
		getch();//odroid//kb_getkey();
		return;
 	}

    szTmp = calloc(100,1);

    get_date(&d);
    get_time(&t);

    memset(szTmp,0x00,100);
    memset(szTmp,'=',42);
    serialPuts(ifd_prn, szTmp);
    serialPuts(ifd_prn, CRLF);

    //memset(szTmp,0x00,100);
    //2.6.0
    //buffer overflow olmasın diye koyuldu bu kontrol
    if(strlen(rec_YAZICI.ACIKLAMA1)<42){
        memset(szTmp,0x00,100);
        memset(szTmp,' ',(42-strlen(rec_YAZICI.ACIKLAMA1))/2);
        serialPuts(ifd_prn, szTmp);
    }
    serialPuts(ifd_prn, rec_YAZICI.ACIKLAMA1);
    serialPuts(ifd_prn, CRLF);
    //7.5.2
    //memset(szTmp,0x00,100);
    //2.6.0
    //buffer overflow olmasın diye koyuldu bu kontrol
    if(strlen(rec_YAZICI.ACIKLAMA2)<42){
        memset(szTmp,0x00,100);
        memset(szTmp,' ',(42-strlen(rec_YAZICI.ACIKLAMA2))/2);
        serialPuts(ifd_prn, szTmp);
    }
    serialPuts(ifd_prn, rec_YAZICI.ACIKLAMA2);
    serialPuts(ifd_prn, CRLF);

    //memset(szTmp,0x00,100);
    //2.6.0
    //buffer overflow olmasın diye koyuldu bu kontrol
    if(strlen(rec_YAZICI.ACIKLAMA3)<42){
        memset(szTmp,0x00,100);
        memset(szTmp,' ',(42-strlen(rec_YAZICI.ACIKLAMA3))/2);
        serialPuts(ifd_prn, szTmp);
    }
    serialPuts(ifd_prn, rec_YAZICI.ACIKLAMA3);
    serialPuts(ifd_prn, CRLF);

    //7.5.2
    /*
    //6.3.1 artik terminal aciklamasi yazilacak
    // sprintf(szTmp,"             TERMNAL : %s\r\n",rec_TERM.KOD);
    memset(szTmp,0x00,100);
    memset(szTmp,' ',(42-strlen(rec_TERM.ACIKLAMA))/2);
    serialPuts(ifd_prn, szTmp);
    SerPort_SendStr (1,rec_TERM.ACIKLAMA);
    */
    sprintf(szTmp,"            %02d-%02d-%04d %02d:%02d\r\n",
                                d.da_day,d.da_mon,d.da_year,
                                t.ti_hour,t.ti_min);
    serialPuts(ifd_prn, szTmp);

    memset(szTmp,0x00,100);
    memset(szTmp,'=',42);
    serialPuts(ifd_prn, szTmp);
    serialPuts(ifd_prn, CRLF);
    /*
    if (Y)
    {
     SerPort_SendStr (1,"       PARA YKLEME LEM\r\n");
    } else {
     SerPort_SendStr (1,"        PARA DME LEM\r\n");
    }

    memset(szTmp,0x00,100);
    memset(szTmp,'-',42);
    SerPort_SendStr (1,szTmp);
    SerPort_SendStr (1,CRLF);
    */
    if(rec_YAZICI.KART_NO_BAS){
    serialPuts(ifd_prn, "                 ");
    serialPuts(ifd_prn, rec_Personel.KARTNO_HEX);
    }
    serialPuts(ifd_prn, CRLF);
    //7.5.2
    if(rec_YAZICI.SIRA_NO_BAS){
        memset(szTmp,0x00,100);
        sprintf(szTmp, "SIRA NO: %u", SAYAC.SAYI);
        boy = strlen(szTmp);
        memset(szTmp,0x00,100);
        memset(szTmp,' ',(42-boy)/2);
        serialPuts(ifd_prn, szTmp);
        sprintf(szTmp, "SIRA NO: %u", SAYAC.SAYI);
        serialPuts(ifd_prn, szTmp);
        serialPuts(ifd_prn, CRLF);
    }


    // SerPort_SendStr (1,"\x1d\x21\x11");
    memset(szTmp,0x00,100);
    sprintf(szTmp, "%s %s", rec_Personel.ADI, rec_Personel.SOYAD);
    boy = strlen(szTmp);
    memset(szTmp,0x00,100);
    memset(szTmp,' ',(42-boy)/2);
    serialPuts(ifd_prn, szTmp);
    memset(szTmp,0x00,100);
    sprintf(szTmp, "%s %s", rec_Personel.ADI, rec_Personel.SOYAD);
    serialPuts(ifd_prn, szTmp);
    serialPuts(ifd_prn, CRLF);

    // SerPort_SendStr (1,"\x1d\x21\x0");
    memset(szTmp,0x00,100);
    memset(szTmp,'-',42);
    serialPuts(ifd_prn, szTmp);
    serialPuts(ifd_prn, CRLF);

    memset(szTmp,0x00,100);
    sprintf(szTmp,"       ESKi BAKiYE : %8ld.%02ld TL",(unsigned long) ((rec_Personel.BAKIYE+rec_Personel.HARCANAN)/100),(unsigned long)(rec_Personel.BAKIYE+rec_Personel.HARCANAN)%100);
    serialPuts(ifd_prn, szTmp);
    serialPuts(ifd_prn, CRLF);

    // sprintf(szTmp,"HARCANAN    : %8ld.%02ld TL",(unsigned long) (Tutar/100),(unsigned long)Tutar%100);
    memset(szTmp,0x00,100);
    sprintf(szTmp,"       HARCANAN    : %8lu.%02lu TL",(unsigned long) (rec_Personel.HARCANAN/100),(unsigned long)rec_Personel.HARCANAN%100);
    serialPuts(ifd_prn, szTmp);
    serialPuts(ifd_prn, CRLF);

    // sprintf(szTmp,"YEN BAKYE : %8ld.%02ld TL",(unsigned long) (rec_Personel.BAKIYE/100),(unsigned long)rec_Personel.BAKIYE%100);
    memset(szTmp,0x00,100);
    sprintf(szTmp,"       KALAN       : %8ld.%02ld TL",(unsigned long) (rec_Personel.BAKIYE/100),(unsigned long)rec_Personel.BAKIYE%100);
    serialPuts(ifd_prn, szTmp);
    serialPuts(ifd_prn, CRLF);

    memset(szTmp,0x00,100);
    memset(szTmp,'-',42);
    serialPuts(ifd_prn, szTmp);
    serialPuts(ifd_prn, CRLF);
    serialPuts(ifd_prn, CRLF);
    serialPuts(ifd_prn, CRLF);
    serialPuts(ifd_prn, CRLF);
    serialPuts(ifd_prn, CRLF);

    //Partial Cut (one point left uncut)
    serialPuts(ifd_prn, "\x1b\x69");

    //2.5.2
    //kapatılmıyordu, kapattım artık
    serialClose(ifd_prn);

    free(szTmp);

}


void YAZICI_FisYemekhane_Custom(void)
{
 //unsigned char *szTmp; m8110
 char *szTmp;
 const char CRLF[] ="\r\n";
 int ifd_prn = -1;

 //unsigned char bTemp[256];
// int           iTemp;
 struct_date dd;
 struct_time tt;
 struct tm * timeinfo;
 time_t rawtime;

 //struct timeval	t;
 //fd_set			rfds;
 int printer_ok;//0.0.91
 int prn_status;
 int check_again;

 	 //lcd_clean();
 	 //odroid
 	 //burada seri port açılacak vs...
 	 //ifd_prn = open(DEVICE_NAME,  O_RDWR);
 	 ifd_prn = serialOpen(DEVICE_NAME, 115200);
 	 if (ifd_prn >=0){

 		szTmp = calloc(100,1);

 		time(&rawtime);
		timeinfo = localtime(&rawtime);

		dd.da_day = (char) timeinfo->tm_mday;
		dd.da_mon = (char) timeinfo->tm_mon+1;
		dd.da_year = (uint16_t)timeinfo->tm_year + 1900;

		tt.ti_hour = (unsigned char) timeinfo->tm_hour;
		tt.ti_min = (unsigned char) timeinfo->tm_min;
		tt.ti_sec = (unsigned char) timeinfo->tm_sec;
		tt.ti_hund = 0;

		//lcd_printf(ALG_CENTER, "UART %s", DEVICE_NAME);
		//lcd_printf(ALG_CENTER, "SETTING: 115200,8,n,1");

		//lcd_flip();
		//odroid
		//seri port nasıl set edilecek??
		//tty_property_config(ifd_prn, 115200, 8,'n',1,'n');

		do{
			prn_status = check_paper(ifd_prn);
			switch(prn_status)
			{
				case NEAR_END:
					debug_print("YAZICI KAGIDI BITMEK UZERE");
					printer_ok = 1;
					check_again = 0;
					break;
				case PAPER_END:
					//lcd_set_bk_color(COLOR_RED);
					//lcd_set_font_color(COLOR_YELLOW);
					lcd_flip();
					lcd_printf(ALG_CENTER, "YAZICI DURUMU => KAGIT BITMIS");//0.0.8
					lcd_printf(ALG_CENTER, "YENI RULO TAKIP BIR TUSA BASIN");//0.0.9
					lcd_printf(ALG_CENTER, "CANCEL TUSUNA BASARSANIZ");
					lcd_printf(ALG_CENTER, "BASKI ALMADAN DEVAM EDILIR");
					lcd_flip();
					MSJ_Ses("5343332313");
					//odroid
					//if(kb_getkey() == DIKS_ESCAPE){
					if(getch() == 0x1B){
						printer_ok = 0;
						check_again = 0;
						break;
					}
					else{
						printer_ok = 0;
						check_again = 1;
					}
					break;
				case PAPER_OK:
					//debug_print("YAZICI OK");
					printer_ok = 1;
					check_again = 0;
					break;
				default:
					//lcd_set_bk_color(COLOR_RED);
					//lcd_set_font_color(COLOR_YELLOW);
					lcd_flip();
					lcd_printf(ALG_CENTER, "YAZICI PROBLEMLI");
					lcd_printf(ALG_CENTER, "SORUNU COZUP BIR TUSA BASIN");
					lcd_printf(ALG_CENTER, "CANCEL TUSUNA BASARSANIZ");
					lcd_printf(ALG_CENTER, "BASKI ALMADAN DEVAM EDILIR");
					lcd_flip();
					MSJ_Ses("5343332313");
					//odroid
					//if(kb_getkey() == DIKS_ESCAPE){
					if(getch() == 0x1B){
						printer_ok = 0;
						check_again = 0;
						break;
					}
					else{
						printer_ok = 0;
						check_again = 1;
					}

					break;
			}
		}while(check_again);
/*
		t.tv_sec  = 0;
		t.tv_usec = 500000;

		FD_ZERO(&rfds);
		FD_SET(ifd_prn, &rfds);

		//do{
		write(ifd_prn, "\x1b\x76", 2);//0.0.91

		if ((select(ifd_prn + 1, &rfds, NULL, NULL, &t) > 0)&&
			FD_ISSET(ifd_prn, &rfds))
		{
			iTemp = read(ifd_prn, bTemp, sizeof(bTemp));
			debug_print("iTemp: %d, bTemp: %d", iTemp, bTemp[0]);
			if (iTemp > 0){//0.0.91
				//write(ifd_prn, bTemp, iTemp);
				//lcd_printf(ALG_LEFT, "Recived %d bytes...", iTemp);
				//lcd_flip();
				switch(bTemp[0]){
					case 0x03:
						debug_print("KAGIT AZALMIS");
						break;
					case 0x12:
						debug_print("KAGIT BITMIS");
						break;
					default:
						break;
				}

			}
		}//aslında burada hata vermek gerekecek.
		//}while();
*/
		//else {
		if(printer_ok){
			memcpy(szTmp, "\x18\x1b\x21\x08\x1b\x2d\x0\x1b\x32\x1d\x21\x11\x1b\x61\x01\x1b\x74\x02\x1b\x45\x01",18);
			//iTemp = strlen((char*)szTmp);
			//write(ifd_prn, szTmp, iTemp);
			serialPuts(ifd_prn, szTmp);

			memset(szTmp,0x00,100);
			memset(szTmp,'=',24);
			//iTemp = strlen((char*)szTmp);
			//write(ifd_prn, szTmp, iTemp);
			//write(ifd_prn, CRLF, 2);
			serialPuts(ifd_prn, szTmp);
			serialPuts(ifd_prn, CRLF);
			//debug_print("ilk satir-0: %d", strlen(rec_YAZICI.ACIKLAMA1));
			//memset(szTmp,0x00,100);
			//2.6.0
			//buffer overflow olmasın diye eklendi bu kontrol
			if(strlen(rec_YAZICI.ACIKLAMA1)<24){
                memset(szTmp,0x00,100);
                memset(szTmp,' ',(24-strlen(rec_YAZICI.ACIKLAMA1))/2);
                //iTemp = strlen((char*)szTmp);
                //write(ifd_prn, szTmp, iTemp);
                serialPuts(ifd_prn, szTmp);
            }
			//iTemp = strlen((char*)rec_YAZICI.ACIKLAMA1);
			//write(ifd_prn, rec_YAZICI.ACIKLAMA1, iTemp);
			//write(ifd_prn, CRLF, 2);
			serialPuts(ifd_prn, rec_YAZICI.ACIKLAMA1);
			serialPuts(ifd_prn, CRLF);
			//debug_print("ilk satir-1");
			if(strlen(rec_YAZICI.ACIKLAMA2)>0){
				//memset(szTmp,0x00,100);
				//2.6.0
                //buffer overflow olmasın diye eklendi bu kontrol
                if(strlen(rec_YAZICI.ACIKLAMA2)<24){
                    memset(szTmp,0x00,100);
                    memset(szTmp,' ',(24-strlen(rec_YAZICI.ACIKLAMA2))/2);
                    //iTemp = strlen((char*)szTmp);
                    //write(ifd_prn, szTmp, iTemp);
                    serialPuts(ifd_prn, szTmp);
                }
				//iTemp = strlen((char*)rec_YAZICI.ACIKLAMA2);
				//write(ifd_prn, rec_YAZICI.ACIKLAMA2, iTemp);
				//write(ifd_prn, CRLF, 2);
				serialPuts(ifd_prn, rec_YAZICI.ACIKLAMA2);
                serialPuts(ifd_prn, CRLF);
			}
			if(strlen(rec_YAZICI.ACIKLAMA3)>0){
				//memset(szTmp,0x00,100);
				//2.6.0
                //buffer overflow olmasın diye eklendi bu kontrol
                if(strlen(rec_YAZICI.ACIKLAMA3)<24){
                    memset(szTmp,0x00,100);
                    memset(szTmp,' ',(24-strlen(rec_YAZICI.ACIKLAMA3))/2);
                    //iTemp = strlen((char*)szTmp);
                    //write(ifd_prn, szTmp, iTemp);
                    serialPuts(ifd_prn, szTmp);
                }
				//iTemp = strlen((char*)rec_YAZICI.ACIKLAMA3);
				//write(ifd_prn, rec_YAZICI.ACIKLAMA3, iTemp);
				serialPuts(ifd_prn, rec_YAZICI.ACIKLAMA3);
				//write(ifd_prn, "ğüşiöçĞÜŞIÖÇ", iTemp);
				//write(ifd_prn, CRLF, 2);
				serialPuts(ifd_prn, CRLF);
			}

			//memset(szTmp,0x00,100);
			//2.6.0
			//buffer overflow olmasın diye eklendi bu kontrol
			if(strlen(rec_TERM.ACIKLAMA)<24){
                memset(szTmp,0x00,100);
                memset(szTmp,' ',(24-strlen(rec_TERM.ACIKLAMA))/2);
                //iTemp = strlen((char*)szTmp);
                //write(ifd_prn, szTmp, iTemp);
                serialPuts(ifd_prn, szTmp);
            }
			//iTemp = strlen((char*)rec_TERM.ACIKLAMA);
			//write(ifd_prn, rec_TERM.ACIKLAMA, iTemp);
			//write(ifd_prn, CRLF, 2);
			serialPuts(ifd_prn, rec_TERM.ACIKLAMA);
			serialPuts(ifd_prn, CRLF);
			sprintf(szTmp,"    %02d-%02d-%04d %02d:%02d\r\n",
											dd.da_day,dd.da_mon,dd.da_year,
											tt.ti_hour,tt.ti_min);
			//iTemp = strlen((char*)szTmp);
			//write(ifd_prn, szTmp, iTemp);
            serialPuts(ifd_prn, szTmp);

            memset(szTmp,0x00,100);
			memset(szTmp,'=',24);
			//iTemp = strlen((char*)szTmp);
			//write(ifd_prn, szTmp, iTemp);
			serialPuts(ifd_prn, szTmp);

			//write(ifd_prn, CRLF, 2);
			serialPuts(ifd_prn, CRLF);

			if(rec_YAZICI.KART_NO_BAS){
			//if(strcmp("BAS", rec_YAZICI.KART_NO_BAS) ==0){
				strcpy((char*)szTmp, "        ");
				//iTemp = strlen((char*)szTmp);
				//write(ifd_prn, szTmp, iTemp);
				serialPuts(ifd_prn, szTmp);
				strcpy((char*)szTmp, rec_Personel.KARTNO_HEX);
				//iTemp = strlen((char*)szTmp);
				//write(ifd_prn, szTmp, iTemp);
				serialPuts(ifd_prn, szTmp);
			}
			//write(ifd_prn, CRLF, 2);
            serialPuts(ifd_prn, CRLF);

			memset(szTmp,0x00,100);
			memset(szTmp,' ',(24-strlen(rec_Personel.ADI))/2);
			//iTemp = strlen((char*)szTmp);
			//write(ifd_prn, szTmp, iTemp);
			serialPuts(ifd_prn, szTmp);
			strcpy((char*)szTmp, rec_Personel.ADI);
			//iTemp = strlen((char*)szTmp);
			//write(ifd_prn, szTmp, iTemp);
			serialPuts(ifd_prn, szTmp);
			//write(ifd_prn, CRLF, 2);
			serialPuts(ifd_prn, CRLF);
			memset(szTmp,0x00,100);
			memset(szTmp,' ',(24-strlen(rec_Personel.SOYAD))/2);
			//iTemp = strlen((char*)szTmp);
			//write(ifd_prn, szTmp, iTemp);
			serialPuts(ifd_prn, szTmp);
			strcpy((char*)szTmp, rec_Personel.SOYAD);
			//iTemp = strlen((char*)szTmp);
			//write(ifd_prn, szTmp, iTemp);
            serialPuts(ifd_prn, szTmp);
			//write(ifd_prn, CRLF, 2);
			serialPuts(ifd_prn, CRLF);

			memset(szTmp,0x00,100);
			memset(szTmp,'-',24);
			//iTemp = strlen((char*)szTmp);
			//write(ifd_prn, szTmp, iTemp);
			//write(ifd_prn, CRLF, 2);
			serialPuts(ifd_prn, szTmp);
			serialPuts(ifd_prn, CRLF);

			sprintf(szTmp,"ESKI BAKIYE: %4ld.%02ld TL",(unsigned long) ((rec_Personel.BAKIYE+rec_Personel.HARCANAN)/100),(unsigned long)(rec_Personel.BAKIYE+rec_Personel.HARCANAN)%100);
			//iTemp = strlen((char*)szTmp);
			//write(ifd_prn, szTmp, iTemp);
			//write(ifd_prn, CRLF, 2);
			serialPuts(ifd_prn, szTmp);
			serialPuts(ifd_prn, CRLF);

			sprintf(szTmp,"HARCANAN   : %4lu.%02lu TL",(unsigned long) (rec_Personel.HARCANAN/100),(unsigned long)rec_Personel.HARCANAN%100);
			//iTemp = strlen((char*)szTmp);
			//write(ifd_prn, szTmp, iTemp);
			//write(ifd_prn, CRLF, 2);
			serialPuts(ifd_prn, szTmp);
			serialPuts(ifd_prn, CRLF);

			sprintf(szTmp,"KALAN      : %4ld.%02ld TL",(unsigned long) (rec_Personel.BAKIYE/100),(unsigned long)rec_Personel.BAKIYE%100);
			//iTemp = strlen((char*)szTmp);
			//write(ifd_prn, szTmp, iTemp);
			//write(ifd_prn, CRLF, 2);
			serialPuts(ifd_prn, szTmp);
			serialPuts(ifd_prn, CRLF);

			memset(szTmp,0x00,100);
			memset(szTmp,'-',24);
			//iTemp = strlen((char*)szTmp);
			//write(ifd_prn, szTmp, iTemp);
			//write(ifd_prn, CRLF, 2);
			serialPuts(ifd_prn, szTmp);
			serialPuts(ifd_prn, CRLF);

			//Total Cut and automatic paper moving back
			//strcpy((char*)szTmp, "\x1b\x69");
			strcpy((char*)szTmp, "\x1c\xc0\x34");
			//iTemp = strlen((char*)szTmp);
			//write(ifd_prn, szTmp, iTemp);
			serialPuts(ifd_prn, szTmp);

			//usleep(100000);
			napms(100);
		}
		//2.3.1
		free(szTmp);
	}else{
		lcd_printf(ALG_CENTER, "OPEN UART Failed.");
		lcd_flip();
		getch();//odroid//kb_getkey();
	}


	if (ifd_prn >=0){
		//close(ifd_prn);
		serialClose(ifd_prn);
		ifd_prn = -1;
	}

//2.3.1
//yukarı alındı, calloc çağrılmadan free edilme riskine karşı.
// free(szTmp);

}


void YAZICI_FisYemekhane_Custom_Bilecik()
{
char *szTmp;
char CRLF[] ="\r\n";
struct_date d;
struct_time t;
int ifd_prn = -1;

    ifd_prn = serialOpen(DEVICE_NAME, 115200);
 	if (ifd_prn <0){
        lcd_printf(ALG_CENTER, "OPEN UART Failed.");
		lcd_flip();
		getch();//odroid//kb_getkey();
		return;
 	}

    szTmp = calloc(100,1);

    get_date(&d);
    get_time(&t);
    //SerPort_Clear (1);
    serialPuts(ifd_prn,  "\x1b\x45\x1");//Bold
    serialPuts(ifd_prn,  "\x1b\x61\x31");//Center
    serialPuts(ifd_prn, "Harcama Noktasi Fisi");
    serialPuts(ifd_prn, CRLF);
    memset(szTmp,0x00,100);
    memset(szTmp,'-',24);
    serialPuts(ifd_prn, szTmp);
    serialPuts(ifd_prn, CRLF);


    serialPuts(ifd_prn, rec_YAZICI.ACIKLAMA1);
    serialPuts(ifd_prn, CRLF);
    serialPuts(ifd_prn, rec_YAZICI.ACIKLAMA2);
    serialPuts(ifd_prn, CRLF);
    serialPuts(ifd_prn, rec_YAZICI.ACIKLAMA3);
    serialPuts(ifd_prn, CRLF);
    serialPuts(ifd_prn, CRLF);

    serialPuts(ifd_prn,  "\x1b\x61\x30");//Left Justify
    sprintf(szTmp,"%02d.%02d.%04d %02d:%02d:%02d\r\n",
                                d.da_day,d.da_mon,d.da_year,
                                t.ti_hour,t.ti_min, t.ti_sec);
    serialPuts(ifd_prn, szTmp);
    serialPuts(ifd_prn, CRLF);

    if(rec_YAZICI.KART_NO_BAS){
    memset(szTmp,0x00,100);
    memcpy(szTmp,rec_Personel.KARTNO_HEX, 4);
    memcpy(szTmp+4, "****", 4);
    serialPuts(ifd_prn, szTmp);
    serialPuts(ifd_prn, CRLF);
    }

    if(rec_YAZICI.SIRA_NO_BAS){
    serialPuts(ifd_prn,  "\x1b\x61\x32");//Right Justify
    serialPuts(ifd_prn,  "\x1d\x21\x22");
    memset(szTmp,0x00,100);
    sprintf(szTmp, "%5u ", SAYAC.SAYI);
    serialPuts(ifd_prn, szTmp);
    serialPuts(ifd_prn, CRLF);
    serialPuts(ifd_prn,  "\x1b\x40");
    serialPuts(ifd_prn,  "\x1b\x45\x1");//Bold
    }

    serialPuts(ifd_prn,  "\x1b\x61\x30");//Left Justify
    memset(szTmp,0x00,100);
    strncpy(szTmp,rec_Personel.ADI, 1);
    memset(szTmp+1, '*', 4);
    memset(szTmp+5, ' ', 1);
    strncpy(szTmp+6, rec_Personel.SOYAD, 1);
    memset(szTmp+7, '*', 4);
    //aWinToDos(szTmp);
    serialPuts(ifd_prn, szTmp);
    serialPuts(ifd_prn, CRLF);
    serialPuts(ifd_prn, CRLF);

    sprintf(szTmp,"Yemek Bedeli: %3lu.%02lu TL",(unsigned long) (rec_Personel.HARCANAN/100),(unsigned long)rec_Personel.HARCANAN%100);
    serialPuts(ifd_prn, szTmp);
    serialPuts(ifd_prn, CRLF);

    serialPuts(ifd_prn, CRLF);
    serialPuts(ifd_prn, CRLF);

    serialPuts(ifd_prn, "\x1c\xc0\x34");//Total Cut&Automatic Paper Moving Back


    //2.5.2
    //kapatılmıyordu, kapattım artık
    serialClose(ifd_prn);

    free(szTmp);

}
