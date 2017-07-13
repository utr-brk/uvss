/*
 * reader.c
 *
 *  Created on: 11 Haz 2014
 *      Author: Burak
 */


#include <stdlib.h>
#include <string.h>
//#include <dos.h>

#include "reader.h"
//#include "serport.h"
#include "com.h"
//#include "mfr.h"
#include "smm.h"
#include "lcd.h"
#include "config.h"
//#include "resource.h"
#include "mesaj.h"
#include "personel.h"
//#include "watchdt.h"
#include "tool.h"
//#include <posapi.h>
#include <errno.h>
//#include <iso14443.h>
//#include <mifone.h>
//#include <uart.h>
#include <poll.h>
//1.7.0
#include "admin.h"
//1.8.0
#include "serport.h"

//2.2.7
#include "main.h"

//#define DEVICE_NAME "/dev/ttyS2"

//2.5.2
#include "errno.h"

//2.0.4
#define File_KART	 "/home/odroid/Solidus/KARTLAR.DAT"

char READER_TYPE; //=0;

int ifd[2] = {-1,-1}; //mifare reader file decsriptor
int pinpad = -1;

//2.4.0
/*
//2.0.4
typedef struct{
	char KART_NO[21];
	char SICIL_NO[21];
	char ADISOYADI[21];
	char TZ_INOUT;
	char TZ_ONOFF;
	char MSJ_NO;
} PDKS_KART_Config;
*/

//2.4.0
PDKS_KART_Config KART;

//2.0.4
/// Personel Bilgisi
char PDKS_KartBul (char* KartNo,PDKS_KART_Config *KART,char MSJSIL)
{
    char ret=0x00;
    FILE * inp;
    long flen;
    long rlen;
    long pos=0;
    long lastpos=0;
    long l,r;
    int k=1;
    size_t blen;
//2.5.2
    errno = 0;

    blen=strlen(KartNo);

    //2.5.2
    //if ((inp=fopen(File_KART,"r+b"))==NULL) return ret;
    inp=fopen(File_KART,"r+b");
    if(inp ==NULL)
    {
        fprintf(stderr, "Couldn't open file %d; %s;\n",
                errno, strerror(errno));
        return ret;
    }

    rlen=sizeof(PDKS_KART_Config);
    fseek(inp,0L,SEEK_END);
    flen = ftell(inp)/rlen;

    if (flen==0)
    {
        fclose(inp);
        return ret;
    }

    l=1;
    r=flen+1;

    while(1)
    {
        pos=(l+r)/2;
        if(lastpos==pos) break; //Yoksa
        fseek(inp,(pos-1)*rlen,SEEK_SET);
        fread(KART,rlen,1,inp);
        k=strncmp(KART->KART_NO,KartNo,blen);

        //printf("%d\n",pos);

        if (k==0)
        {
            if (MSJSIL==0x01)
            {
                fseek(inp,(pos-1)*rlen,SEEK_SET);
                KART->MSJ_NO=0x00; //Personel Mesajn Sil
                fwrite(KART,rlen,1,inp);
            }
            break; //Bulduysa
        }

        if (k<0) //ILERLE
        {
            l=pos;
        }
        if (k>0) //GERILE
        {
            r=pos;
        }
        lastpos=pos;
    }
    fclose(inp);
    if (k==0)
    {
        ret=0x01;
    }

    return ret;
}

void READER_Init(void)
{
    int i;
    //5.4.4
    //eger ikinci reader aktif degilse comportu acma
    //ama sayet terminal turu harcama terminaliyse ac
    for(i=0; i<2; i++)
        if (rec_TERM.READERS[i].AKTIF) COM_init(i, ifd);

    //6.1.0 yazici secenegi eklendi
    //if(rec_TERM.TERM_TIP == TERM_HARCAMA)
    if((rec_TERM.TERM_TIP == TERM_HARCAMA) || rec_YAZICI.AKTIF)
        if (!rec_TERM.READERS[1].AKTIF)
            COM_init(1, ifd);

}
/*
unsigned int COM_ReadBuffer(int com_port,char * Buffer)
{
	unsigned int i,j,max;
	//4.8.500
	int brk = 1;
	int c;
	max = COM_avail(ifd[com_port],100);
	if (max==0) return 0;
	j=0;
	while (max>0 && brk)
	{
		for(i=0;i<max;i++)
		{
			c = serialGetchar(ifd[com_port]);
			Buffer[j]=(char)c;
			//j++;
			//4.8.500
			if(j++ > 512){
			 brk = 0;
			 break;
			}
    }
    max=COM_avail(ifd[com_port],max*3);
  }
  serialFlush(ifd[com_port]);
	return j;
}
*/
/*
unsigned int COM_ReadBuff(int com_port, char * Buffer, int adet)
{
	unsigned int i, max;
	//4.8.500
	//int brk = 1;
	int c;
	max = COM_avail(ifd[com_port],500);
	if (max<adet) return 0;
	//j=0;
	//while (max>0 && brk)
	//{
    for(i=0;i<adet;i++)
	{
		c = serialGetchar(ifd[com_port]);
		Buffer[i]=(char)c;
		//j++;
		//4.8.500
	//	if(j++ > 18){
		// brk = 0;
		// break;
		//}
    }
    //max=COM_avail(ifd[com_port],max*3);
    //}
    //serialFlush(ifd[com_port]);
	return i;//j;
}
*/
/*
//4.8.500
//rf-id icin sayili data okusun diye yapildi
unsigned int COM_ReadBuff(int com_port,char * Buffer, int adet)
{
	unsigned int i, max;
	int c;
	max = COM_avail(ifd[com_port],500);
	if (max==0) return 0;
	napms(5);
	for(i=0; i< adet; i++){
        c = serialGetchar(ifd[com_port]);
        if(c>-1)
            Buffer[i] = (char) c;
        else
            return i-1;
	}
	//serialFlush(ifd[com_port]);
	return i;
}
*/
char RFID_Read(int Port, unsigned char *CardNum, unsigned char *CardNumHex)
{
    unsigned int max;
    char* sz;
    char ret = 0xFF;//2.3.1
    unsigned char CardMask[12];
    unsigned char CardMaskHex[6];
    //unsigned char CardNumHex[5]; //4.6.100 [6] idi
    //char szStr[100];

    //if (!COM_avail(ifd[Port],10)) return 0x01;

    //4.8.500
    sz = (char*) malloc(128);
    memset(sz, 0x00, 128);
    //	sz = (char*) malloc(4096);
    //	memset(sz, 0x00, 4096);
    max =  COM_ReadBuff(ifd[Port], sz, 18);

    //max = COM_ReadBuffer(ifd[Port], sz);
    //printf(sz);
    if (max == 0)
    {
        free(sz);
        return 0x01; //Data yoksa cik
    }
//	printf("\n[COM%d]: RF-ID Len: %d\n", Port+1, max);
    //aHexToString(sz, szStr);
    //printf("Data: %s\n", sz);

    //switch(max)
    switch(max)
    {
    case 18://kart bilgisi gelmis
        memcpy(CardMaskHex, sz+4, 6);
        Str2Hex(CardMaskHex, 6, CardMask);
        //printf("CardMask: %s\n", CardMask);
        //if(strncmp(CardMaskHex, rec_TERM.CARDMASK, 6)!=0)
        if((RF_ID_MASK_AKTIF) && (strncmp((char *) CardMask, RF_ID_MASK, 12)!=0))
        {
            //burann kart degil
            //free(sz);
            //printf("buranin degil\n");

            //4.9.6
            //TOOL_FailBeep();

            //SerPort_Clear(Port);
            ret = 0x02;
        }
        else
        {
            //4.6.100
            //memcpy(CardNumHex, sz+10, 6);
            //1.8.0
            //memcpy(CardNum, sz+11, 5);
            //Str2Hex(CardNum, 5, CardNumHex);
            memcpy(CardNum, sz+12, 4);
            Str2Hex(CardNum, 4, CardNumHex);
            //memcpy(Card)
            //printf("CardNum: %s\n", CardNum);
            //4.6.500 surekli beeplemesin diye cikartildi
            //TOOL_SuccessBeep();

            //SerPort_Clear(Port);
            ret = 0x00; //RF_ID OK
        }
        //4.8.500
        //LCD_PrintCenter(4,CardMask);
        //LCD_PrintCenter(5,CardNum);
        break;
    case 14://beat msg gelmis
        if(!strcmp((char *) sz, "\x0D\x00\xAA\x00\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\x83\x28"))
            //TOOL_StringToSound("1210");
            ret = 0x01;//hata mi verecek??
        break;
    default:
        ret = 0x01;
        break;
    }

    free(sz);
    return ret;
}

char READER_Read(void)
{
    int r,i,k;
    unsigned char ret = 0xFF;//Sector,Block;
    unsigned char *KartNo;//m8110 unsigned char idi
    unsigned char *KartNoHex;//m8110 unsigned char idi

    guint16 barkod_uzunluk;

    GdkRGBA bg_color;
    GdkRGBA font_color;

//struct pollfd mypoll ={STDIN_FILENO, POLLIN|POLLPRI};
    char    barcode[21];
//2.1.0
    char barcode1[7], barcode2[7];//, barcode_control[3];
//2.4.0
    /*
    //2.0.4
    PDKS_KART_Config KART;
    */
    KartNo = (unsigned char *) calloc(4, 1);//m8110 unsigned char idi
    KartNoHex = (unsigned char *) calloc(10, 1);//m8110 unsigned char idi

    //1.0.1
    //dogru porttan gondesin diye
    READER_TYPE = READER_MIFARE;

    //2.1.0
    if(ISKUR)
    {
        //1.0.0
        //burada barcode okutulmaya çalışılacak.
        //okursa READER_TYPE = READER_BARCODE yapılıp çıkılacak,
        //okumazsa mifare'ye, yani aşağıya devam edilecek.
        memset(barcode, 0, 21);
        //1.0.5
        barkod_uzunluk = gtk_entry_get_text_length(GTK_ENTRY(txtBarcode));
        //barkod_uzunluk = 15;
        if(barkod_uzunluk == 15)
        {
            sprintf(barcode, "%15s", gtk_entry_get_text(GTK_ENTRY(txtBarcode)));
            //fffHHddssyymmMM 764062512163205 2016-05-25-16:32:18.543
            //sprintf(barcode, "%s", "223022703160406");
            get_date(&rec_Personel.OKUMA_TARIHI);
            sprintf(barcode1, "%02d%02d%02d", rec_Personel.OKUMA_TARIHI.da_day, rec_Personel.OKUMA_TARIHI.da_mon, rec_Personel.OKUMA_TARIHI.da_year-2000);
            memcpy(barcode2, barcode+5, 2);
            memcpy(barcode2+2, barcode+13, 2);
            memcpy(barcode2+4, barcode+9, 2);

            //strncpy(barcode2, barcode[5], 2);
            //strncat(barcode2, barcode[13], 2);
            //strncat(barcode2, barcode[9], 2);
            if(memcmp(barcode1, barcode2, 6) ==0)
            {
                gtk_entry_set_text(GTK_ENTRY(txtBarcode), "");
                get_time(&rec_Personel.OKUMA_SAATI);
                READER_TYPE = READER_BARCODE;
                sprintf(Barcode_Fis, "%15s", barcode);
                free(KartNo);
                free(KartNoHex);
                //2.1.0
                //return 0x1; //1 ve 2 smm reader'lar, 3 barcode reader
                return 3; //3 barkod okuyucu, 4 keypad
            }
        }
        //2.3.0
        //barkodu sıfırla
        gtk_entry_set_text(GTK_ENTRY(txtBarcode), "");
    }
    if(rec_TERM.TERM_TIP == TERM_YEMEKHANE)
    {
        //1.0.0
        //burada barcode okutulmaya çalışılacak.
        //okursa READER_TYPE = READER_BARCODE yapılıp çıkılacak,
        //okumazsa mifare'ye, yani aşağıya devam edilecek.
        memset(barcode, 0, 21);
        //1.0.5
        barkod_uzunluk = gtk_entry_get_text_length(GTK_ENTRY(txtBarcode));
        if(barkod_uzunluk == 17)
        {
            sprintf(barcode, "%17s", gtk_entry_get_text(GTK_ENTRY(txtBarcode)));
            //1.0.7 Kiosktan basılan ve 'K' ile başlayan barkodları da okusun diye
            //if((barcode[0] == 'P') && (barcode[4] == 'T')){
            if(((barcode[0] == 'P') || (barcode[0] == 'K')) && (barcode[4] == 'T'))
            {
                gtk_entry_set_text(GTK_ENTRY(txtBarcode), "");
                get_date(&rec_Personel.OKUMA_TARIHI);
                get_time(&rec_Personel.OKUMA_SAATI);
                READER_TYPE = READER_BARCODE;
                sprintf(Barcode_Fis, "%17s", barcode);
                free(KartNo);
                free(KartNoHex);
                //1.0.4
                //barcode okuyucu 1. reader gibi davranacak
                //2.1.0
                //return 0x1; //1 ve 2 smm reader'lar, 3 barcode reader
                return 3; //3 barkod okuyucu, 4 keypad
            }
        }
        //2.3.0
        //barkodu sıfırla
        gtk_entry_set_text(GTK_ENTRY(txtBarcode), "");
        /*
                if(poll(&mypoll, 1, 100)){
                    scanf("%21s", barcode);
                    //burada P, T kontrolleri yap
                    if((barcode[0] == 'P') && (barcode[4] == 'T')){
                        get_date(&rec_Personel.OKUMA_TARIHI);
                        get_time(&rec_Personel.OKUMA_SAATI);
                        READER_TYPE = READER_BARCODE;
                        sprintf(Barcode_Fis, "%17s", barcode);
                        free(KartNo);
                        free(KartNoHex);
                        return 0x3; //1 ve 2 smm reader'lar, 3 barcode reader
                    }
                }*/
    }

    r=0;
    for(i=0; i<2; i++)
    {
        if(rec_TERM.READERS[i].AKTIF)
        {
            memset(KartNo,0x00,4);
            memset(KartNoHex,0x00,10);


            //1.8.0
            if(rec_TERM.TERM_TIP == TERM_KAPI)
            {
                if(Okuyucu_TIP[i] == TIP_RF_ID)
                {
                    if(RFID_Read(i, KartNo, KartNoHex) == 0)
                    {
                        ret =  SMM_ERR_NONE;
                        get_date(&rec_Personel.OKUMA_TARIHI);
                        get_time(&rec_Personel.OKUMA_SAATI);
                        memcpy(rec_Personel.KARTNO,KartNo,4);
                        memcpy(rec_Personel.KARTNO_HEX,KartNoHex,8);
                        r = i+1;
                        READER_TYPE = READER_RFID;
                        break;
                    }
                    else
                        ret = SMM_ERR_CARDNONE;
                }
            }
            //2.0.0
            //brk silmeyi unutma
            //Okuyucu_TIP[0] = TIP_MIFARE;
            //2.0.0
            //if(Okuyucu_TIP[i] == TIP_MIFARE){
            if((Okuyucu_TIP[i] == TIP_MIFARE)||(Okuyucu_TIP[i] == TIP_CN_MFR)||(Okuyucu_TIP[i] == TIP_CN_KYB))
            {
                //2.0.0
                //brk silmeyi unutma
                //Okuyucu_TIP[0] = 3;
                for(k=0; k<3; k++)
                {
                    ret = SMM_ReadCardNumber(i,KartNo, KartNoHex);
                    if(ret == SMM_ERR_NONE)
                    {
                        //1.7.0
                        if(memcmp(KartNoHex, rec_TERM.ADMIN_CARD, 8) == 0)
                        {
                            MSJ_Ses("314231");
                            //WDT_Disable();
                            ADMIN_Menu();
                            CONFIG_Save();
                            READER_Uninit(COM1);
                            READER_Uninit(COM2);
                            lcd_clean();
                            lcd_printf_ex(ALG_CENTER, 20, "Ayarlar yenileniyor...");
                            lcd_flip();

                            gtk_clean();

                            GdkRGBA font_color;
                            font_color.alpha = 1;
                            font_color.blue = 1;
                            font_color.green = 1;
                            font_color.red = 0;
                            GdkRGBA bg_color;
                            bg_color.alpha = 1;
                            bg_color.blue = 0;
                            bg_color.green = 0;
                            bg_color.red = 1;
                            gtk_widget_override_color(label1, GTK_STATE_FLAG_NORMAL, &font_color);
                            gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
                            gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);
                            gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                            gtk_printf(label1, "AYARLAR YENİLENİYOR...");
                            while(gtk_events_pending())
                                gtk_main_iteration();
                            MSJ_Ses(rec_TERM.SES_IZIN_OK);
                            lcd_uninit();
                            sleep(1);

                            raise(SIGTERM);
                            //1.9.4
                            sleep(1);
                            return 0;
                        }

                        //odroid
                        //aşağısı yapılmalı bir şekilde
                        //6.7.0
                        if(okuma_bekleme)
                            napms(okuma_bekleme);


                        //2.4.0
                        /*
                        //2.1.0
                        //iskur'sa sadece kart no oku geç.
                        if(!ISKUR){
                            //2.0.4
                            //ret = SMM_ReadPersonel(i);
                            //eğer kapı terminaliyse ve okuyuculardan herhangi birisi CN56 reader ise login olmak yerine kartlar.dat dosyasında ara kartı
                            if((rec_TERM.TERM_TIP == TERM_KAPI) && ((Okuyucu_TIP[0] == 2)||(Okuyucu_TIP[0] == 3)||(Okuyucu_TIP[1] == 2)||(Okuyucu_TIP[1] == 3))){
                                if(!PDKS_KartBul((char *)KartNoHex,&KART,0x00))
                                    ret = SMM_ERR_UNV;
                                else
                                    ret = SMM_ERR_NONE;

                            }else//2.0.5 burası unutulmuş
                                ret = SMM_ReadPersonel(i);
                        }//iskur'sa master kart okumaya geçebilecek mi?
                        */
                        //2.4.3
                        //kapı modunda değilsen ya da ISKUR degilsen diğer işleri de yap.
                        if((rec_TERM.TERM_TIP != TERM_KAPI) && (!ISKUR))
                        {
                            ret = SMM_ReadPersonel(i);
                        }
                        //2.4.0
                        //artık kapı modunda sadece kartnumarası okunup geçiliyor.
                        //haliyle şifre işlemi vs yok, sadece master kart mı diye bakmak yeterli
                        //if(ret==SMM_ERR_UNV){
                        //2.4.3
                        if((rec_TERM.TERM_TIP == TERM_KAPI) || (ret==SMM_ERR_UNV))
                        {
                            ret = SMM_ReadMaster(i);
                            //1.6.0
                            //master kart 2. blok'da SHUTDOWN yazıyorsa terminali kapa,
                            //REBOOT yazıyorsa reboot et.
                            if(ret == SMM_ERR_SHUTDOWN)
                            {
                                //2.0.6
                                READER_Uninit(COM1);
                                READER_Uninit(COM2);
                                lcd_clean();
                                lcd_printf_ex(ALG_CENTER, 20, "Terminal Kapatiliyor...");
                                lcd_flip();

                                GdkRGBA bg_color;
                                bg_color.alpha = 1;
                                bg_color.blue = 0;
                                bg_color.green = 1;
                                bg_color.red = 0;
                                gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
                                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12
                                gtk_printf(label1, "\n\nSOLI-TERM KAPATILIYOR!\n\n");
                                while(gtk_events_pending())
                                    gtk_main_iteration();
                                MSJ_Ses("1221314151");
                                sleep(1);
                                system("sudo shutdown -h now");
                                sleep(1);
                            }
                            if(ret == SMM_ERR_REBOOT)
                            {
                                //2.0.6
                                READER_Uninit(COM1);
                                READER_Uninit(COM2);
                                lcd_clean();
                                lcd_printf_ex(ALG_CENTER, 20, "Terminal Yeniden Baslatiliyor...");
                                lcd_flip();

                                GdkRGBA bg_color;
                                bg_color.alpha = 1;
                                bg_color.blue = 0;
                                bg_color.green = 1;
                                bg_color.red = 0;
                                gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
                                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12
                                gtk_printf(label1, "\n\nSOLI-TERM YENİDEN BAŞLATILIYOR!\n\n");
                                while(gtk_events_pending())
                                    gtk_main_iteration();
                                MSJ_Ses("1221314151");
                                sleep(1);
                                system("sudo shutdown -r now");
                                sleep(1);
                            }
                            //2.2.5
                            if(ret == SMM_ERR_DATETIME)
                            {
                                lcd_clean();
                                lcd_printf_ex(ALG_CENTER, 20, "Tarih Saat Ayarlaniyor...");
                                lcd_flip();

                                GdkRGBA bg_color;
                                bg_color.alpha = 1;
                                bg_color.blue = 0;
                                bg_color.green = 1;
                                bg_color.red = 0;
                                gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
                                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12
                                gtk_printf(label1, "\n\nTARİH SAAT AYARLANIYOR!\n\n");
                                while(gtk_events_pending())
                                    gtk_main_iteration();
                                MSJ_Ses("1221314151");
                                if(main_TS_Guncelle())
                                {
                                    GdkRGBA bg_color;
                                    bg_color.alpha = 1;
                                    bg_color.blue = 0;
                                    bg_color.green = 1;
                                    bg_color.red = 0;
                                    gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
                                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12
                                    gtk_printf(label1, "\n\nSOLI-TERM YENİDEN BAŞLATILIYOR!\n\n");
                                    while(gtk_events_pending())
                                        gtk_main_iteration();
                                    MSJ_Ses("1221314151");
                                    sleep(1);
                                    free(KartNo);
                                    free(KartNoHex);
                                    raise(SIGTERM);
                                    sleep(1);
                                    return 0;
                                }
                                else
                                {
                                    //2.2.7
                                    DATE_TIME_OK = 0;
                                    GdkRGBA bg_color;
                                    bg_color.alpha = 1;
                                    bg_color.blue = 0;
                                    bg_color.green = 1;
                                    bg_color.red = 0;
                                    gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
                                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12
                                    gtk_printf(label1, "\n\nTARİH SAAT AYARLANAMADI!\n\n");
                                    while(gtk_events_pending())
                                        gtk_main_iteration();
                                    MSJ_Ses("1221314151");
                                    sleep(1);
                                }
                            }

                            if(ret==SMM_ERR_NONE)
                            {
                                CONFIG_Save();
                                lcd_printf(ALG_CENTER,"MASTER KEY OK !");
                                lcd_flip();

                                GdkRGBA bg_color;
                                bg_color.alpha = 1;
                                bg_color.blue = 0;
                                bg_color.green = 1;
                                bg_color.red = 0;
                                gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
                                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12
                                gtk_printf(label1, "\n\n\nMASTER KEY OK !");
                                while(gtk_events_pending())
                                    gtk_main_iteration();
                                MSJ_Ses("1221314151");
                                MSJ_OK=0;
                                z_MSJ_SURE_SN=0;
                                break;
                            }
                            //2.4.0
                            //artık sadece kart no okunduğu için burada solidus kartı mı değil mi bilinemez.
                            //2.4.3
                            //kapı değilse normal işlere devam ediliyor.
                            ///*
                            if((rec_TERM.TERM_TIP != TERM_KAPI) && (ret==SMM_ERR_UNV))
                            {
                                lcd_clean();
                                gtk_clean();
                                gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);
                                gtk_printf(label1, "%s\n%s %s\n%s",rec_Personel.KARTNO_HEX, rec_Personel.ADI, rec_Personel.SOYAD, "SOLİDUS KARTI DEĞİL");
                                gtk_label_set_text(GTK_LABEL(label_status), "               SOLİDUS KARTI DEĞİL");
                                gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_ERROR);//GTK_MESSAGE_ERROR
                                lcd_flip();
                                MSJ_Goster(MSJ_TIP_NK);
                                //1.8.1
                                MSJ_Ses(rec_TERM.SES_TANIMSIZ);
                                break;
                            }
                            //2.4.3
                            //kapı ise ve admin kart değilse
                            //kart numarası ile devam et.
                            if(((rec_TERM.TERM_TIP == TERM_KAPI))||(ret==SMM_ERR_NONE))
                            {
                                get_date(&rec_Personel.OKUMA_TARIHI);
                                get_time(&rec_Personel.OKUMA_SAATI);
                                memcpy(rec_Personel.KARTNO,KartNo,4);
                                memcpy(rec_Personel.KARTNO_HEX,KartNoHex,8);
                                r = i+1;
                                //1.0.0
                                READER_TYPE = READER_MIFARE;
                                ret = SMM_ERR_NONE;
                                break;
                            }
                            //*/
                            //2.4.0
                            //sadece kart no okunup geçiliyor, şifre falan kalmadı.
                            //2.4.3
                            //kapı değilse normal işlere devam ediliyor
                            ///*
                        }
                        else //1.7.0
                            if((rec_TERM.TERM_TIP != TERM_KAPI) && (ret==SMM_ERR_SIFRE))
                            {
                                //izin verilmedi
                                lcd_clean();
                                gtk_clean();
                                gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);
                                gtk_printf(label1, "%s\n%s %s\n%s",rec_Personel.KARTNO_HEX, rec_Personel.ADI, rec_Personel.SOYAD, "İZİN VERİLMEDİ");
                                gtk_label_set_text(GTK_LABEL(label_status), "                   ŞİFRE HATASI");
                                gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_ERROR);//GTK_MESSAGE_ERROR
                                lcd_flip();
                                MSJ_Goster(MSJ_TIP_NK);
                                //1.8.1
                                MSJ_Ses(rec_TERM.SES_TANIMSIZ);
                                break;
                                //sleep(1);
                            }
                            else
                                //2.4.3
                                //kapı değilse normal işlere devam ediliyor
                                //*/
                                //2.4.0
                                //buraya kart no okutulup gelindi
                                //bir daha bu kontrol yapılmamalı, çünkü yukarıda master kart işlerinde
                                //ret değeri bozulmuş olabilir.
                                //
                                //2.4.3
                                //kapı değilse normal işlere devam ediliyor
                                if(((rec_TERM.TERM_TIP == TERM_KAPI))||(ret==SMM_ERR_NONE))
                                {
                                    get_date(&rec_Personel.OKUMA_TARIHI);
                                    get_time(&rec_Personel.OKUMA_SAATI);
                                    memcpy(rec_Personel.KARTNO,KartNo,4);
                                    memcpy(rec_Personel.KARTNO_HEX,KartNoHex,8);
                                    r = i+1;
                                    //1.0.0
                                    READER_TYPE = READER_MIFARE;
                                    ret = SMM_ERR_NONE;
                                    break;
                                }
                    }
                    if (ret==SMM_ERR_CARDNONE) break;
                }
            }
            //1.7.1
            //if ((ret==SMM_ERR_NONE)||(ret==SMM_ERR_CARDNONE)) break;
            if ((ret==SMM_ERR_NONE)) break;//||(ret==SMM_ERR_CARDNONE)) break;
        }
    }
    if (ret==SMM_ERR_CARDERR)
    {
        lcd_clean();
        lcd_printf(ALG_CENTER,"");
        lcd_printf(ALG_CENTER,"");
        lcd_printf(ALG_CENTER,"OKUMA HATASI");//RESOURCE_ReadLine(400));// "KART OKUMA HATASI"
        lcd_printf(ALG_CENTER,"KARTI OKUYUCUYA" );//RESOURCE_ReadLine(400));// "KART OKUMA HATASI"
        lcd_printf(ALG_CENTER,"YAKIN TUTUNUZ" );//RESOURCE_ReadLine(400));// "KART OKUMA HATASI"
        lcd_printf(ALG_CENTER,"3 SN SONRA OKUR" );//RESOURCE_ReadLine(400));// "KART OKUMA HATASI"
        lcd_flip();
        gtk_clean();
        gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);
        bg_color.alpha = 1;
        bg_color.blue = 0;
        bg_color.green = 0;
        bg_color.red = 1;
        font_color.alpha = 1;
        font_color.blue = .7;
        font_color.green = .01;
        font_color.red = 0.1;
        gtk_widget_override_color(label1, GTK_STATE_FLAG_NORMAL, &font_color);
        gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
        gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12
        gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_ERROR);//GTK_MESSAGE_ERROR
        gtk_printf(label1, "OKUMA HATASI\nKARTI OKUYUCUYA\nYAKIN TUTUNUZ\n3 SN SONRA OKUR");
        while(gtk_events_pending())
            gtk_main_iteration();
        for (i=0; i<2; i++)
        {
            //while(gtk_events_pending())
            //    gtk_main_iteration();
            MSJ_Ses(rec_TERM.SES_OKUMA_NK);
        }

        for (i=0; i<3; i++)
        {
            //1.8.1
            //while(gtk_events_pending())
            //    gtk_main_iteration();
            napms(500);
            MSJ_Ses(rec_TERM.SES_OKUMA_NK);
        }
        lcd_clean();
        gtk_clean();
        gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x, logo_y);

        gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_WARNING);//GTK_MESSAGE_ERROR
        while(gtk_events_pending())
            gtk_main_iteration();
        MSJ_OK=0;
        z_MSJ_SURE_SN=0;
    }

    free(KartNo);
    free(KartNoHex);
    return r;
}

//7.0.0
char READER_Rezervasyonlu_Read(void)
{

    char r,i,j,k;
    unsigned char ret = 0xFF;//2.3.1
    unsigned char *KartNo;
    unsigned char *KartNoHex;

    GdkRGBA bg_color;
    GdkRGBA font_color;

    KartNo = (unsigned char *) malloc(4);
    KartNoHex = (unsigned char *) malloc(10);


    r=0;
    for(i=0; i<2; i++)
    {
        if(rec_TERM.READERS[(int)i].AKTIF)
        {
            memset(KartNo,0x00,4);
            memset(KartNoHex,0x00,10);

            for(k=0; k<3; k++)
            {
                ret = SMM_ReadCardNumber(i,KartNo,KartNoHex);
                if(ret == SMM_ERR_NONE)
                {
                    /*
                    //6.7.0
                    if(okuma_bekleme)
                    	delay(okuma_bekleme);

                    ret = SMM_ReadPersonel(i);
                    if(ret==SMM_ERR_NONE){
                    	getdate(&rec_Personel.OKUMA_TARIHI);
                    	gettime(&rec_Personel.OKUMA_SAATI);
                    	memcpy(rec_Personel.KARTNO,KartNo,4);
                    	memcpy(rec_Personel.KARTNO_HEX,KartNoHex,8);
                    	r=i+1;
                    	break;
                    }else{
                    	ret=SMM_ReadMaster(i);
                    	if(ret==SMM_ERR_NONE){
                    		CONFIG_Save();
                    		LCD_PrintCenter(6,"MASTER KEY OK !");
                    		MSJ_Ses("1221314151");
                    		MSJ_OK=0;
                    		z_MSJ_SURE_SN=0;
                    		delay(1000);
                    		break;
                    	}
                    }*/
                    get_date(&rec_Personel.OKUMA_TARIHI);
                    get_time(&rec_Personel.OKUMA_SAATI);
                    memcpy(rec_Personel.KARTNO,KartNo,4);
                    memcpy(rec_Personel.KARTNO_HEX,KartNoHex,8);
                    r=i+1;
                    break;
                }
                if (ret==SMM_ERR_CARDNONE) break;
            }
            //1.8.0
            //if ((ret==SMM_ERR_NONE)||(ret==SMM_ERR_CARDNONE)) break;
            if ((ret==SMM_ERR_NONE)) break;//||(ret==SMM_ERR_CARDNONE)) break;
        }
    }

    if (ret==SMM_ERR_CARDERR)
    {
        //LCD_Clear();
        lcd_clean();
        lcd_printf_ex(ALG_CENTER, 2,"OKUMA HATASI");//RESOURCE_ReadLine(400));// "KART OKUMA HATASI"
        lcd_printf_ex(ALG_CENTER, 3,"KARTI OKUYUCUYA" );//RESOURCE_ReadLine(400));// "KART OKUMA HATASI"
        lcd_printf_ex(ALG_CENTER, 4,"YAKIN TUTUNUZ" );//RESOURCE_ReadLine(400));// "KART OKUMA HATASI"
        lcd_printf_ex(ALG_CENTER, 5,"3 SN SONRA OKUR" );//RESOURCE_ReadLine(400));// "KART OKUMA HATASI"
        gtk_clean();
        gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);
        bg_color.alpha = 1;
        bg_color.blue = 0;
        bg_color.green = 0;
        bg_color.red = 1;
        font_color.alpha = 1;
        font_color.blue = .7;
        font_color.green = .01;
        font_color.red = 0.1;
        gtk_widget_override_color(label1, GTK_STATE_FLAG_NORMAL, &font_color);
        gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
        gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12
        gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_ERROR);//GTK_MESSAGE_ERROR
        gtk_printf(label1, "OKUMA HATASI\nKARTI OKUYUCUYA\nYAKIN TUTUNUZ\n3 SN SONRA OKUR");
        while(gtk_events_pending())
            gtk_main_iteration();
        for (j=0; j<2; j++)
            MSJ_Ses(rec_TERM.SES_OKUMA_NK);
        for (j=0; j<3; j++)
        {
            //WDT_Clear();
            //delay(900);
            napms(900);
            MSJ_Ses(rec_TERM.SES_OKUMA_NK);
        }
        //LCD_Clear();
        lcd_clean();
        MSJ_OK=0;
        z_MSJ_SURE_SN=0;
        gtk_clean();
        gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x, logo_y);

        gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_WARNING);//GTK_MESSAGE_ERROR
        while(gtk_events_pending())
            gtk_main_iteration();
        /*
        LCD_PrintCenter(6,RESOURCE_ReadLine(400));// "KART OKUMA HATASI"
        MSJ_Ses(rec_TERM.SES_OKUMA_NK);
        MSJ_OK=0;
        z_MSJ_SURE_SN=0;
        */
    }

    free(KartNo);
    free(KartNoHex);
    return r;
}
//char READER_Write(char *KartNo)//m8110 unsigned idi
char READER_Write(int okyc)//m8110 unsigned idi
{
    char ret;

    if (SMM_WritePersonel(okyc)==SMM_ERR_NONE)
        ret=1;
    else
        ret=0;

    return ret;
}

//6.0.0
char READER_Havuz_Write(int okyc)
{
    char ret;


    if (SMM_WriteHavuz(okyc)==SMM_ERR_NONE)
        ret=1;
    else
        ret=0;

    return ret;
}

void READER_Uninit(int com_port)
{
//	int i;
    /*	for (i=0;i<2;i++)
    		//5.4.1
    		//printer da oldugu icin her iki port da kapatiliyor.
    		//if (rec_TERM.READERS[i].AKTIF)	COM_init(i);
    		//if (rec_TERM.READERS[i].AKTIF)	COM_uninit(i);
    		//COM_uninit(i);

    		//5.4.3
    		//term tipi term_harcama ise ve com2 zaten kapatilmadiysa,
    		//burada kapat.
    		for (i=0;i<2;i++)
    		if (rec_TERM.READERS[i].AKTIF)
    			COM_uninit(i);

    		//6.1.0
    		//if(rec_TERM.TERM_TIP == TERM_HARCAMA)
    		if((rec_TERM.TERM_TIP == TERM_HARCAMA) || rec_YAZICI.AKTIF)
    			if (!rec_TERM.READERS[i].AKTIF)
    				COM_uninit(1);
    */

//odroid
//burada her iki portun da kapatılması gerek aslında.
//o yuzden yukarıdaki kısma dikkatli bak.
    if (ifd[com_port] >=0)
    {
        printf("COM[%d]: %d closed\r\n", com_port, ifd[com_port]);
        COM_uninit(ifd[com_port]);
        ifd [com_port]= -1;
    }

    //3.0.0
    //if(uvss_socket > 0)
    //    close(uvss_socket);
}


