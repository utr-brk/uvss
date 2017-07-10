/*
 * personel.c
 *
 *  Created on: 10 Haz 2014
 *      Author: Burak
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>

#include "tool.h"
#include "config.h"
#include "lcd.h"
#include "mesaj.h"
#include "log.h"
#include "tcp.h"

//0.0.1
//#include "watchdt.h"
#include "personel.h"
//#include "resource.h"
#include "reader.h"

#include "zamanis.h"
#include "sayac.h"
#include "reader.h"
#include "serport.h"

#include "smm.h"
#include "keyb.h"
#include "timer.h"

#include "string_convert.h"

//1.4.2
#include "yazici.h"

#include <wiringPi.h>

//2.0.0
#include "cn56b.h"


//2.1.0
#include "minini.h"
#define sizearray(a)	(sizeof(a) / sizeof((a)[0]))
#define ISKUR_INI "/home/odroid/Solidus/ISKUR.INI"

#define		printer_device_name		"/dev/printer0"
//#define		font_file_xx			"/usr/share/fonts/wqy-microhei.ttf"
#define		font_file_printer		"/usr/share/fonts/arial.ttf"

//1.4.2
/*
//#define 	DEVICE_NAME 		"/dev/ttyS3"
#define 	DEVICE_NAME 		"/dev/ttyS2"
*/
#define		ic_Izin_Tamam		0x01
#define 	ic_Izin_Yok			0x02
#define 	ic_KaraLise			0x03
#define 	ic_Tanimsiz			0x04
#define 	ic_KayipCalinti		0x04

#define		ic_HakBitmis		0x05
#define		ic_ParaBitmis		0x06
#define		ic_GrupYetkisiz		0x07
#define		ic_OgunDisinda		0x08

//6.9.0
#define     ic_RefakatBitmis 	0x09
#define     ic_PDKSGirmemis	 	0x0A
//6.3.2
#define		ic_UygunsuzGrup		0x09
#define		ic_UygunsuzCinsiyet 0x0A

//6.4.0
#define		ic_CikisYapmamis	0x0B
#define		ic_GirisYapmamis	0x0C

//7.0.0
#define		ic_RezYok			0x0D
//7.2.0
#define		ic_YemekOkunamadi	0x0E
//7.6.0
#define		ic_YemekhaneLimitiBitmis 	0x0F
//6.4.0
#define		SINIRSIZ			0x1
#define		TEK_KULLANIMLIK		0x2

#define		TopUp_NoFile		0x01
#define		TopUp_NoCard    	0x02
#define		TopUp_OK        	0x03
#define		TopUp_NOK       	0x04
#define		TopUp_Bakiye_NOK 	0x04
#define		TopUp_Bakiye_OK 	0x03
#define		TopUp_Grup_NOK   	0x04
#define		TopUp_Grup_OK    	0x03

#define		FN_KARA_LISTE		"/home/odroid/Solidus/KL.DAT"
#define		FN_KAYIP_CALINTI	"/home/odroid/Solidus/KC.DAT"
#define		FN_GRUP_GECIS		"/home/odroid/Solidus/PG.DAT"
#define		FN_OGUN				"/home/odroid/Solidus/OGUN.DAT"
#define		FN_GRUP_OGUN		"/home/odroid/Solidus/POGUN.DAT"

//brk
//Vakifbank icin karta bakiye yukleme amacli eklendi
//02/08/2011

//1.1.2
///home/odroid/Solidus/ path'i yoktu, eklendi
#define     FN_TOPUP		"/home/odroid/Solidus/BAKIYE.DAT"
//6.0.0
#define     FN_HAVUZ_TOPUP	"/home/odroid/Solidus/HAVUZ.DAT"

//1.4.2
/*
#define		NEAR_END		0x1
#define		PAPER_END		0x2
#define		PAPER_OK		0x0
*/
//1.0.0
#define cmd_LOG_AL		42
#define cAyrac_1		0x01
#define cAyrac_2		2
#define cAyrac_Son		13
#define cmd_OK  		10
//2.1.0
#define cmd_CANCEL  	20
#define cmd_MSFR_ONAY  	25
#define cmd_HAREKET		50

#define cmd_IZIN		30

//1.4.2
//struct_PERSONEL			rec_Personel;
//2.3.1
//extern struct_PERSONEL		rec_Personel;

struct_LOG_KART				rec_LogKart;
//0.0.5
struct_LOG_PARA				rec_LogPara;


struct_TOPUP				rec_TopUp;
struct_HATA					Hata;
struct_TOPUP_ERR			TopUP_ERR;

struct_HAVUZ_BILGI 			rec_HB; //6.0.0
struct_HAVUZ_TOPUP			rec_Havuz_TopUp;
struct_HAVUZ_HATA			Havuz_Hata;
struct_HAVUZ_TOPUP_ERR		Havuz_TopUP_ERR;
struct_SEANS_BILGI			rec_SEANS;

char	yCevap;
int		res_TopUp;
char	Hvz_TopUp;

//7.0.0
struct_KART_Config			rez_Kart;
//1.0.0
char Barcode_Fis[18];

//2.1.0
char TCKimlik[12];

//0.61
GdkRGBA font_color;
GdkRGBA bg_color;

char *menu[]=
{
    "PARA YUKLEME",
    "PARA DUSME",
    "CIKIS",
};

//2.4.0
PDKS_KART_Config KART;

char PERSONEL_KaraListe(char *KartNo)//m8110 unsigned idi
{
    unsigned long lng_KartNo;
    unsigned long lng_Read;
    FILE *fKara;
    //long i;
    long pos=0;
    long lastpos=0;
    long l,r;
    char ret=0;


    memcpy(&lng_KartNo,KartNo,4);

    fKara = fopen(FN_KARA_LISTE,"rb");
    if (fKara==NULL) return ret;
    fseek(fKara,0L,SEEK_END);

    r=(ftell(fKara)/4) + 1;
    l=1;

    while(1)
    {
        pos=(l+r)/2;
        if(lastpos==pos) break; //Yoksa
        fseek(fKara,(pos-1)*4,SEEK_SET);
        fread(&lng_Read,4,1,fKara);

        //printf("%d\n",pos);
        if (lng_Read==lng_KartNo) break; //BULDU
        if (lng_Read<lng_KartNo) //ILERLE
        {
            l=pos;
        }
        if (lng_Read>lng_KartNo) //GERILE
        {
            r=pos;
        }
        lastpos=pos;
    }

    if (lng_Read==lng_KartNo)
    {
        ret = 1;
    }
    fclose(fKara);

    return ret;

}

char PERSONEL_KayipCalinti(char *KartNo)//m8110 unsigned idi
{
    unsigned long lng_KartNo;
    unsigned long lng_Read;
    FILE *fKara;
    long pos=0;
    long lastpos=0;
    long l,r;
    char ret=0;


    memcpy(&lng_KartNo,KartNo,4);

    fKara = fopen(FN_KAYIP_CALINTI,"rb");
    if (fKara==NULL) return ret;
    fseek(fKara,0L,SEEK_END);

    r=(ftell(fKara)/4) + 1;
    l=1;

    while(1)
    {
        pos=(l+r)/2;
        if(lastpos==pos) break; //Yoksa
        fseek(fKara,(pos-1)*4,SEEK_SET);
        fread(&lng_Read,4,1,fKara);

        //printf("%d\n",pos);
        if (lng_Read==lng_KartNo) break; //BULDU
        if (lng_Read<lng_KartNo) //ILERLE
        {
            l=pos;
        }
        if (lng_Read>lng_KartNo) //GERILE
        {
            r=pos;
        }
        lastpos=pos;
    }

    if (lng_Read==lng_KartNo)
    {
        ret = 1;
    }
    fclose(fKara);

    return ret;

}

//brk
//Vakifbank icin eklendi
//02/08/2012

//int POS_PERSONEL_TopUp(char *KartNo)//m8110 unsigned idi
int PERSONEL_TopUp(char *KartNo)//m8110 unsigned idi
{

    uint32_t	lng_KartNo;
    uint32_t	lng_Read;
    uint32_t	lng_TopUp;
//unsigned long lng_ReadValue = 0;
    //unsigned long	lng_SiraNo = 0;
    uint16_t	KartSiraNo = 0;
    uint16_t	FileSiraNo = 0;
    uint16_t	KartGrup 	 = 0;
    uint16_t	FileGrup 	 = 0;

    FILE	*fTopUp;
    long	pos = 0;
    long	lastpos = 0;
    long	l,r;
    int		ret = TopUp_NoCard;
    unsigned char	OGV[16]; //m8110 unsigned char idi
    unsigned char 	GRP[4];//m8110 unsigned char idi

    //burasi karta islem yaparken daha once hata olduysa kaldigi yerden
    //devam etsin diye eklendi.
    //nerede hata olduysa orasini yapacak.

    if ((TopUP_ERR.HATA)&&(strcmp(TopUP_ERR.KARTNO,rec_Personel.KARTNO_HEX)==0)) //Son Yazilan Hatali Kartsa
    {
        switch(TopUP_ERR.HATA)
        {
        case 1: //OGV_Hatasi
            if(SMM_Login(Para_Sektor,0xBB,rec_TERM.KEY_TOPUP) == MFR_ERR_NONE)
            {
                if(SMM_WriteBlock(Para_Sektor, 1, TopUP_ERR.OGV) == MFR_ERR_NONE)
                {
                    if(SMM_Increment(Para_Sektor, 0, &TopUP_ERR.TOPUP) == MFR_ERR_NONE)
                    {
                        //if(mifare_one_transfer(ifd, Para_Sektor*4+0)== MFR_ERR_NONE){
                        memset(&TopUP_ERR,0,sizeof(TopUP_ERR)); //Hatayi Bosalt
                        ret = TopUp_OK;
                        //}else{
                        //	TopUP_ERR.HATA = 2; //bu asamada sadece incr edememis duruma geldi
                        //	return TopUp_NOK;
                        //}
                    }
                    else
                    {
                        TopUP_ERR.HATA = 2; //bu asamada sadece incr edememis duruma geldi
                        return TopUp_NOK;
                    }
                }
            }
            break;
        case 2: //TopUp_Incr_Hatasi
            if(rec_Personel.BAKIYE == TopUP_ERR.BAKIYE)//incr etmemis gozukuyor ama etmis
            {
                printf("incr edememis gozukuyor ama etmis\n");
                memset(&TopUP_ERR,0,sizeof(TopUP_ERR)); //Hatayi Bosalt
                ret = TopUp_OK;
                break;
            }
            else if(SMM_Login(Para_Sektor, 0xBB, rec_TERM.KEY_TOPUP) == MFR_ERR_NONE)
            {
                if(SMM_Increment(Para_Sektor, 0, &TopUP_ERR.TOPUP) == MFR_ERR_NONE)  //TopUp OK
                {
                    //if(mifare_one_transfer(ifd, Para_Sektor*4+0)== MFR_ERR_NONE){
                    memset(&TopUP_ERR,0,sizeof(TopUP_ERR)); //Hatayi Bosalt
                    ret = TopUp_OK;
                    //}else{
                    //	TopUP_ERR.HATA = 2; //hala daha incr edemedi
                    //	return TopUp_NOK;
                    //}
                }
                else
                {
                    TopUP_ERR.HATA = 2; //hala daha incr edemedi
                    return TopUp_NOK;
                }
            }
            break;
        case 3: //YB_Hatasi
            if(SMM_Login(YB_Sektor, 0xAA, rec_TERM.KEY_PERSONEL) != MFR_ERR_NONE)
                break;
            else if(SMM_WriteBlock(YB_Sektor, 0, TopUP_ERR.YB) != MFR_ERR_NONE)
                break;
            else
                TopUP_ERR.HATA = 4;//eksik kalan kismi yazmak icin asagidan devam et
        case 4: //TopUp_OGV_Grup_Hatasi
            if(SMM_Login(Para_Sektor, 0xBB, rec_TERM.KEY_TOPUP) == MFR_ERR_NONE)
            {
                if(SMM_ReadBlock(Para_Sektor, 1, OGV) == MFR_ERR_NONE)
                {
                    memcpy(OGV+2, &TopUP_ERR.FILEGRUP, 2);
                    if(SMM_WriteBlock(Para_Sektor, 1, OGV) == MFR_ERR_NONE)
                    {
                        memset(&TopUP_ERR,0,sizeof(TopUP_ERR)); //Hatayi Bosalt
                        ret = TopUp_OK;
                    }
                }
            }
            break;
        case 5: //TopUp_OGV_Grup_Yazma_Hatasi
            if(SMM_Login(Para_Sektor, 0xBB, rec_TERM.KEY_TOPUP) == MFR_ERR_NONE)
            {
                if(SMM_WriteBlock(Para_Sektor, 1, TopUP_ERR.OGV) == MFR_ERR_NONE)
                {
                    memset(&TopUP_ERR,0,sizeof(TopUP_ERR)); //Hatayi Bosalt
                    ret = TopUp_OK;
                }
            }
            break;
        }
    }
    else
    {
        memset(&TopUP_ERR,0,sizeof(TopUP_ERR)); //Hatayi Bosalt
    }

    memcpy(&lng_KartNo,KartNo,4);

    fTopUp = fopen(FN_TOPUP,"rb");
    if (fTopUp==NULL) return TopUp_NoFile;
    fseek(fTopUp,0L,SEEK_END);

    r=(ftell(fTopUp)/12) + 1;
    l=1;

    while(1)
    {
        pos=(l+r)/2;
        if(lastpos==pos) break; //Yoksa
        fseek(fTopUp,(pos-1)*12,SEEK_SET);
        fread(&rec_TopUp,12, 1, fTopUp);
        memcpy(&lng_Read, rec_TopUp.KARTNO, 4);
        lng_TopUp = rec_TopUp.TOPUP;

        if (lng_Read == lng_KartNo) break; //BULDU
        if (lng_Read < lng_KartNo) //ILERLE
        {
            l = pos;
        }
        if (lng_Read > lng_KartNo) //GERILE
        {
            r = pos;
        }
        lastpos = pos;
    }
    fclose(fTopUp);
    if (lng_Read != lng_KartNo)
    {
        //2.5.2
        //yukarı alındı
        //her halukarda kapanması lazım bu dosyanın cunku
        //fclose(fTopUp);
        return ret;
    }

    lng_TopUp	= rec_TopUp.TOPUP;
    FileSiraNo	= rec_TopUp.SIRANO;
    FileGrup	= rec_TopUp.GRUP;


    if(lng_TopUp > 0)
    {
        if(SMM_Login(Para_Sektor, 0xBB, rec_TERM.KEY_TOPUP) == MFR_ERR_NONE)
        {
            if(SMM_ReadBlock(Para_Sektor, 1, OGV) == MFR_ERR_NONE)
            {
                memcpy(&KartSiraNo, OGV, 2);
                if(FileSiraNo > KartSiraNo)
                {
                    memcpy(OGV, &FileSiraNo, 2);
                    if(SMM_WriteBlock(Para_Sektor, 1, OGV) == MFR_ERR_NONE)  // OGV yazıldı
                    {
                        if(SMM_Increment(Para_Sektor, 0, &lng_TopUp) == MFR_ERR_NONE)  //TopUp OK
                        {
                            //if(mifare_one_transfer(ifd, Para_Sektor*4+0)== MFR_ERR_NONE)
                            ret = TopUp_Bakiye_OK;
                            //else
                            //	TopUP_ERR.HATA = 2; //TopUp_Incr_Hatasi
                        }
                        else
                            TopUP_ERR.HATA = 2; //TopUp_Incr_Hatasi
                    }
                    else
                        TopUP_ERR.HATA = 1;//OGV_Hatasi

                }
            }
            else
                ret = TopUp_NOK;
        }
        else
            ret = TopUp_NOK;
    }
    if (ret == TopUp_NOK)//para sektoruyle is yapamadiysan devam etme
        return ret;
    if (TopUP_ERR.HATA)
    {
        memcpy(TopUP_ERR.KARTNO,rec_Personel.KARTNO_HEX, 9);
        memcpy(TopUP_ERR.OGV, OGV, 16);
        TopUP_ERR.TOPUP = lng_TopUp;
        TopUP_ERR.BAKIYE = rec_Personel.BAKIYE + lng_TopUp;
        return TopUp_NOK;
    }
    if(SMM_Login(YB_Sektor, 0xAA, rec_TERM.KEY_PERSONEL) == MFR_ERR_NONE)
    {
        if(SMM_ReadBlock(YB_Sektor, YB_Blok, OGV) == MFR_ERR_NONE)
        {
            //memcpy(GRP, OGV, 3);
            strncpy((char *)GRP, (const char *)OGV, 3);
            GRP[3] = 0;
            KartGrup = atoi((const char *)GRP);
            if(KartGrup != FileGrup)
            {
                OGV[0] = 48 + FileGrup / 100;
                OGV[1] = 48 + (FileGrup % 100) / 10;
                OGV[2] = 48 + (FileGrup % 100) % 10;
                if(SMM_WriteBlock(YB_Sektor, 0, OGV) == MFR_ERR_NONE)
                {
                    if(SMM_Login(Para_Sektor, 0XBB, rec_TERM.KEY_TOPUP) == MFR_ERR_NONE)
                    {
                        if(SMM_ReadBlock(Para_Sektor, 1, OGV) == MFR_ERR_NONE)
                        {
                            memcpy(OGV+2, &FileGrup, 2);
                            if(SMM_WriteBlock(Para_Sektor, 1, OGV) == MFR_ERR_NONE)
                                ret = TopUp_OK;
                            else
                            {
                                memcpy(TopUP_ERR.OGV, OGV, 16);
                                TopUP_ERR.HATA = 5; //TopUp_OGV_Grup_Yazma_Hatasi
                            }
                        }
                        else
                            TopUP_ERR.HATA = 4; //TopUp_OGV_Grup_Hatasi
                    }
                    else
                        TopUP_ERR.HATA = 4; //TopUp_OGV_Grup_Hatasi
                }
                else
                {
                    memcpy(TopUP_ERR.YB, OGV, 16);
                    TopUP_ERR.FILEGRUP = FileGrup;
                    TopUP_ERR.HATA = 3; //TopUp_YB_Hatasi
                }
            }
        }
        else
            ret = TopUp_Grup_NOK;
    }
    else
        ret = TopUp_Grup_NOK;
    //2.5.2
    //yukarıda kapatıldı
    //buraya gerek yok artık
    //fclose(fTopUp);
    if (ret == TopUp_NOK)//YB islerini yapamadiysan cik
        return ret;
    if (TopUP_ERR.HATA)
    {
        //buraya gelirken TopUP.ERR.HATA 0 gelebilir mi?
        memcpy(TopUP_ERR.KARTNO,rec_Personel.KARTNO_HEX, 9);
        memcpy(TopUP_ERR.OGV, OGV, 16);
        TopUP_ERR.TOPUP = lng_TopUp;
        TopUP_ERR.FILEGRUP = FileGrup;
        return TopUp_NOK;
    }
    return ret;
}

//6.0.0
//m8110
//char POS_HAVUZ_TopUp(char *KartNo)
char HAVUZ_TopUp(char *KartNo)
{

    uint32_t	lng_KartNo;
    uint32_t	lng_Read;
    uint32_t	lng_TopUp;
//unsigned long lng_ReadValue = 0;
    //unsigned long	lng_SiraNo 	= 0;
    uint16_t	KartSiraNo 	= 0;
    uint16_t	FileSiraNo 	= 0;
    uint16_t	KartGrup 		= 0;
    uint16_t	FileGrup 		= 0;

    FILE	*fTopUp;
    long	pos 		= 0;
    long	lastpos = 0;
    long	l,r;
    char	ret = TopUp_NoCard;
    char	OGV[16];//unsigned idi
    char	GRP[4];//unsigned idi

    //burasi karta islem yaparken daha once hata olduysa kaldigi yerden
    //devam etsin diye eklendi.
    //nerede hata olduysa orasini yapacak.

    if ((Havuz_TopUP_ERR.HATA)&&(strcmp(Havuz_TopUP_ERR.KARTNO,rec_Personel.KARTNO_HEX)==0)) //Son Yazilan Hatali Kartsa
    {
        switch(Havuz_TopUP_ERR.HATA)
        {
        case 1: //OGV_Hatasi
            if(SMM_Login(Para_Sektor,0xBB,rec_TERM.KEY_TOPUP) == MFR_ERR_NONE)
            {
                if(SMM_WriteBlock(Para_Sektor, 1,Havuz_TopUP_ERR.OGV) == MFR_ERR_NONE)
                {
                    if(SMM_Increment(Para_Sektor, 0, &Havuz_TopUP_ERR.TOPUP) == MFR_ERR_NONE)
                    {
                        //if(mifare_one_transfer(ifd, Para_Sektor*4+0)== MFR_ERR_NONE){
                        memset(&Havuz_TopUP_ERR,0,sizeof(Havuz_TopUP_ERR)); //Hatayi Bosalt
                        ret = TopUp_OK;
                        //}else{
                        //	Havuz_TopUP_ERR.HATA = 2; //bu asamada sadece incr edememis duruma geldi
                        //	return TopUp_NOK;
                        //}
                    }
                    else
                    {
                        Havuz_TopUP_ERR.HATA = 2; //bu asamada sadece incr edememis duruma geldi
                        return TopUp_NOK;
                    }
                }
            }
            break;
        case 2: //TopUp_Incr_Hatasi
            if(rec_Personel.BAKIYE == Havuz_TopUP_ERR.BAKIYE)//incr etmemis gozukuyor ama etmis
            {
                //printf("incr edememis gozukuyor ama etmis\n");
                memset(&Havuz_TopUP_ERR,0,sizeof(Havuz_TopUP_ERR)); //Hatayi Bosalt
                ret = TopUp_OK;
                break;
            }
            else if(SMM_Login(Para_Sektor, 0xBB, rec_TERM.KEY_TOPUP) == MFR_ERR_NONE)
            {
                if(SMM_Increment(Para_Sektor, 0, &Havuz_TopUP_ERR.TOPUP) == MFR_ERR_NONE)  //TopUp OK
                {
                    //if(mifare_one_transfer(ifd, Para_Sektor*4+0)== MFR_ERR_NONE){
                    memset(&Havuz_TopUP_ERR,0,sizeof(Havuz_TopUP_ERR)); //Hatayi Bosalt
                    ret = TopUp_OK;
                    //}else{
                    //	Havuz_TopUP_ERR.HATA = 2; //hala daha incr edemedi
                    //	return TopUp_NOK;
                    //}
                }
                else
                {
                    Havuz_TopUP_ERR.HATA = 2; //hala daha incr edemedi
                    return TopUp_NOK;
                }
            }
            break;
        case 3: //YB_Hatasi
            if(SMM_Login(YB_Sektor, 0xAA, rec_TERM.KEY_PERSONEL) != MFR_ERR_NONE)
                break;
            else if(SMM_WriteBlock(YB_Sektor, 0, Havuz_TopUP_ERR.YB) != MFR_ERR_NONE)
                break;
            else
                Havuz_TopUP_ERR.HATA = 4;//eksik kalan kismi yazmak icin asagidan devam et
        case 4: //TopUp_OGV_Grup_Hatasi
            if(SMM_Login(Para_Sektor, 0xBB, rec_TERM.KEY_TOPUP) == MFR_ERR_NONE)
            {
                if(SMM_ReadBlock(Para_Sektor, 1, (unsigned char *)OGV) == MFR_ERR_NONE)
                {
                    memcpy(OGV+2, &Havuz_TopUP_ERR.FILEGRUP, 2);
                    if(SMM_WriteBlock(Para_Sektor, 1, (unsigned char *)OGV) == MFR_ERR_NONE)
                    {
                        memset(&Havuz_TopUP_ERR,0,sizeof(Havuz_TopUP_ERR)); //Hatayi Bosalt
                        ret = TopUp_OK;
                    }
                }
            }
            break;
        case 5: //TopUp_OGV_Grup_Yazma_Hatasi
            if(SMM_Login(Para_Sektor, 0xBB, rec_TERM.KEY_TOPUP) == MFR_ERR_NONE)
            {
                if(SMM_WriteBlock(Para_Sektor, 1, Havuz_TopUP_ERR.OGV) == MFR_ERR_NONE)
                {
                    memset(&Havuz_TopUP_ERR,0,sizeof(Havuz_TopUP_ERR)); //Hatayi Bosalt
                    ret = TopUp_OK;
                }
            }
            break;
        }
    }
    else
    {
        memset(&Havuz_TopUP_ERR,0,sizeof(Havuz_TopUP_ERR)); //Hatayi Bosalt
    }

    memcpy(&lng_KartNo,KartNo,4);

    fTopUp = fopen(FN_HAVUZ_TOPUP,"rb");
    if (fTopUp==NULL) return TopUp_NoFile;
    fseek(fTopUp,0L,SEEK_END);

    r=(ftell(fTopUp)/12) + 1;
    l=1;

    while(1)
    {
        pos=(l+r)/2;
        if(lastpos==pos) break; //Yoksa
        fseek(fTopUp,(pos-1)*12,SEEK_SET);
        fread(&rec_Havuz_TopUp,12, 1, fTopUp);
        memcpy(&lng_Read, rec_Havuz_TopUp.KARTNO, 4);
        lng_TopUp = rec_Havuz_TopUp.TOPUP;

        //FileSiraNo = rec_TopUp.SIRANO;
        //FileGrup = rec_TopUp.GRUP;
        if (lng_Read == lng_KartNo) break; //BULDU
        if (lng_Read < lng_KartNo) //ILERLE
        {
            l = pos;
        }
        if (lng_Read > lng_KartNo) //GERILE
        {
            r = pos;
        }
        lastpos = pos;
    }

    fclose(fTopUp);
    if(lng_Read != lng_KartNo)
    {
        //2.5.2
        //yukarıda her halukarda kapatılması gerekiyordu
        //o yuzden yukarı tasındı
        //fclose(fTopUp);
        return ret;
    }
    lng_TopUp	= rec_Havuz_TopUp.TOPUP;
    FileSiraNo	= rec_Havuz_TopUp.SIRANO;
    FileGrup	= rec_Havuz_TopUp.GRUP;



    if(lng_TopUp > 0)
    {
        if(SMM_Login(Para_Sektor, 0xBB, rec_TERM.KEY_TOPUP) == MFR_ERR_NONE)
        {
            if(SMM_ReadBlock(Para_Sektor, 1, (unsigned char *)OGV) == MFR_ERR_NONE)
            {
                memcpy(&KartSiraNo, OGV, 2);
                if(FileSiraNo > KartSiraNo)
                {
                    memcpy(OGV, &FileSiraNo, 2);
                    if(SMM_WriteBlock(Para_Sektor, 1, (unsigned char *)OGV) == MFR_ERR_NONE)  // OGV yazıldı
                    {
                        if(SMM_Increment(Para_Sektor, 0, &lng_TopUp) == MFR_ERR_NONE)  //TopUp OK
                        {
                            //if(mifare_one_transfer(ifd, Para_Sektor*4+0)== MFR_ERR_NONE)
                            ret = TopUp_Bakiye_OK;
                            //else
                            //	Havuz_TopUP_ERR.HATA = 2; //TopUp_Incr_Hatasi
                        }
                        else
                            Havuz_TopUP_ERR.HATA = 2; //TopUp_Incr_Hatasi
                    }
                    else
                        Havuz_TopUP_ERR.HATA = 1;//OGV_Hatasi
                }
            }
            else
                ret = TopUp_NOK;
        }
        else
            ret = TopUp_NOK;
    }
    if(ret == TopUp_NOK)//para sektoruyle is yapamadiysan devam etme
        return ret;
    if(Havuz_TopUP_ERR.HATA)
    {
        memcpy(Havuz_TopUP_ERR.KARTNO,rec_Personel.KARTNO_HEX, 9);
        memcpy(Havuz_TopUP_ERR.OGV, OGV, 16);
        Havuz_TopUP_ERR.TOPUP = lng_TopUp;
        Havuz_TopUP_ERR.BAKIYE = rec_Personel.BAKIYE + lng_TopUp;
        return TopUp_NOK;
    }
    if(SMM_Login(YB_Sektor, 0xAA, rec_TERM.KEY_PERSONEL) == MFR_ERR_NONE)
    {
        if(SMM_ReadBlock(YB_Sektor, YB_Blok, (unsigned char *)OGV) == MFR_ERR_NONE)
        {
            //memcpy(GRP, OGV, 3);
            strncpy(GRP, OGV, 3);
            GRP[3] = 0;
            KartGrup = atoi(GRP);
            if(KartGrup != FileGrup)
            {
                OGV[0] = 48 + FileGrup / 100;
                OGV[1] = 48 + (FileGrup % 100) / 10;
                OGV[2] = 48 + (FileGrup % 100) % 10;
                if(SMM_WriteBlock(YB_Sektor, 0, (unsigned char *)OGV) == MFR_ERR_NONE)
                {
                    if(SMM_Login(Para_Sektor, 0xBB, rec_TERM.KEY_TOPUP) == MFR_ERR_NONE)
                    {
                        if(SMM_ReadBlock(Para_Sektor, 1, (unsigned char *)OGV) == MFR_ERR_NONE)
                        {
                            memcpy(OGV+2, &FileGrup, 2);
                            if(SMM_WriteBlock(Para_Sektor, 1, (unsigned char *)OGV) == MFR_ERR_NONE)
                                ret = TopUp_OK;
                            else
                            {
                                memcpy(Havuz_TopUP_ERR.OGV, OGV, 16);
                                Havuz_TopUP_ERR.HATA = 5; //TopUp_OGV_Grup_Yazma_Hatasi
                            }
                        }
                        else
                            Havuz_TopUP_ERR.HATA = 4; //TopUp_OGV_Grup_Hatasi
                    }
                    else
                        Havuz_TopUP_ERR.HATA = 4; //TopUp_OGV_Grup_Hatasi
                }
                else
                {
                    memcpy(Havuz_TopUP_ERR.YB, OGV, 16);
                    Havuz_TopUP_ERR.FILEGRUP = FileGrup;
                    Havuz_TopUP_ERR.HATA = 3; //TopUp_YB_Hatasi
                }
            }
        }
        else
            ret = TopUp_Grup_NOK;
    }
    else
        ret = TopUp_Grup_NOK;
    //2.5.2
    //yukarıda kapatıldı
    //buraya gerek yok
    //fclose(fTopUp);
    if (ret == TopUp_NOK)//YB islerini yapamadiysan cik
        return ret;
    if (Havuz_TopUP_ERR.HATA)
    {
        //buraya gelirken TopUP.ERR.HATA 0 gelebilir mi?
        memcpy(Havuz_TopUP_ERR.KARTNO,rec_Personel.KARTNO_HEX, 9);
        memcpy(Havuz_TopUP_ERR.OGV, OGV, 16);
        Havuz_TopUP_ERR.TOPUP = lng_TopUp;
        Havuz_TopUP_ERR.FILEGRUP = FileGrup;
        return TopUp_NOK;
    }
    return ret;
}

char PERSONEL_GrupGecis(char * GrupKodu)//m8110 unsigned idi
{
    char ret=0;
    struct_GRUPGECIS GC;
    FILE *f;
    unsigned long tNow,tStart,tEnd;
    unsigned char i;
    unsigned char hGun,GunDolu;

    f=fopen(FN_GRUP_GECIS,"rb");
    if (f==NULL) return ret;

    tNow = (rec_Personel.OKUMA_SAATI.ti_hour*256L) +
           rec_Personel.OKUMA_SAATI.ti_min;
    hGun=aDayOfWeek();
    while (!feof(f))
    {
        fread(&GC,sizeof(GC),1,f);
        if (strcmp(GC.GRUP_KODU,GrupKodu)==0) //Grup Tuttu Ise
        {
            if (GC.ZAMAN_KODU[0]==0x00) //Zaman Ba§msz Gecer
            {
                ret=1;
            }
            else if (GC.HAFTA_PLAN[hGun-1]=='1')   //Hafta Gunu Tuttu
            {
                GunDolu = 0;
                for (i=0; i<200; i++)
                {
                    if (GC.DETAY[i].GUN==hGun)
                    {
                        tStart = (GC.DETAY[i].SAAT_BASLA * 256L) + GC.DETAY[i].DAKIKA_BASLA;
                        tEnd = (GC.DETAY[i].SAAT_BITIR * 256L) + GC.DETAY[i].DAKIKA_BITIR;
                        if ((tStart+tEnd)==0L) break; //Sonra Kayit Yok
                        if ((tNow>=tStart)&&(tNow<=tEnd)) //Gecme Zamani Dogru
                        {
                            ret=1;
                            break;
                        }
                        GunDolu=1;
                    }
                    else
                    {
                        if ((GC.DETAY[i].GUN==0)||(GC.DETAY[i].GUN>hGun)) break;
                    }
                }
                if ((ret==0)&&(GunDolu==0)) ret=1; //GUNDE KISITLAMA YOKSA
            }
            break;
        }
    }
    fclose(f);
    return ret;
}

unsigned long PERSONEL_YemekOgun_EkranAc(void)
{
    FILE *f;
    struct_OGUN recOgun;
    unsigned long r=0L;
    unsigned long tNow,tStart,tEnd;

    struct tm * timeinfo;
    time_t rawtime;
    struct_time suan;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    suan.ti_hour = (unsigned char) timeinfo->tm_hour;
    suan.ti_min = (unsigned char) timeinfo->tm_min;


    f=fopen(FN_OGUN,"rb");
    if (f==NULL) return r;

    tNow = (suan.ti_hour*256L) + suan.ti_min;
    while (!feof(f))
    {
        fread(&recOgun,sizeof(recOgun),1,f);
        tStart = (recOgun.SAAT_BASLA * 256L) + recOgun.DAKIKA_BASLA;
        tEnd = (recOgun.SAAT_BITIR * 256L) + recOgun.DAKIKA_BITIR;
        if ((tNow>=tStart)&&(tNow<=tEnd)) //Gecme Zamani Dogru
        {
            r=recOgun.ID;
            break;
        }
    }
    fclose(f);
    //debug_print("ogun id: %d", recOgun.ID);
    return r;
}

unsigned long PERSONEL_YemekOgun(void)
{
    FILE *f;
    struct_OGUN recOgun;
    unsigned long r=0L;
    unsigned long tNow,tStart,tEnd;

    f=fopen(FN_OGUN,"rb");
    if (f==NULL) return r;

    tNow = (rec_Personel.OKUMA_SAATI.ti_hour*256L) + rec_Personel.OKUMA_SAATI.ti_min;
    while (!feof(f))
    {
        fread(&recOgun,sizeof(recOgun),1,f);
        tStart = (recOgun.SAAT_BASLA * 256L) + recOgun.DAKIKA_BASLA;
        tEnd = (recOgun.SAAT_BITIR * 256L) + recOgun.DAKIKA_BITIR;
        if ((tNow>=tStart)&&(tNow<=tEnd)) //Gecme Zamani Dogru
        {
            r=recOgun.ID;
            break;
        }
    }
    fclose(f);
    //debug_print("ogun id: %d", recOgun.ID);
    return r;
}

//6.0.0
unsigned long PERSONEL_HavuzSeans(void)
{
    unsigned long r=0L;
    int i;

    //burada once kartin gecerlilik tarihine bakilacak
    //sinirsiz kart mi diye bakilacak,
    //sinirliysa karttan okunan rec_Personel.GRUP terminalin o anki yetkili
    //gruplarinda var mi diye bakilmali, varsa dusulecek para olarak o fiyata
    //bakilacak. ama bir de sayet ini dosyasinda fiyat bilgisi varsa o dikkate
    //alinmali.
    i=0;
    r = rec_Seans.GRUP[i];
    while(r>0)
    {
        if(r == (int) atoi(rec_Personel.GRUP))
        {
            //6.4.0
            if(rec_HB.SNRSZ == TEK_KULLANIMLIK)//tek kullanim ise 2. fiyat
                rec_Personel.HARCANAN = (unsigned long)rec_Seans.GRUP_FIYAT[i][1];
            else
                rec_Personel.HARCANAN = (unsigned long)rec_Seans.GRUP_FIYAT[i][0];

            break;
        }
        i++;
    }
    return r;
}

unsigned char PERSONEL_YemekHak(unsigned long Ogun_ID,struct_YEMEKHAK *yHak)
{
    FILE *f;
    struct_GRUPOGUN recGOgun;
    unsigned char r;
    //7.4.0
    char Ogun_Test;

    r=0;
    memset(yHak,0,sizeof(struct_YEMEKHAK)); //m8110
    //debug_print("sizeof(struct_YEMEKHAK): %d", sizeof(struct_YEMEKHAK));
    //memset(yHak,0,5);
    f=fopen(FN_GRUP_OGUN,"rb");
    if (f==NULL)
    {
        return r;
    }
    while (!feof(f))
    {
        fread(&recGOgun,sizeof(recGOgun),1,f);//m8110
        //fread(&recGOgun,21,1,f);
        //debug_print("recOgun.GrupKodu: %s, recGOgun.OGUN_ID: %d, recGOgun.HAK: %d, recGOgun.FIYAT[0]: %d, recGOgun.FIYAT[1]: %d, recGOgun.FIYAT[2]: %d", recGOgun.GRUP_KODU, recGOgun.OGUN_ID, recGOgun.HAK, recGOgun.FIYAT[0], recGOgun.FIYAT[1], recGOgun.FIYAT[2]);
        if ((strcmp(recGOgun.GRUP_KODU,rec_Personel.GRUP)==0)&&
                (recGOgun.OGUN_ID==Ogun_ID)&&
                (recGOgun.HAK>0))
        {
            r=1;
            break;
        }
    }
    fclose(f);

    if (r==1)
    {
        //debug_print("yemek hak");
        if (rec_Personel.YB.OGUN_ID>0) //Yemek Yemis
        {
            //7.4.0
            //if ((rec_Personel.YB.OGUN_ID==Ogun_ID)&&
            Ogun_Test = 0;
            if(rec_Ayarlar.Grup_ID)
            {
                if(rec_Personel.YB.OGUN_ID==Ogun_ID)
                    Ogun_Test = 1;
            }
            else
                Ogun_Test = 1;

            if ((Ogun_Test)&&
                    ((rec_Personel.YB.TARIH.da_year==rec_Personel.OKUMA_TARIHI.da_year)&&
                     (rec_Personel.YB.TARIH.da_mon==rec_Personel.OKUMA_TARIHI.da_mon)&&
                     (rec_Personel.YB.TARIH.da_day==rec_Personel.OKUMA_TARIHI.da_day))) //Bu Ogunden Bugun Yemis
            {
                if (rec_Personel.YB.SAYI<recGOgun.HAK) //Bu Ogunde Yedigi Yemek Hakkından Az Ise
                {
                    yHak->HAK = rec_Personel.YB.SAYI + 1;
                }
                else    //odroid unutma duzelt
                {
                    //2.0.2
                    //if (aFileexist("/home/odroid/Solidus/SNRZ.YMK")==0x01)
                    if (access("/home/odroid/Solidus/SNRZ.YMK", 0) ==0 )//sınırsız yemek dosyası varsa
                    {
                        yHak->HAK = recGOgun.HAK; //Sonrakiler son fiyatdan
                    }
                    else
                    {
                        yHak->HAK = 0;
                    }
                }
                //else {
                //	yHak->HAK = 0;
                //}

            }
            else
            {
                yHak->HAK 	= 1;
            }
        }
        else
        {
            yHak->HAK 	= 1;
        }
        if (yHak->HAK > 0)
            yHak->FIYAT = recGOgun.FIYAT[yHak->HAK-1];
    }
    else
    {
        yCevap = ic_GrupYetkisiz;
    }
    //debug_print("yHak->FIYAT: %d", yHak->FIYAT);
    return r;
}

//6.9.0
/*
		*****return values******
 1: refakatci grubunda degil
 0: grupta ama sure dolmamis
-1: grupta ama sure dolmus
*/
int Refakatci_Kontrol(void)
{
//char *ptr;
    struct_date bugun;
    int i, per_grp;
    int found = 0;

    per_grp = atoi(rec_Personel.GRUP);

    for(i=0; i<256; i++)
    {
        if(refakatci[i] == 0)
            break;
        if(per_grp == refakatci[i])
        {
            found = 1;
            break;
        }
    }

    if(!found)
        return 1; //refakatci grubunda degil
//	ptr = strtok(refakatci, rec_Personel.GRUP);
//	if(ptr == NULL)
//		return 1;//refakatci grubunda degil

    //refakatci grubunda skt kontrol et

    get_date(&bugun);

    if(rec_Personel.REFAKAT_SKT.da_year < bugun.da_year)
        return -1;//grupta ama sure dolmus

    if(rec_Personel.REFAKAT_SKT.da_year > bugun.da_year)
        return 0;//grupta ama sure dolmamis

    if(rec_Personel.REFAKAT_SKT.da_mon < bugun.da_mon)
        return -1;//grupta ama sure dolmus

    if(rec_Personel.REFAKAT_SKT.da_mon > bugun.da_mon)
        return 0;//grupta ama sure dolmamis

    if(rec_Personel.REFAKAT_SKT.da_day < bugun.da_day)
        return -1;//grupta ama sure dolmus
    else
        return 0;//grupta ama sure dolmamis

}

//6.9.0
/*
		*****return values******
 1: kontrolsuz grupta ya da kontrollu grupta ama pdks'den gecmis
 0: kontrollu grupta ve pdks'den gecmemis
*/
int PDKS_Kontrol(void)
{
//char *ptr;
    struct_date bugun;

    int i, per_grp;
    int found = 0;

    per_grp = atoi(rec_Personel.GRUP);

    for(i=0; i<512; i++)
    {
        if(kontrolsuz[i] == 0)
            break;
        if(per_grp == kontrolsuz[i])
        {
            found = 1;
            break;
        }
    }

    if(found)
        return 1; //kontrolsuz grup, cik git

//	ptr = strtok(kontrolsuz, rec_Personel.GRUP);
//	if(ptr != NULL)
//		return 1;// kontrolsuz grupta

    //kontrollu grupta, pdks okutma tarihini kontrol et
    get_date(&bugun);

    if(rec_Personel.PDKS_TARIHI.da_year != bugun.da_year)
        return 0;

    if(rec_Personel.PDKS_TARIHI.da_mon != bugun.da_mon)
        return 0;

    if(rec_Personel.PDKS_TARIHI.da_day != bugun.da_day)
        return 0;

    return 1;//kontrollu grupta ama bugun pdks'den gecmis
}

//7.0.0
char Rezervasyon_KartBul (char * File_Rez)
{
    char ret=0x00;
    FILE * inp; //,*yemek_file;
// char ymk_file[13];
    struct_KART_Config Rez_Kart;
// long i;
    long flen;
    long rlen;
    long pos=0;
    long lastpos=0;
    long l,r;
    int k=1;
    size_t blen;
// char ymk_tarihi[9];
//7.2.0
    char *Data;

    blen=strlen(rec_Personel.KARTNO_HEX);

    if ((inp=fopen(File_Rez,"rt"))==NULL)
        return ret; //rezervasyon dosyasi yok

    rlen=sizeof(KART_Config);
    fseek(inp,0L,SEEK_END);
    flen = ftell(inp)/rlen;

    if (flen==0)
    {
        fclose(inp);
        return ret;
    }

    l=1;
    r=flen+1;

//7.2.0
    Data = (char *) calloc(16, sizeof (char));

    memset(&Rez_Kart, 0, sizeof(KART_Config));
    while(1)
    {
        pos=(l+r)/2;
        if(lastpos==pos) break; //Yoksa
        fseek(inp,(pos-1)*rlen,SEEK_SET);
        fread(&Rez_Kart,rlen,1,inp);
        k=strncmp(Rez_Kart.KART_NO,rec_Personel.KARTNO_HEX,blen);

        //printf("%d\n",pos);

        if (k==0)
        {
            //burada o kartla bugun yemek yendi mi diye bakiliyor
            //kartno.ymk dosyasinda bugunun tarihi varsa yedirme (rez var, hak yok)
            //bugunden baska tarih varsa, dosyanin icine bugunu yaz, yedir
            //kartno.ymk yoksa yap, icine bugunun tarihini yaz
            //sprintf(ymk_file, "%02d%02d%04d.ymk",
            //						rec_Personel.OKUMA_TARIHI.da_day,
            //						rec_Personel.OKUMA_TARIHI.da_mon,
            //						rec_Personel.OKUMA_TARIHI.da_year);
            //7.2.0
            //artik yemek dosyasi yok
            //sprintf(ymk_file, "%s.ymk",rec_Personel.KARTNO_HEX);
            memcpy(rec_Personel.ADI, Rez_Kart.ADI,16);
            memcpy(rec_Personel.SOYAD, Rez_Kart.SOYADI,16);
            //7.2.0
            //artik yemek yiyip yemedigi dosyadan degil
            //14. sektorun 0. blogundan kontrol ediliyor.
            /*
            if(access(ymk_file, 0) == 0){//yemek dosyasi var, bugun mu yemis bak
            	yemek_file=fopen(ymk_file,"r+t");
            	fscanf(yemek_file, "%s", &ymk_tarihi);
            	switch(memcmp(File_Rez, ymk_tarihi, 8)){//bugun yemek yemis mi?
            		case 0: ret = 0x2; //bugun yemek yemis
            			break;
            		default: ret = 0x1;//bugun yemek yememis, yedir, tarihi kaydet
            			fseek(yemek_file,0L,SEEK_SET);
            			fprintf(yemek_file, "%02d%02d%04d",
            														rec_Personel.OKUMA_TARIHI.da_day,
            														rec_Personel.OKUMA_TARIHI.da_mon,
            														rec_Personel.OKUMA_TARIHI.da_year);
            			break;
            	}
            }else
            {
            	yemek_file=fopen(ymk_file,"wt");
            	fprintf(yemek_file, "%02d%02d%04d",
            												rec_Personel.OKUMA_TARIHI.da_day,
            												rec_Personel.OKUMA_TARIHI.da_mon,
            												rec_Personel.OKUMA_TARIHI.da_year);
            	ret = 0x1;//rezervasyonu var ama yemek yememis, yemek yer
            }
            */
            //7.2.0
            //14|0 sektor|blokdan yemek kontrolu
            if(SMM_Login(14, 0xBB, (unsigned char *)"\xB0\xB1\xB2\xB3\xB4\xB5") == SMM_ERR_NONE)
            {
                if(SMM_ReadBlock(14, 0, (unsigned char *)Data) == SMM_ERR_NONE)
                {
                    //memcpy(&ymk_tarihi, Data, 8); ne gerek var???
                    switch(memcmp(File_Rez, Data, 8)) //bugun yemek yemis mi?
                    {
                    case 0:
                        ret = 0x2; //bugun yemek yemis
                        break;
                    default:
                        ret = 0x1;//bugun yemek yememis, yedir, tarihi kaydet
                        sprintf(Data, "%02d%02d%04d",
                                rec_Personel.OKUMA_TARIHI.da_day,
                                rec_Personel.OKUMA_TARIHI.da_mon,
                                rec_Personel.OKUMA_TARIHI.da_year);
                        SMM_WriteBlock(14,0, (unsigned char *)Data);
                        break;
                    }
                }
                else
                {
                    ret = 0xFF;//yemek datasi okunamadi
                }
            }
            else
            {
                ret = 0xFF;//yemek datasi okunamadi
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
// fclose(yemek_file);
    if (k)
    {
        ret=0x3;//rezervasyonu yok
    }

//7.2.0
    free(Data);

    return ret;
}


char PERSONEL_YemekYermi(void)
{
    unsigned long Ogun_ID;

    struct_YEMEKHAK rYH;
    int i =-2;//6.9.0

    //7.6.0
    if(rec_LIMIT.AKTIF)
    {
        //7.6.1
        //if((rec_LIMIT.TURNIKE >0) && (SAYAC.SAYI > rec_LIMIT.TURNIKE)){
        if((rec_LIMIT.TURNIKE >0) && (SAYAC.SAYI >= rec_LIMIT.TURNIKE))
        {
            yCevap = ic_YemekhaneLimitiBitmis;
            return ic_Izin_Yok;
        }
    }
    //6.9.0
    //7.6.2
    //if(access("KONTROL.INI", 0) == 0){//dosya var kontrollu calisilacak
    if(rec_Ayarlar.Refakatci_Kontrol) //dosya var kontrollu calisilacak
    {
        //Refakatci ve fakat refakat suresi dolmus mu
        i = Refakatci_Kontrol();
        if(i == -1) //refakatci ama suresi dolmus
        {
            yCevap = ic_RefakatBitmis;
            return ic_Izin_Yok;
        }
    }
    else  //refakatci kontrolu yok
    {
        i=1;
    }
    if((i==1) && (rec_Ayarlar.Giris_Kontrol)) //refakatci degil, o zaman kontrolsuz mu diye bak
    {
        //kontrollu grupta ama pdks'den gecis yapmamis mi
        if(!PDKS_Kontrol())
        {
            yCevap = ic_PDKSGirmemis;
            return ic_Izin_Yok;
        }
    }

    Ogun_ID=PERSONEL_YemekOgun();
    if (Ogun_ID>0)
    {
        if (PERSONEL_YemekHak(Ogun_ID,&rYH)==1)
        {
            if (rYH.HAK>0) //Yemek Yiyecek
            {
                //debug_print("bakiye: %d, yemek fiyat: %d", rec_Personel.BAKIYE, rYH.FIYAT);
                //0.50
                //1.0.2
                //unutmusum:)
                //odroid silmeyi unutma
                //rYH.FIYAT = 1;
                if (rec_Personel.BAKIYE>=rYH.FIYAT) //Bakiyesi Yeterli
                {
                    rec_Personel.YB.OGUN_ID=Ogun_ID;  //Ogun Bilgisi
                    rec_Personel.YB.SAYI=rYH.HAK;     //Yemek Sayisi
                    //m8210
                    get_date(&rec_Personel.YB.TARIH);  //Tarih
                    rec_Personel.HARCANAN =rYH.FIYAT; //Harcanan
                    return ic_Izin_Tamam;
                }
                else
                {
                    yCevap = ic_ParaBitmis;
                }
            }
            else
            {
                yCevap = ic_HakBitmis;
            }
        }
    }
    else
    {
        yCevap = ic_OgunDisinda;
    }
    return ic_Izin_Yok;
}

char REZERVASYON_YemekYermi(void)
{
    //7.1.0
    unsigned long Ogun_ID;

    //struct_YEMEKHAK rYH;
    //int i;//6.9.0

    //7.0.0
    char ret;
    char rez_file[13];//gunluk rezervasyon dosyasi
//	char ymk_file[13];//ayni dosyann yemek yenip yenmedigini tutan versiyonu
//	struct_date d;

//	getdate(&d);
//	sprintf(rez_file, "%02d%02d%04d.rez", d.da_day, d.da_mon, d.da_year);
//	sprintf(ymk_file, "%02d%02d%04d.ymk", d.da_day, d.da_mon, d.da_year);


    //7.6.0
    if(rec_LIMIT.AKTIF)
    {
        //7.6.1
        //if(SAYAC.SAYI >rec_LIMIT.TURNIKE){
        if((rec_LIMIT.TURNIKE >0) && (SAYAC.SAYI >=rec_LIMIT.TURNIKE))
        {
            yCevap = ic_YemekhaneLimitiBitmis;
            return ic_Izin_Yok;
        }
    }

    //7.1.0
    //yemek saati degilse izin verme
    Ogun_ID=PERSONEL_YemekOgun();
    if (Ogun_ID == 0)
    {
        yCevap = ic_OgunDisinda;
        return ic_Izin_Yok;
    }


    sprintf(rez_file, "%02d%02d%04d.rez", rec_Personel.OKUMA_TARIHI.da_day,
            rec_Personel.OKUMA_TARIHI.da_mon,
            rec_Personel.OKUMA_TARIHI.da_year);

    if(access(rez_file, 0) == 0) //rezervasyon dosyasi var
    {
        //o gun rezervasyonu var mi diye kontrol et
        switch(Rezervasyon_KartBul(rez_file))//o gun icin rezervasyonu var mi
        {
        case 1:
            ret = ic_Izin_Tamam;//yemek yer
            break;
        case 2:
            ret = ic_Izin_Yok;
            yCevap = ic_HakBitmis;//yemek yemis, artik yiyemez
            break;
        case 3:
            ret = ic_Izin_Yok;
            yCevap= ic_RezYok;//rezervasyonu yok, yiyemez
            break;
        default:
            ret = ic_Izin_Yok;
            yCevap= ic_YemekOkunamadi;//rezervasyonu yok, yiyemez
            break;
        }
        if(ret)//ret = 0 ==> rezervasyon dosyasi yok, normal devam et
            return ret;
    }
    return 0;//burada aslinda dosyayi bulamadi.
    //o yuzden uyari vermeli
    /*
    	//6.9.0
    	if(access("KONTROL.INI", 0) == 0){//dosya var kontrollu calisilacak
    		//Refakatci ve fakat refakat suresi dolmus mu
    		i = Refakatci_Kontrol();
    		if(i == -1){//refakatci ama suresi dolmus
    			yCevap = ic_RefakatBitmis;
    			return ic_Izin_Yok;
    		}else{
    			if(i==1){//refakatci degil, o zaman kontrolsuz mu diye bak
    				//kontrollu grupta ama pdks'den gecis yapmamis mi
    				if(!PDKS_Kontrol()){
    					yCevap = ic_PDKSGirmemis;
    					return ic_Izin_Yok;
    				}
    			}
    		}
    	}


    	Ogun_ID=PERSONEL_YemekOgun();
    	if (Ogun_ID>0)
    	{
    		if (PERSONEL_YemekHak(Ogun_ID,&rYH)==1)
    		{
    			if (rYH.HAK>0) //Yemek Yiyecek
    						{
    				if (rec_Personel.BAKIYE>=rYH.FIYAT) //Bakiyesi Yeterli
    				{
    					rec_Personel.YB.OGUN_ID=Ogun_ID;  //Ogun Bilgisi
    					rec_Personel.YB.SAYI=rYH.HAK;     //Yemek Sayisi
    					getdate(&rec_Personel.YB.TARIH);  //Tarih
    					rec_Personel.HARCANAN =rYH.FIYAT;  //Harcanan
    					return ic_Izin_Tamam;
    				} else {
    					yCevap = ic_ParaBitmis;
    				}
    			} else {
    				yCevap = ic_HakBitmis;
    			}
    		}
    	} else {
    		yCevap = ic_OgunDisinda;
    	}
    	return ic_Izin_Yok;
    */


}

//6.0.0
char PERSONEL_HavuzaGirermi(void)
{
    //unsigned long Ogun_ID;
    int i,r, found;
    char start[9], end[9], now[9]; //, qq[2];
    struct_date dd;

    //struct_YEMEKHAK rYH;

    //burada once kartin gecerlilik tarihine bakilacak
    //grup kontrolu yap
    //cinsiyet kontrolu yap
    //sinirsiz kart mi diye bak
    //sinirsizsa fiyat 0.0 tl, sinirliysa dahil olunan grubun fiyatini al
    //ancak seansin kendi fiyati varsa o fiyat gecerli olacak

    //sinirsiz kartsa para dusme
    if(rec_HB.SNRSZ)
        rec_Personel.HARCANAN = 0;

    //giris yapiliyorsa
    if(rec_HAVUZ.GIRIS)
    {
        //islem yapilan seans id'yi kaydet
        rec_HB.SEANS_ID = rec_Seans.ID;

        //6.4.0
        if(rec_HB.IN_OUT)
        {
            yCevap = ic_CikisYapmamis; //ic_OgunDisinda;
            return ic_Izin_Yok;
        }
        //uyelik sure kontrolu
        //m8110
        get_date(&dd);
        sprintf(now, "%04d%02d%02d",dd.da_year, dd.da_mon, dd.da_day);
        sprintf(start, "%04d%02d%02d",rec_HB.BASLANGIC.da_year, rec_HB.BASLANGIC.da_mon, rec_HB.BASLANGIC.da_day);
        sprintf(end, "%04d%02d%02d",rec_HB.BITIS.da_year, rec_HB.BITIS.da_mon, rec_HB.BITIS.da_day);

        if((atol(now) > atol(end)) || (atol(now) < atol(start)))
        {
            yCevap = ic_HakBitmis; //ic_OgunDisinda;
            return ic_Izin_Yok;
        }
        //6.4.0
        memcpy(&rec_HB.GIRIS_TARIHI,&dd, 4);

        //grup uygun mu kontrolu
        i=0;
        found = 0;
        r = rec_Seans.GRUP[i];
        while(r>0)
        {
            if(r == (int) atoi(rec_Personel.GRUP))
            {
                //6.3.2
                if(rec_HB.SNRSZ != SINIRSIZ)  //sinirsizsa para dusme
                {
                    if(rec_Seans.FIYAT <0)  //seansin sabit fiyati yoksa
                    {
                        if(rec_HB.SNRSZ == TEK_KULLANIMLIK)//6.4.0
                            rec_Personel.HARCANAN = (unsigned long)rec_Seans.GRUP_FIYAT[i][1];
                        else
                            rec_Personel.HARCANAN = (unsigned long)rec_Seans.GRUP_FIYAT[i][0];
                    }
                    else
                        rec_Personel.HARCANAN = rec_Seans.FIYAT;
                }
                found = 1;
                break;
            }
            r = rec_Seans.GRUP[++i];
        }

        if(!found) //seans personel'in grubunu icermiyorsa
        {
            //6.3.2
            //yCevap = ic_OgunDisinda;
            yCevap = ic_UygunsuzGrup;
            return ic_Izin_Yok;
        }

        //cinsiyet uygun mu kontrolu
        if(strchr(rec_Seans.CINSIYET, rec_HB.CINSIYET) == NULL)
        {
            //6.3.2
            //yCevap = ic_OgunDisinda;// buraya baska bir hata bulunmali
            yCevap = ic_UygunsuzCinsiyet;
            return ic_Izin_Yok;
        }
        //6.3.2
        //parasi eksiye dustuyse izin verme
        if(rec_Personel.BAKIYE < 20000)
        {
            yCevap = ic_ParaBitmis;
            return ic_Izin_Yok;
        }
        //bakiyesi yeterli mi
        if((rec_Personel.BAKIYE -20000)< rec_Personel.HARCANAN * rec_HAVUZ.CARPANI)
        {

            yCevap = ic_ParaBitmis;
            return ic_Izin_Yok;
        }
    }
    else  //cikis turnikesiyse
    {
        //6.4.0
        if(!rec_HB.IN_OUT)
        {
            yCevap = ic_GirisYapmamis;
            return ic_Izin_Yok;
        }
    }

    return ic_Izin_Tamam;
    /*
    //	Ogun_ID=PERSONEL_HavuzSeans();
    	if (Ogun_ID>0)
    	{
    		if (PERSONEL_YemekHak(Ogun_ID,&rYH)==1)
    		{
    			if (rYH.HAK>0) //Yemek Yiyecek
    			{
    				if (rec_Personel.BAKIYE>=rYH.FIYAT) //Bakiyesi Yeterli
    				{
    					rec_Personel.YB.OGUN_ID=Ogun_ID;  //Ogun Bilgisi
    					rec_Personel.YB.SAYI=rYH.HAK;     //Yemek Sayisi
    					getdate(&rec_Personel.YB.TARIH);  //Tarih
    					rec_Personel.HARCANAN =rYH.FIYAT; //Harcanan
    					return ic_Izin_Tamam;
    				} else {
    					yCevap = ic_ParaBitmis;
    				}
    			} else {
    				yCevap = ic_HakBitmis;
    			}
    		}
    	} else {
    		yCevap = ic_OgunDisinda;
    	}
    	return ic_Izin_Yok;
    */
}

char PERSONEL_KapidanGecermi(void)
{
    char ret=0;
    unsigned char *tcp_Resp;
    struct_IZINCEVAP tcp_Cevap;

    tcp_Resp = (unsigned char *) calloc(sizeof(tcp_Cevap)+1, 1);//0.0.6 malloc'du calloc oldu
    //memset(tcp_Resp,0,sizeof(tcp_Cevap)+1); //calloc olunca gerek kalmadı


//	if (net_ONLINE)
//	{
    //m8110
    if (TCP_PersonelIzinAl(&rec_Personel,tcp_Resp)==1)
    {
        memcpy(&tcp_Cevap,tcp_Resp,sizeof(tcp_Cevap));
        ret=tcp_Cevap.CEVAP+1;
        rec_LogKart.CEVAP_ONLINE = 1;
    }
//	}

    if (rec_LogKart.CEVAP_ONLINE!=1) //Offline
    {
        rec_LogKart.CEVAP_ONLINE = 0;
        if (PERSONEL_KaraListe(rec_Personel.KARTNO)==1)
        {
            ret=ic_Tanimsiz;
        }
        else
        {
            if (PERSONEL_GrupGecis(rec_Personel.GRUP)==1)
            {
                ret=ic_Izin_Tamam;
            }
            else
            {
                ret=ic_Izin_Yok;
            }
        }
    }

    free(tcp_Resp);
    return ret;
}

char PERSONEL_HareketIsle(void)
{
    char ret=0;
    unsigned char *tcp_Resp;
    struct_IZINCEVAP tcp_Cevap;
    tcp_Resp = (unsigned char *) calloc(sizeof(tcp_Cevap)+1,1);//0.0.6 malloc'du calloc oldu
    //memset(tcp_Resp,0,sizeof(tcp_Cevap)+1); //calloc olunca gerek kalmadı
    //m8110
    if (TCP_PersonelHareketIsle(&rec_Personel,tcp_Resp)==1)
    {
        rec_LogKart.GECTI_ONLINE=1;
        ret=1;
    }
    free(tcp_Resp);
    return ret;
}

void PERSONEL_Log(char Okuyucu)
{
    memcpy(rec_LogKart.KART_NO,rec_Personel.KARTNO,4);
    memcpy(&rec_LogKart.OKUMA_TARIH,&rec_Personel.OKUMA_TARIHI,sizeof(struct_date));
    memcpy(&rec_LogKart.OKUMA_ZAMAN,&rec_Personel.OKUMA_SAATI,sizeof(struct_date));
    rec_LogKart.TERM_TIP = rec_TERM.TERM_TIP;
    rec_LogKart.KART_BAKIYE = rec_Personel.BAKIYE;
    rec_LogKart.KART_HARCANAN = rec_Personel.HARCANAN;
    rec_LogKart.KART_OGV = rec_Personel.OGV;
    rec_LogKart.OKUYUCU=Okuyucu;
    LOG_Add_KART(&rec_LogKart);
    memset(&rec_LogKart,0,sizeof(rec_LogKart));
}

void PERSONEL_ParaLog(struct_date Trh, struct_time Zmn)
{
    //getdate(&rec_LogPara.OKUMA_TARIH);
    //gettime(&rec_LogPara.OKUMA_ZAMAN);
    rec_LogPara.OKUMA_TARIH = Trh;
    rec_LogPara.OKUMA_ZAMAN = Zmn;
    LOG_Add_PARA(&rec_LogPara);
}

char PERSONEL_ParaIsle(void)
{
    char r=0;
    unsigned char *tcp_Resp;
    struct_IZINCEVAP tcp_Cevap;
    struct_TCP_Para recPara;
    time_t rawtime;
//	struct tm * timeinfo;
    time(&rawtime);
//	timeinfo = localtime(&rawtime);

    tcp_Resp = (unsigned char *) calloc(sizeof(tcp_Cevap)+1, 1);//0.0.6 malloc'du calloc oldu
    //memset(tcp_Resp,0,sizeof(tcp_Cevap)+1); //calloc olunca gerek kalmadı

    //debug_print("rec_LogPara.KART_NO: %s", rec_LogPara.KART_NO);
    memcpy(recPara.KARTNO,rec_LogPara.KART_NO,4);
    //getdate(&recPara.OKUMA_TARIH);
    get_date(&recPara.OKUMA_TARIH);
    //recPara.OKUMA_TARIH.da_day = timeinfo->tm_mday;
    //recPara.OKUMA_TARIH.da_mon = timeinfo->tm_mon;
    //recPara.OKUMA_TARIH.da_year = timeinfo->tm_year;
    //gettime(&recPara.OKUMA_ZAMAN);
    get_time(&recPara.OKUMA_ZAMAN);
    //recPara.OKUMA_ZAMAN.ti_hour = timeinfo->tm_hour;
    //recPara.OKUMA_ZAMAN.ti_min = timeinfo->tm_min;
    //recPara.OKUMA_ZAMAN.ti_sec = timeinfo->tm_sec;
    //recPara.OKUMA_ZAMAN.ti_hund = 0;

    recPara.BAKIYE = rec_LogPara.BAKIYE;
    recPara.TUTAR = rec_LogPara.TUTAR;
    recPara.ISLEM = rec_LogPara.ISLEM;

    //lcd_printf(ALG_LEFT, "para islenecek");
    //lcd_flip();
    if (TCP_PersonelParaIsle(&recPara,tcp_Resp)==1)
    {
        r=1;
        memcpy(&tcp_Cevap,tcp_Resp,sizeof(tcp_Cevap));
        rec_LogPara.CEVAP=tcp_Cevap.CEVAP+1;
        rec_LogPara.CEVAP_ONLINE=1;
        //debug_print("para online islendi");
    }
    else
    {
        //debug_print("para offline islenecek");
        rec_LogPara.CEVAP=ic_Izin_Tamam;
        rec_LogPara.CEVAP_ONLINE=0;
        r=0;
    }

    free(tcp_Resp);

    //4.0.6 loga yazarken tcp'den gonderdigi tarih saatle yaziyor
    //bu sayede sql'de mukerrer kayit olusmasi engellenecek?
    PERSONEL_ParaLog(recPara.OKUMA_TARIH, recPara.OKUMA_ZAMAN);

    return r;
}


//6.0.0
void PERSONEL_Havuz_Log(void)
{
    memcpy(rec_LogKart.KART_NO,rec_Personel.KARTNO,4);
    memcpy(&rec_LogKart.OKUMA_TARIH,&rec_Personel.OKUMA_TARIHI,sizeof(struct_date));
    memcpy(&rec_LogKart.OKUMA_ZAMAN,&rec_Personel.OKUMA_SAATI,sizeof(struct_date));
    rec_LogKart.TERM_TIP = rec_TERM.TERM_TIP;
    rec_LogKart.KART_BAKIYE = rec_Personel.BAKIYE;
    rec_LogKart.KART_HARCANAN = rec_Personel.HARCANAN;
    //burada OGV yerine HB yazilsa, sunucu dogru okur mu acaba?
    rec_LogKart.KART_OGV = rec_Personel.OGV;
    rec_LogKart.OKUYUCU=0;
    LOG_Add_KART(&rec_LogKart);
    memset(&rec_LogKart,0,sizeof(rec_LogKart));
}

char GPIO_Gecti(char Okuyucu)
{
    unsigned char ret;
    struct timeval tNow, tLong, tEnd;
    //0.55
    //int Relay[2] = {13, 14};
    //int Sense[2] = {10, 11};
    int Relay[2] = {23, 13};
    int Sense[2] = {0, 2};
    ret=0;

    if (rec_TERM.READERS[Okuyucu-1].ROLE!=0) //Role Aktif Ise
    {
        //digitalWrite((Okuyucu-1)*2+1, 1);//röleyi çek
        //1.9.0
        //digitalWrite(Relay[Okuyucu-1], 1);//röleyi çek
        digitalWrite(Relay[rec_TERM.READERS[Okuyucu-1].ROLE -1], 1);//röleyi çek

        if (rec_TERM.READERS[Okuyucu-1].SENS!=0) //Sens Bekleyecekse
        {
            gettimeofday(&tNow, NULL);
            tLong.tv_sec = rec_TERM.READERS[Okuyucu-1].SENS_SN ;
            tLong.tv_usec = 0;
            //tLong.tv_usec = rec_TERM.READERS[Okuyucu-1].SENS_SN *1000000;
            timeradd(&tNow, &tLong, &tEnd);

            while (timercmp(&tNow, &tEnd, <))
            {
                //if(digitalRead((Okuyucu-1)*2+2)){
                //1.0.5
                //ayarlar.ini'deki sens cevabına göre hareket ediyor artık
                //if(digitalRead(Sense[Okuyucu-1])){
                //1.9.0
                //if(digitalRead(Sense[Okuyucu-1]) == rec_TERM.SENS_CEVAP){
                if(digitalRead(Sense[rec_TERM.READERS[Okuyucu-1].SENS-1]) == rec_TERM.SENS_CEVAP)
                {
                    ret=1;
                    break;
                }
                gettimeofday(&tNow, NULL);
            }
        }
        else
        {
            sleep((int)rec_TERM.READERS[Okuyucu-1].ROLE_SN);
            ret=1;
        }
        //digitalWrite((Okuyucu-1)*2+1, 0);//röleyi bırak
        //1.9.0
        //digitalWrite(Relay[Okuyucu-1], 0);//röleyi bırak
        digitalWrite(Relay[rec_TERM.READERS[Okuyucu-1].ROLE -1], 0);//röleyi bırak
    }
    else
    {
        ret=1;
    }

    return ret;
}


char PERSONEL_Gectimi(char Okuyucu)
{
    //0.50
    //1.8.1
    //while(gtk_events_pending())
    //  gtk_main_iteration();
    return GPIO_Gecti(Okuyucu);
}

void PERSONEL_Lcd(void)
{
    //unsigned char *szTmp; m8110
    char *szTmp;
    unsigned long BAKIYE;
    szTmp=calloc(100,1);
    lcd_clean();
    lcd_printf(ALG_CENTER,rec_Personel.KARTNO_HEX);
    //0.50
    gtk_clean();
    //0.60
    //gtk_layout_move(GTK_LAYOUT(main_layout), logo, 50, 5);
    gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);
    //gtk_printf(label1, rec_Personel.KARTNO_HEX);
    //LCD_PrintCenter(1,rec_Personel.UNVAN);
    //LCD_PrintCenter(1,rec_Personel.ADI);
    //LCD_PrintCenter(2,rec_Personel.SOYAD);
    sprintf(szTmp,"%s %s\n",rec_Personel.ADI,rec_Personel.SOYAD);
    lcd_printf(ALG_CENTER,szTmp);
    //0.50
    //gtk_printf(label2, szTmp);
    //0.52
    //1.0.1
    //unvan eklendi
    //1.0.6
    gtk_printf(label1, "%s\n%s\n%s",rec_Personel.KARTNO_HEX, szTmp, rec_Personel.UNVAN);
    if (rec_TERM.TERM_TIP==TERM_YEMEKHANE)
    {
        //6.0.0
        if(rec_HAVUZ.AKTIF)
            BAKIYE = rec_Personel.BAKIYE-20000;
        else
            BAKIYE = rec_Personel.BAKIYE;
        sprintf(szTmp,"PARA  : %4lu%c%02lu TL",(unsigned long) (BAKIYE/100),'.',(unsigned long)BAKIYE%100);
        lcd_printf(ALG_CENTER,szTmp);//odroid left idi
        //0.50
        //gtk_printf(label3, szTmp);
        //0.52
        gtk_printf(label1, "%s\n%s", gtk_label_get_label(GTK_LABEL(label1)), szTmp);

    }
    //lcd_printf(ALG_LEFT," ");
    lcd_flip();
    //while(gtk_events_pending())
    //  gtk_main_iteration();
    //2.0.1                    /
    if((Okuyucu_TIP[SMM_PORT] == 2)||(Okuyucu_TIP[SMM_PORT] == 3))
    {
        CN56B_Buzzer(ifd[SMM_PORT]);
        CN56B_Led(ifd[SMM_PORT]);
    }
    //2.2.0
    //brk
    //unutma koymayı yeniden
    MSJ_Ses(rec_TERM.SES_OKUMA_OK);

    //1.8.1
    //2.0.0
    //MSJ_Ses(rec_TERM.SES_OKUMA_OK);
    //2.0.1
    //CN56B_Buzzer(ifd[SMM_PORT]);
    //CN56B_Led(ifd[SMM_PORT]);
    free(szTmp);
}




int kb_hit(void)
{
    int ch = getch();

    if(ch != ERR)
    {
        ungetch(ch);
        return 1;
    }
    else
    {
        return 0;
    }
}

int get_keyms(int timeout)
{
    struct timeval  ts, tn, td;
    struct timezone tz;
    int             keyval = 0;

    nodelay(stdscr, TRUE);
    scrollok(stdscr, TRUE);

    gettimeofday(&ts, &tz);
    while(1)
    {
        if (kb_hit())
        {
            //keyval = kb_getkey();
            keyval = getch();
            break;
        }
        else
        {
            gettimeofday(&tn, &tz);
            timersub(&tn, &ts, &td);
            if (td.tv_sec * 1000 + td.tv_usec / 1000 >= timeout)
                break;

            //odroid
            //usleep(1*1000);
            napms(1);
        }
    }

    return keyval;
}

//m8110
//6.0.0
void HAVUZ(char Okyc)
{
    char 	*szTmp; //m8110 unsigned idi
//long 		 	t;
//unsigned long 	BAKIYE;
    unsigned long 	HARCAMA;
    int Seans_Fark = 0;

//6.4.0
    struct_date dd;

    int j;

    szTmp = calloc(100,1);
    memset(&rec_LogKart,0,sizeof(rec_LogKart));

    //t= AccTimer_SetTime(0);

    if (PERSONEL_KayipCalinti(rec_Personel.KARTNO)==1) //Kayip Calintı  Ise
    {
        rec_LogKart.GECTI_ONLINE=0;
        rec_LogKart.GECTI=0;
        rec_LogKart.CEVAP_ONLINE=0;
        rec_LogKart.CEVAP=ic_KayipCalinti;
        MSJ_Goster(rec_LogKart.CEVAP);
        MSJ_Ses(rec_TERM.SES_TANIMSIZ);
    }
    else
    {
        //6.0.0 Havuz problemi
        //odroid
        //if(rec_PINPAD.AKTIF)
        //	Hvz_TopUp = PINPAD_HAVUZ_TopUp(rec_Personel.KARTNO);
        //else
        //Hvz_TopUp = POS_HAVUZ_TopUp(rec_Personel.KARTNO);
        Hvz_TopUp = HAVUZ_TopUp(rec_Personel.KARTNO);

        switch(Hvz_TopUp)
        {
        case 1: //TopUp_NoFile:
            break;
        case 2: //TopUp_NoCard:
            break;
        case 3: //TopUp_OK:
            lcd_clean();
            lcd_printf(ALG_CENTER, "KART BILGILERINIZ");
            lcd_printf(ALG_CENTER, "GUNCELLENDI");
            lcd_printf(ALG_CENTER, "TEKRAR OKUTUNUZ");
            lcd_flip();
            //0.50
            gtk_clean();
            //gtk_printf(label2, "KART BILGILERINIZ");
            //gtk_printf(label3, "GUNCELLENDI");
            //gtk_printf(label4, "TEKRAR OKUTUNUZ");
            //0.61
            gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);
            //0.51
            //GdkRGBA font_color;
            font_color.alpha = 1;
            font_color.blue = 1;
            font_color.green = 0;
            font_color.red = 0;
            gtk_widget_override_color(label1, GTK_STATE_FLAG_NORMAL, &font_color);
            //0.61
            gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);
            //0.51
            //GdkRGBA font_color;
            font_color.alpha = 1;
            font_color.blue = 0;
            font_color.green = 1;
            font_color.red = 0;
            gtk_widget_override_color(label1, GTK_STATE_FLAG_NORMAL, &font_color);
            //0.52
            gtk_printf(label1, "KART BILGILERINIZ\nGUNCELLENDI\nTEKRAR OKUTUNUZ");
            gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_QUESTION);//GTK_MESSAGE_ERROR
            while(gtk_events_pending())
                gtk_main_iteration();

            MSJ_Ses(rec_TERM.SES_OKUMA_OK);
            MSJ_Ses(rec_TERM.SES_OKUMA_OK);
            sleep(2);
            MSJ_Goster(MSJ_TIP_LG);
            free(szTmp);
            return;
        //break;
        case 4: //TopUp_NOK:
            lcd_clean();
            lcd_printf(ALG_CENTER, "KART BILGILERINIZ");
            lcd_printf(ALG_CENTER, "GUNCELLENEMEDI");
            lcd_printf(ALG_CENTER, "TEKRAR OKUTUNUZ");
            lcd_flip();
            //0.50
            gtk_clean();
            //gtk_printf(label2, "KART BILGILERINIZ");
            //gtk_printf(label3, "GUNCELLENEMEDI");
            //gtk_printf(label4, "TEKRAR OKUTUNUZ");
            //0.61
            gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);
            //0.51
            //GdkRGBA font_color;
            font_color.alpha = 1;
            font_color.blue = 0;
            font_color.green = 0;
            font_color.red = 1;
            gtk_widget_override_color(label1, GTK_STATE_FLAG_NORMAL, &font_color);
            //0.52
            gtk_printf(label1, "KART BILGILERINIZ\nGUNCELLENEMEDI\nTEKRAR OKUTUNUZ");
            gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_ERROR);//GTK_MESSAGE_ERROR
            while(gtk_events_pending())
                gtk_main_iteration();
            for (j=0; j<4; j++)
                MSJ_Ses(rec_TERM.SES_OKUMA_NK);
            for (j=0; j<3; j++)
            {
                //WDT_Clear();
                //delay(900);
                sleep(1);
                MSJ_Ses(rec_TERM.SES_OKUMA_NK);
            }
            //MSJ_Ses(rec_TERM.SES_OKUMA_NK);
            //MSJ_Ses(rec_TERM.SES_OKUMA_NK);
            //delay(2000);
            MSJ_Goster(MSJ_TIP_LG);
            free(szTmp);
            return;
            //break;
        }


        //Havuz islemleri
        yCevap = 0x00;
        MSJ_OK = 0;
        z_MSJ_SURE_SN = 0;
        rec_LogKart.CEVAP_ONLINE = 0;
        if ((Havuz_Hata.HATA)&&(strcmp(Havuz_Hata.KARTNO,rec_Personel.KARTNO_HEX) == 0)) //Son Yazilan Hatali Kartsa
        {
            rec_LogKart.CEVAP = ic_Izin_Tamam;
            rec_Personel.HARCANAN = Havuz_Hata.HARCAMA;
            if (!Havuz_Hata.OK_B) //Bakiye Yazilamamis Gozukuyor
            {
                if (rec_Personel.BAKIYE < Havuz_Hata.BAKIYE) //Ama Yazilmis
                {
                    Havuz_Hata.OK_B = 1;
                    rec_Personel.BAKIYE = Havuz_Hata.BAKIYE; //Gosterim duzgun olsun
                }
            }
            else
            {
                rec_Personel.BAKIYE = Havuz_Hata.BAKIYE;
            }
        }
        else
        {
            memset(&Havuz_Hata, 0, sizeof(Havuz_Hata)); //Hatayi Bosalt
            rec_LogKart.CEVAP = PERSONEL_HavuzaGirermi();
        }

        if (rec_LogKart.CEVAP == ic_Izin_Tamam)
        {
            if (rec_Personel.BAKIYE > 40000) //kartta zaten 200.00 tl var
            {
                //havuz icin max 200.00 tl tasinabilir
                //lcd_set_bk_color(COLOR_RED);
                //lcd_set_font_color(COLOR_YELLOW);
                PERSONEL_Lcd();
                MSJ_Goster(rec_LogKart.CEVAP);
                lcd_printf(ALG_CENTER,"IZIN VERILMEDI !");
                lcd_printf(ALG_CENTER,"KART BOZULMUS !");
                lcd_flip();
                //gtk_printf(label4, "IZIN VERILMEDI !");
                //gtk_printf(label5, "KART BOZULMUS !");
                //0.52
                gtk_printf(label1, "%s\nİZİN VERİLMEDİ !\nKART BOZULMUŞ !", gtk_label_get_label(GTK_LABEL(label1)));
                //GdkRGBA bg_color;
                bg_color.alpha = 1;
                bg_color.blue = 0;
                bg_color.green = 0;
                bg_color.red = 1;
                gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
                //0.61
                //gtk_widget_override_font(label1, pango_font_description_from_string("Droid Sans Bold 12"));//Ubuntu Medium Italic 12
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12

                gtk_label_set_text(GTK_LABEL(label_status), "               İZİN VERİLMEDİ, KART BOZULMUŞ");


                gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_ERROR);//GTK_MESSAGE_ERROR
                while(gtk_events_pending())
                    gtk_main_iteration();
                MSJ_Ses(rec_TERM.SES_OKUMA_NK);
                MSJ_Ses(rec_TERM.SES_OKUMA_NK);
                MSJ_Ses(rec_TERM.SES_OKUMA_NK);
                MSJ_Ses(rec_TERM.SES_OKUMA_NK);
                //lcd_set_bk_color(COLOR_BLUE);
                //lcd_set_font_color(COLOR_YELLOW);
                free(szTmp);
                return;
            }
            //if (READER_Havuz_Write(rec_Personel.KARTNO))
            if (READER_Havuz_Write(Okyc-1))
            {
                //lcd_set_bk_color(COLOR_WITE);
                //lcd_set_font_color(COLOR_BLACK);
                memset(&Havuz_Hata,0,sizeof(Havuz_Hata)); //Hatayi Bosalt
                //t = AccTimer_GetTime(t);
                //printf("MS:%lu\r\n",t);
                PERSONEL_Lcd();
                if(rec_HAVUZ.GIRIS)//giris turnikesiyse
                    HARCAMA = rec_Personel.HARCANAN;
                else //cikis turnikesiyse
                {
                    if(rec_HB.SNRSZ != SINIRSIZ) //6.4.0 sinirsiz uyelik degilse seans farki al
                    {
                        get_date(&dd);
                        if(memcmp(&rec_HB.GIRIS_TARIHI,&dd, 4) != 0)
                            HARCAMA = rec_HAVUZ.CEZA;
                        else
                        {
                            Seans_Fark = rec_Seans.ID - rec_HB.SEANS_ID;
                            if(Seans_Fark > 0) //farkli bir seansta ciktiysa
                                HARCAMA = Seans_Fark * rec_HB.FIYAT;//girdigi seans ucreti kadar ekle
                            else
                                HARCAMA = 0;
                        }
                    }
                    else
                        HARCAMA = 0;//6.4.0 sinirsiz uyelikse seans farki alma

//					Seans_Fark = rec_Seans.ID - rec_HB.SEANS_ID;
//					if(Seans_Fark > 0) //farkli bir seansta ciktiysa
//						HARCAMA = Seans_Fark * rec_HB.FIYAT;//girdigi seans ucreti kadar ekle
//					else
//						HARCAMA = 0;
                }

                //sprintf(szTmp,"SEANS : %6lu%c%02lu TL",(unsigned long) (rec_Personel.HARCANAN/100),'.',(unsigned long)rec_Personel.HARCANAN%100);
                sprintf(szTmp,"SEANS : %6lu%c%02lu TL",(unsigned long) (HARCAMA/100),'.',(unsigned long)HARCAMA%100);
                lcd_printf(ALG_LEFT, szTmp);
                //0.50
                //gtk_printf(label4, szTmp);
                //0.52
                gtk_printf(label1, "%s\n%s", gtk_label_get_label(GTK_LABEL(label1)), szTmp);

                //rec_Personel.BAKIYE = rec_Personel.BAKIYE - rec_Personel.HARCANAN;
                rec_Personel.BAKIYE = rec_Personel.BAKIYE - HARCAMA;
                //6.4.0
                //sprintf(szTmp,"KALAN : %6lu%c%02lu TL",(unsigned long) ((rec_Personel.BAKIYE-20000)/100),'.',(unsigned long)(rec_Personel.BAKIYE-20000)%100);
                if(rec_Personel.BAKIYE < 20000)
                    sprintf(szTmp,"KALAN : (-)%3lu%c%02lu TL",(unsigned long) ((20000-rec_Personel.BAKIYE)/100),'.',(unsigned long)(20000-rec_Personel.BAKIYE)%100);
                else
                    sprintf(szTmp,"KALAN : %6lu%c%02lu TL",(unsigned long) ((rec_Personel.BAKIYE-20000)/100),'.',(unsigned long)(rec_Personel.BAKIYE-20000)%100);

                lcd_printf(ALG_LEFT,szTmp);
                lcd_printf(ALG_CENTER,"IZIN VERILDI");
                if(rec_HAVUZ.GIRIS) //giris turnikesiyse
                {
                    lcd_printf(ALG_CENTER,"IYI YUZMELER");
                    //0.52
                    gtk_printf(label1, "%s\n%s\nİZİN VERİLDİ\nİYİ YÜZMELER", gtk_label_get_label(GTK_LABEL(label1)), szTmp);
                }
                else
                {
                    lcd_printf(ALG_CENTER,"YINE BEKLERIZ");
                    //0.52
                    gtk_printf(label1, "%s\n%s\nİZİN VERİLDİ\nYİNE BEKLERİZ", gtk_label_get_label(GTK_LABEL(label1)), szTmp);
                }
                //0.52
                while(gtk_events_pending())
                    gtk_main_iteration();
                rec_LogKart.GECTI = PERSONEL_Gectimi(Okyc);
                lcd_flip();
                //if (rec_LogKart.GECTI) gecsede gecmesede
                //{
                rec_Personel.YB.SAYI = rec_LogKart.GECTI;
                rec_LogKart.GECTI_ONLINE = PERSONEL_HareketIsle();
                SAYAC_Arti();//burasi da havuzicin ayrica yapilmali mi acaba?
                PERSONEL_Havuz_Log();//havuz icin yapilacak.
                //lcd_set_bk_color(COLOR_BLUE);
                //lcd_set_font_color(COLOR_YELLOW);
                //}
            }
            else
            {

                //lcd_set_bk_color(COLOR_RED);
                //lcd_set_font_color(COLOR_YELLOW);
                Havuz_Hata.HATA=1;
                strcpy(Havuz_Hata.KARTNO, rec_Personel.KARTNO_HEX);
                Havuz_Hata.BAKIYE = rec_Personel.BAKIYE;
                //cikis turnikesi ise rec_Personel_HARCANAN degil de
                //o sirada yapilan Harcama olmali burada.
                Havuz_Hata.HARCAMA = rec_Personel.HARCANAN;
                memcpy(&Havuz_Hata.HB, &rec_HB, sizeof(struct_HAVUZ_BILGI));
                //6.4.0
                memcpy(&Havuz_Hata.GIRIS_TARIHI, &rec_HB.GIRIS_TARIHI, 4);
                if(rec_HAVUZ.GIRIS)
                    Havuz_Hata.IN_OUT = 1;
                else
                    Havuz_Hata.IN_OUT = 0;

                rec_LogKart.CEVAP = ic_Izin_Yok;
                PERSONEL_Lcd();
                lcd_printf(ALG_CENTER,"YAZMA HATASI");//RESOURCE_ReadLine(400));// "KART OKUMA HATASI"
                lcd_printf(ALG_CENTER,"KARTI OKUYUCUYA" );//RESOURCE_ReadLine(400));// "KART OKUMA HATASI"
                lcd_printf(ALG_CENTER,"YAKIN TUTUNUZ" );//RESOURCE_ReadLine(400));// "KART OKUMA HATASI"
                lcd_printf(ALG_CENTER,"3 SN SONRA OKUR" );//RESOURCE_ReadLine(400));// "KART OKUMA HATASI"
                lcd_flip();
                //0.50
                //gtk_printf(label4, "YAZMA HATASI KARTI OKUYUCUYA");
                //gtk_printf(label5, "YAKIN TUTUNUZ 3 SN SONRA OKUR");
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
                //0.52
                gtk_printf(label1, "%s\nYAZMA HATASI KARTI OKUYUCUYA\nYAKIN TUTUNUZ 3 SN SONRA OKUR", gtk_label_get_label(GTK_LABEL(label1)));
                //0.61
                //gtk_widget_override_font(label1, pango_font_description_from_string("Droid Sans Bold 12"));//Ubuntu Medium Italic 12
                gtk_label_set_text(GTK_LABEL(label_status), "                İZİN VERİLMEDİ, YAZMA HATASI");

                while(gtk_events_pending())
                    gtk_main_iteration();
                for (j=0; j<3; j++)
                    MSJ_Ses(rec_TERM.SES_OKUMA_NK);
                //1.8.1
                for (j=0; j<3; j++)
                {
                    //WDT_Clear();
                    //delay(900);
                    sleep(1);
                    MSJ_Ses(rec_TERM.SES_OKUMA_NK);
                }
                lcd_clean();
                MSJ_OK = 0;
                z_MSJ_SURE_SN = 0;
                //lcd_set_bk_color(COLOR_BLUE);
                //lcd_set_font_color(COLOR_YELLOW);
            }
        }
        else
        {
            //lcd_set_bk_color(COLOR_RED);
            //lcd_set_font_color(COLOR_YELLOW);
            PERSONEL_Lcd();
            MSJ_Goster(rec_LogKart.CEVAP);
            lcd_printf(ALG_CENTER,"IZIN VERILMEDI !");
            lcd_flip();
            //0.52
            gtk_printf(label1, "%s\nİZİN VERİLMEDİ !", gtk_label_get_label(GTK_LABEL(label1)));
            //GdkRGBA bg_color;
            bg_color.alpha = 1;
            bg_color.blue = 0;
            bg_color.green = 0;
            bg_color.red = 1;
            gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
            //0.61
            //gtk_widget_override_font(label1, pango_font_description_from_string("Droid Sans Bold 12"));//Ubuntu Medium Italic 12
            gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12

            //gtk_label_set_text(GTK_LABEL(label_status), "!              İZİN VERİLMEDİ, YAZMA HATASI              !");
            gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_ERROR);//GTK_MESSAGE_ERROR
            while(gtk_events_pending())
                gtk_main_iteration();
            MSJ_Ses(rec_TERM.SES_OKUMA_NK);
            MSJ_Ses(rec_TERM.SES_OKUMA_NK);
        }
        switch (yCevap)
        {
        case ic_HakBitmis:
            lcd_printf(ALG_CENTER,"UYELIGINIZ BITMIS");
            lcd_flip();
            gtk_label_set_text(GTK_LABEL(label_status), "        İZİN VERİLMEDİ, ÜYELİĞİNİZ BİTMİŞ");
            //sleep(1);
            break;
        case   ic_ParaBitmis:
            lcd_printf(ALG_CENTER,"BAKIYE YETERSIZ");
            lcd_flip();
            gtk_label_set_text(GTK_LABEL(label_status), "         İZİN VERİLMEDİ, BAKİYE YETERSİZ");
            //sleep(1);
            break;
        case   ic_GrupYetkisiz:
            lcd_printf(ALG_CENTER,"GECIS YETKISI YOK");
            lcd_flip();
            gtk_label_set_text(GTK_LABEL(label_status), "        İZİN VERİLMEDİ, GEÇİŞ YETKİSİ YOK");
            //sleep(1);
            break;
        case   ic_OgunDisinda:
            lcd_printf(ALG_CENTER,"SEANS DISINDASINIZ");
            lcd_flip();
            gtk_label_set_text(GTK_LABEL(label_status), "        İZİN VERİLMEDİ, SEANS DIŞINDASINIZ");
            //sleep(1);
            break;
        case   ic_UygunsuzGrup:
            lcd_printf(ALG_CENTER,"GRUP KISITLAMASI");
            lcd_flip();
            gtk_label_set_text(GTK_LABEL(label_status), "         İZİN VERİLMEDİ, GRUP KISITLAMASI");
            //sleep(1);
            break;
        case   ic_UygunsuzCinsiyet:
            lcd_printf(ALG_CENTER,"CINSIYET KISITLAMASI");
            lcd_flip();
            gtk_label_set_text(GTK_LABEL(label_status), "        İZİN VERİLMEDİ, CİNSİYET KISITLAMASI      ");
            //sleep(1);
            break;
        case   ic_GirisYapmamis:
            lcd_printf(ALG_CENTER,"GIRIS YAPILMAMIS");
            lcd_flip();
            gtk_label_set_text(GTK_LABEL(label_status), "        İZİN VERİLMEDİ,   GİRİŞ YAPILMAMIŞ         ");
            //sleep(1);
            break;
        case   ic_CikisYapmamis:
            lcd_printf(ALG_CENTER,"CIKIS YAPILMAMIS");
            gtk_label_set_text(GTK_LABEL(label_status), "        İZİN VERİLMEDİ, CIKIŞ YAPILMAMIŞ            ");
            lcd_flip();
            //sleep(1);
            break;
        }
        while(gtk_events_pending())
            gtk_main_iteration();
        sleep(1);
        //lcd_set_bk_color(COLOR_BLUE);
        //lcd_set_font_color(COLOR_YELLOW);
    }
    free(szTmp);
}

int PERSONEL_IzinAl(char * data, int Okuyucu)
{
    struct_date d;
    struct_time t;
    int fis_socket;
    fd_set readset;
    int result, iof = -1;
    struct timeval tv;
    char *strRcv;
    char *szBuf;
//uint16_t   L;
    int retval = 1;
    time_t rawtime;
    struct tm * timeinfo;

//1.7.0
    struct timeval tNow, tLong, tEnd;
    int key, i;
    char sifre[5];
//2.0.0
    char sfr[35];//max 35 chars read by keyb buffer of cn56b

//2.1.0
    char* lData;
    char Line1[128];
    char Line2[128];
    char Ses[128];
//2.1.1
    char szTmp[100];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    d.da_day = (char) timeinfo->tm_mday;
    d.da_mon = (char) timeinfo->tm_mon+1;
    d.da_year = (uint16_t)timeinfo->tm_year + 1900;

    t.ti_hour = (unsigned char) timeinfo->tm_hour;
    t.ti_min = (unsigned char) timeinfo->tm_min;
    t.ti_sec = (unsigned char) timeinfo->tm_sec;
    t.ti_hund = 0;
    szBuf = calloc(100,1);
    strRcv = ( char *) calloc (2049, 1);
    //2.10
    lData = ( char *) calloc (2049, 1);

    //1.7.0
    //10 sn içinde 4 basamlı şifre girilmesini bekler
    //2.0.0
    //if((rec_TERM.TERM_TIP == TERM_KAPI) && KEYPAD_AKTIF && KART_SIFRE_AKTIF){
    if((rec_TERM.TERM_TIP == TERM_KAPI) && KEYPAD_AKTIF && KART_SIFRE_AKTIF && OKUYUCU_SIFRE_AKTIF[Okuyucu])
    {
        //2.0.0
        if(Okuyucu_TIP[Okuyucu] == TIP_CN_KYB)
        {
            key = CN56B_Keyb(ifd[Okuyucu], sfr);//to clear the keyb buffer
        }
        MSJ_Ses(rec_TERM.SES_OKUMA_OK);
        tLong.tv_sec = 5;
        tLong.tv_usec = 0L;
        gettimeofday(&tNow, NULL);
        timeradd(&tNow, &tLong, &tEnd);
        i = 0;
        memset(sifre, 0x0, 5);
        retval = SMM_ERR_SIFRE;
        bg_color.alpha = 0.5;
        bg_color.blue = 0;
        bg_color.green = 1;
        bg_color.red = 0;
        gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
        gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12
        gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);
        gtk_printf(label1, "%s\n%s","5 SANİYE İÇİNDE", "ŞİFRENİZİ GİRİNİZ");
        while(gtk_events_pending())
            gtk_main_iteration();
        while (timercmp(&tNow, &tEnd, <))
        {
            //2.0.0
            if(Okuyucu_TIP[Okuyucu] == TIP_CN_KYB)
            {
                memset(sfr, 0, 35);//clear sfr buffer
                key = CN56B_Keyb(ifd[Okuyucu], sfr);
                if(key > -1)
                {
                    memcpy(sifre+i, sfr, key); //sifre[i++] = (char)key;
                    i+= key;
                }
            }
            else
            {
                key=KEYB_getch(1);
                if((key > -1) && (key < 10))
                {
                    sifre[i++] = key + 0x30;
                }
                else if(key == KEY_BKSP)
                {
                    i--;
                    sifre[i] = 0x0;
                }
            }
            //2.0.0
            //if(i == 4){
            if(i >3)
            {
                if(memcmp(sifre, Kart_Sifre, 4) == 0)
                {
                    retval = 0;
                    break;
                }
                else
                {
                    free(szBuf);
                    free(strRcv);
                    free(lData);//2.1.0
                    return SMM_ERR_SIFRE;
                }
            }
            gettimeofday(&tNow, NULL);
        }
        if(retval == SMM_ERR_SIFRE)
        {
            free(szBuf);
            free(strRcv);
            free(lData);//2.1.0
            return SMM_ERR_SIFRE;
        }
    }
    retval = 1;

    sprintf(szBuf,"%c%c%s%c%s%c%s%c%d%c%04d%02d%02d%02d%02d%02d%c",
            cmd_IZIN,cAyrac_1,
            rec_TERM.KOD, cAyrac_2,
            data,cAyrac_2,"000",cAyrac_2,Okuyucu,cAyrac_2,
            d.da_year,d.da_mon,d.da_day,
            t.ti_hour,t.ti_min,t.ti_sec,
            cAyrac_Son);

    if(READER_TYPE == READER_BARCODE)
        fis_socket = TCP_Connect(rec_TERM.IP_SERVER, rec_TERM.PORT_SERVER+20);//1.0.7 poslarla çakışmasın diye
    else
        fis_socket = TCP_Connect(rec_TERM.IP_SERVER, rec_TERM.PORT_SERVER);
    if (fis_socket>0)
    {
        write(fis_socket, szBuf, strlen((char *)szBuf));
        // Initialize the set
        FD_ZERO(&readset);
        FD_SET(fis_socket, &readset);

        // Initialize time out struct
        //2.4.0
        //tv.tv_sec = 0;
        //tv.tv_usec = 2000 * 1000;
        tv.tv_sec = KAPI_TCP_WAIT_TIME;
        tv.tv_usec = 0;
        // select()
        result = select(fis_socket+1, &readset, NULL, NULL, &tv);

        // Check status
        //1.7.0
        //sunucu bağlantısı var ama timeout'a düşüp cevap dönmediyse offline izin verildi dön
        //if (result < 0){
        //  retval = 0x1;
        if (result < 1)
        {
            //debug_print("tcp_error");
            retval = -1;
        }
        else if (result > 0 && FD_ISSET(fis_socket, &readset))
        {
            // Set non-blocking mode
            if ((iof = fcntl(fis_socket, F_GETFL, 0)) != -1)
                fcntl(fis_socket, F_SETFL, iof | O_NONBLOCK);
            // receive
            result = recv(fis_socket, strRcv, 2048, 0);
            // set as before
            if (iof != -1)
                fcntl(fis_socket, F_SETFL, iof);
            //return result;
            //debug_print("tcp read: %d bytes", result);
            retval = 0;
        }

        if (retval==0)
        {
            retval=1;
            //Length is in result
            //L = result;
            //2.1.0
            TOOL_GetSplit(strRcv, 1, cAyrac_1, lData);
            TOOL_GetSplit(lData, 0, cAyrac_2, Line1);
            TOOL_GetSplit(lData, 1, cAyrac_2, Line2);
            TOOL_GetSplit(lData, 2, cAyrac_2, Ses);
            //Ses[8] = 0;

            //strRcv[0]=cmd_MSFR_ONAY;
            //brk kaldırmayı unutma
            //strRcv[0]=cmd_CANCEL;
            if (strRcv[0]==cmd_OK)
            {
                retval=0;//izin var
                bg_color.alpha = 0.5;
                bg_color.blue = 0;
                bg_color.green = 1;
                bg_color.red = 0;
                gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12
                gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);
                gtk_printf(label1, "%s\n%s", Line1, Line2);
                //2.1.1
                gtk_label_set_text(GTK_LABEL(label_status), "                   İZİN VERİLDİ    ");
                gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_QUESTION);//GTK_MESSAGE_ERROR
                while(gtk_events_pending())
                    gtk_main_iteration();
                MSJ_Ses(Ses);
            }
            else if (strRcv[0]==cmd_MSFR_ONAY)
            {
                //retval=0;//izin var
                bg_color.alpha = 0.5;
                bg_color.blue = 0;
                bg_color.green = 0;
                bg_color.red = 1;
                gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12
                gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);
                gtk_printf(label1, "%s\n%s\nMİSAFİR FİYATI\nONAYLAMAK İÇİN ENT", Line1, Line2);
                while(gtk_events_pending())
                    gtk_main_iteration();

                MSJ_Ses(Ses);
                key=KEYB_getch(5);
                if(key == KEY_ENTR)
                {
                    retval = 0;//onay var
                }//2.1.1
                else
                {
                    gtk_clean();
                    bg_color.alpha = 1;
                    bg_color.blue = 0;
                    bg_color.green = 0;
                    bg_color.red = 1;
                    gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12
                    gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);

                    sprintf(szTmp,"%s\n%s\n%s %s",rec_Personel.KARTNO_HEX, rec_Personel.ADI, rec_Personel.SOYAD, "Izin Verilmedi");
                    gtk_printf(label1, "%s\n%s %s\n%s",rec_Personel.KARTNO_HEX, rec_Personel.ADI, rec_Personel.SOYAD, "RED EDİLDİ");
                    gtk_label_set_text(GTK_LABEL(label_status), "                    RED EDİLDİ");

                    gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_ERROR);//GTK_MESSAGE_ERROR
                    lcd_flip();
                    //1.9.5
                    while(gtk_events_pending())
                        gtk_main_iteration();
                }
            }
            else if (strRcv[0]==cmd_CANCEL)
            {
                bg_color.alpha = 1;
                bg_color.blue = 0;
                bg_color.green = 0;
                bg_color.red = 1;
                gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12
                gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);
                gtk_printf(label1, "%s\n%s", Line1, Line2);
                //2.1.1
                gtk_label_set_text(GTK_LABEL(label_status), "                   İZİN VERİLMEDİ    ");
                gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_ERROR);//GTK_MESSAGE_ERROR
                while(gtk_events_pending())
                    gtk_main_iteration();
                MSJ_Ses(Ses);
            }


            while(gtk_events_pending())
                gtk_main_iteration();
        }


        //2.5.2
        //sıfırdan buyuk olmasa burada ne isi var
        //if (fis_socket >=0)
        close(fis_socket);
        fis_socket = -1;
    }
    else
        retval = -1;//sunucu yok

    free(szBuf);
    free(strRcv);
    free(lData);//2.1.0
    return retval;
}


//2.1.0
int PDKS_HareketIsle(char * data, int Okuyucu)
{
    struct_date d;
    struct_time t;
    int fis_socket;
    fd_set readset;
    int result, iof = -1;
    struct timeval tv;
    char *strRcv;
    char *szBuf;
//uint16_t   L;
    int retval = 1;
    time_t rawtime;
    struct tm * timeinfo;

//2.1.0
//char* lData;
//char Line1[128];
//char Line2[128];
//char Ses[128];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    d.da_day = (char) timeinfo->tm_mday;
    d.da_mon = (char) timeinfo->tm_mon+1;
    d.da_year = (uint16_t)timeinfo->tm_year + 1900;

    t.ti_hour = (unsigned char) timeinfo->tm_hour;
    t.ti_min = (unsigned char) timeinfo->tm_min;
    t.ti_sec = (unsigned char) timeinfo->tm_sec;
    t.ti_hund = 0;
    szBuf = calloc(100,1);
    strRcv = ( char *) calloc (2049, 1);
    //2.10
    //lData = ( char *) calloc (2049, 1);

    retval = 1;

    sprintf(szBuf,"%c%c%s%c%s%c%s%c%d%c%04d%02d%02d%02d%02d%02d%c",
            cmd_HAREKET,cAyrac_1,
            rec_TERM.KOD, cAyrac_2,
            data,cAyrac_2,"000",cAyrac_2,Okuyucu,cAyrac_2,
            d.da_year,d.da_mon,d.da_day,
            t.ti_hour,t.ti_min,t.ti_sec,
            cAyrac_Son);

    if(READER_TYPE == READER_BARCODE)
        fis_socket = TCP_Connect(rec_TERM.IP_SERVER, rec_TERM.PORT_SERVER+20);//1.0.7 poslarla çakışmasın diye
    else
        fis_socket = TCP_Connect(rec_TERM.IP_SERVER, rec_TERM.PORT_SERVER);
    if (fis_socket>0)
    {
        write(fis_socket, szBuf, strlen((char *)szBuf));
        // Initialize the set
        FD_ZERO(&readset);
        FD_SET(fis_socket, &readset);

        // Initialize time out struct
        tv.tv_sec = 0;
        tv.tv_usec = 2000 * 1000;
        // select()
        result = select(fis_socket+1, &readset, NULL, NULL, &tv);

        // Check status
        //1.7.0
        //sunucu bağlantısı var ama timeout'a düşüp cevap dönmediyse offline izin verildi dön
        //if (result < 0){
        //  retval = 0x1;
        if (result < 1)
        {
            //debug_print("tcp_error");
            retval = -1;
        }
        else if (result > 0 && FD_ISSET(fis_socket, &readset))
        {
            // Set non-blocking mode
            if ((iof = fcntl(fis_socket, F_GETFL, 0)) != -1)
                fcntl(fis_socket, F_SETFL, iof | O_NONBLOCK);
            // receive
            result = recv(fis_socket, strRcv, 2048, 0);
            // set as before
            if (iof != -1)
                fcntl(fis_socket, F_SETFL, iof);
            //return result;
            //debug_print("tcp read: %d bytes", result);
            retval = 0;
        }

        if (retval==0)
        {
            retval=1;
            //Length is in result
            //L = result;
            //2.1.0
            //hareket işle de mesaj dönmüyormuş
            //TOOL_GetSplit(strRcv, 1, cAyrac_1, lData);
            //TOOL_GetSplit(lData, 0, cAyrac_2, Line1);
            //TOOL_GetSplit(lData, 1, cAyrac_2, Line2);
            //TOOL_GetSplit(lData, 2, cAyrac_2, Ses);


            //strRcv[0]=cmd_MSFR_ONAY;//brk kaldırmayı unutma
            if (strRcv[0]==cmd_OK)
            {
                retval=1;//Hareket İşlendi
                /*
                bg_color.alpha = 0.5;
                bg_color.blue = 0;
                bg_color.green = 1;
                bg_color.red = 0;
                gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12
                gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);
                gtk_printf(label1, "%s\n%s", Line1, Line2);
                while(gtk_events_pending())
                    gtk_main_iteration();

                MSJ_Ses(Ses);
                */
            }
            else
                retval=0;//Hareket İşlenmedi

            //while(gtk_events_pending())
            //    gtk_main_iteration();
        }


        //2.5.2.
        //sıfırdan buyuk olmasa burada ne işi var
        //if (fis_socket >=0)
        close(fis_socket);
        fis_socket = -1;
    }
    else
        retval = -1;//sunucu yok

    free(szBuf);
    free(strRcv);
    //free(lData);//2.1.0
    return retval;
}

//Online: 0 offline, 1 online
//InOutStatus: 0 giris, 1 çıkış
//2.1.0
//void PDKS_AddLog(char Online, int InOutStatus)
void PDKS_AddLog(char * Data, char Online, int InOutStatus)
{
    FILE * out;
    char *Line;
    char IOS[2], Cevrimici[2];
    Line = (char*) calloc(100, 1);
    memset(IOS, 0, 2);
    memset(Cevrimici, 0, 2);
    memset(Line,0x00,100);

    if(Online)
        Cevrimici[0]= '1';
    else
        Cevrimici[0] = '0';

    if(InOutStatus)
        IOS[0] = '1';
    else
        IOS[0]= '0';

    //2.1.0
    //sprintf(Line,"%s%02d%02d%04d%02d%02d%10s%3s %s%s",
    //2.4.1
    //sprintf(Line,"%s%02d%02d%04d%02d%02d%15s%3s %s%s",
    //        rec_TERM.KOD,
    //        rec_Personel.OKUMA_TARIHI.da_day,
    //        rec_Personel.OKUMA_TARIHI.da_mon,
    //        rec_Personel.OKUMA_TARIHI.da_year,
    //        rec_Personel.OKUMA_SAATI.ti_hour,
    //        rec_Personel.OKUMA_SAATI.ti_min,
    //2.1.0
    //rec_Personel.KARTNO_HEX,
    //       Data,
    //       "000", Cevrimici, IOS);
    if(ISKUR)
    {
        sprintf(Line,"%s%02d%02d%04d%02d%02d%15s%3s %s%s",
                rec_TERM.KOD,
                rec_Personel.OKUMA_TARIHI.da_day,
                rec_Personel.OKUMA_TARIHI.da_mon,
                rec_Personel.OKUMA_TARIHI.da_year,
                rec_Personel.OKUMA_SAATI.ti_hour,
                rec_Personel.OKUMA_SAATI.ti_min,
                //2.1.0
                //rec_Personel.KARTNO_HEX,
                Data,
                "000", Cevrimici, IOS);
    }
    else
    {
        sprintf(Line,"%s%02d%02d%04d%02d%02d%10s%3s %s%s",
                rec_TERM.KOD,
                rec_Personel.OKUMA_TARIHI.da_day,
                rec_Personel.OKUMA_TARIHI.da_mon,
                rec_Personel.OKUMA_TARIHI.da_year,
                rec_Personel.OKUMA_SAATI.ti_hour,
                rec_Personel.OKUMA_SAATI.ti_min,
                //2.1.0
                //rec_Personel.KARTNO_HEX,
                Data,
                "000", Cevrimici, IOS);
    }


    out=fopen("/home/odroid/Solidus/log.txt","a+b");
    fprintf(out,"%s\r\n",Line);
    fclose(out);
    free(Line);
}

//2.1.0
int TCKimlikAl(char *data)
{
    struct timeval tNow, tLong, tEnd;
    int key, i;
    char sifre[12];

    key=KEYB_getch(1);
    if(key== 0xF4)
    {
        //2.1.3
        //15 sn yapildi
        //tLong.tv_sec = 10;
        tLong.tv_sec = 15;
        tLong.tv_usec = 0L;
        gettimeofday(&tNow, NULL);
        timeradd(&tNow, &tLong, &tEnd);
        i = 0;
        memset(sifre, 0x0, 12);
        bg_color.alpha = 0.5;
        bg_color.blue = 0;
        bg_color.green = 1;
        bg_color.red = 0;
        gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
        gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12
        gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);
        //2.1.3
        //gtk_printf(label1, "%s\n%s","10 SANİYE İÇİNDE", "T.C. KİMLİK NUMARANIZI GİRİNİZ");
        gtk_printf(label1, "%s\n%s","15 SANİYE İÇİNDE", "T.C. KİMLİK NUMARANIZI GİRİNİZ");
        while(gtk_events_pending())
            gtk_main_iteration();
        while (timercmp(&tNow, &tEnd, <))
        {
            //2.2.6
            //key=KEYB_getch(1);
            key=KEYB_getch(0);
            if((key > -1) && (key < 10))
            {
                sifre[i++] = key + 0x30;
                gtk_printf(label1, "%s\n%s\n%s","15 SANİYE İÇİNDE", "T.C. KİMLİK NUMARANIZI GİRİNİZ", sifre);
                while(gtk_events_pending())
                    gtk_main_iteration();
            }
            else if(key == KEY_BKSP)
            {
                i--;
                sifre[i] = 0x0;
                gtk_printf(label1, "%s\n%s\n%s","15 SANİYE İÇİNDE", "T.C. KİMLİK NUMARANIZI GİRİNİZ", sifre);
                while(gtk_events_pending())
                    gtk_main_iteration();
            }

            //2.0.0
            //if(i == 4){
            if(i >10)
            {
                memcpy(data, sifre, 11);
                get_date(&rec_Personel.OKUMA_TARIHI);
                get_time(&rec_Personel.OKUMA_SAATI);
                return 0;//tckimlik ok
            }
            gettimeofday(&tNow, NULL);
        }
        gtk_clean();
        gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x, logo_y);
        while(gtk_events_pending())
            gtk_main_iteration();
        MSJ_Ses(rec_TERM.SES_OKUMA_NK);
        return 1;//no data
    }
    else
        return 1;//no data
}

void PERSONEL_Karsila(void)
{
    char Okuyucu;
    int j;
    char *szTmp;
//int selected;


    time_t rawtime;
    struct tm * timeinfo;
//char ret=0;
    unsigned char *tcp_Resp;
    struct_IZINCEVAP tcp_Cevap;

//struct_date d;
//struct_time t;
//int fis_socket;
//fd_set readset;
//int result, iof = -1;
//struct timeval tv;

//uint16_t   L;
    int retval = 1;
//7.6.0
    int key;
//2.2.5
//unsigned long long timer5sn, timer55sn;
    struct timeval timer5sn, timer55sn;
    struct_PERSONEL	 onceki_kart;
//2.1.0
    int yemek_sayisi = 0;//2.3.1
    /*
    //2.4.0
    typedef struct{
    	char KART_NO[21];
    	char SICIL_NO[21];
    	char ADISOYADI[21];
    	char TZ_INOUT;
    	char TZ_ONOFF;
    	char MSJ_NO;
    } PDKS_KART_Config;
    //2.4.0
    PDKS_KART_Config KART;
    */
    tcp_Resp = (unsigned char *) calloc(sizeof(tcp_Cevap)+1, 1);

    szTmp=calloc(100,1);

    //2.2.3
    //memset(&rec_LogKart,0,sizeof(rec_LogKart));
    memset(&rec_LogKart,0,sizeof(struct_LOG_KART));

    //2.2.3
    memset(&rec_Personel, 0, sizeof(struct_PERSONEL));

    //1.0.0
    //1.0.1
    //READER_TYPE = 0x0 olunca server_port + 10'dan data göndermeye uğraşıyordu
    READER_TYPE = 0x1;

    Okuyucu = READER_Read();
    if(Okuyucu == 0) //odroid burası iki okuyucu varsa diye yapılmış. ona göre davran
    {
        //2.1.0
        //burada keypadden tc kimlik bilgisi okumaya calış
        //okuyamazsan çık
        if(TCKimlikAl(TCKimlik))
        {
            free(szTmp);
            free(tcp_Resp);
            //free(szBuf);
            //free(strRcv);
            return;
        }
        else
            Okuyucu = 4;//keypad
    }

    //1.0.0
    if(READER_TYPE == READER_BARCODE)//Barcode
    {
        if (rec_TERM.TERM_TIP == TERM_YEMEKHANE)
        {

            retval = PERSONEL_IzinAl(Barcode_Fis, 2);
            switch (retval)
            {
            case 0:
                //izin verildi turnikeyi aç
                lcd_clean();
                gtk_clean();
                bg_color.alpha = 0.5;
                bg_color.blue = 0;
                bg_color.green = 1;
                bg_color.red = 0;
                gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12
                gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);

                sprintf(szTmp,"%s\n%s","Ziyaretci Fisi", "Izin Verildi");
                lcd_printf(ALG_CENTER,szTmp);
                gtk_printf(label1, "%s\n%s","ZİYARETCİ FİŞİ", "İZİN VERİLDİ");
                gtk_label_set_text(GTK_LABEL(label_status), "            İZİN VERİLDİ, AFİYET OLSUN");
                gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_QUESTION);//GTK_MESSAGE_ERROR
                lcd_flip();
                MSJ_Goster(MSJ_TIP_OK);
                //Yemekhane iken hep 1. okuyucu gibi işlem yap
                PERSONEL_Gectimi(1);

                /*
                rec_LogKart.GECTI = PERSONEL_Gectimi(Okuyucu);
                //if (rec_LogKart.GECTI) gecsede gecmesede
                //{
                rec_Personel.YB.SAYI=rec_LogKart.GECTI;
                rec_LogKart.GECTI_ONLINE=PERSONEL_HareketIsle();
                SAYAC_Arti();
                PERSONEL_Log(Okuyucu);
                */

                MSJ_Ses(rec_TERM.SES_OKUMA_OK);
                break;
            case 1:
                //izin verilmedi
                lcd_clean();
                gtk_clean();
                gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);

                sprintf(szTmp,"%s\n","Ziyaretci Fisi");
                lcd_printf(ALG_CENTER,szTmp);
                gtk_printf(label1, "%s\n%s","ZİYARETCİ FİŞİ", "İZİN VERİLMEDİ");
                lcd_flip();
                MSJ_Goster(MSJ_TIP_NK);
                //sleep(1);
                break;
            case -1:
                //izin verilmedi sunucu bağlantısı yok
                lcd_clean();
                gtk_clean();
                gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);

                sprintf(szTmp,"%s\n","Ziyaretci Fisi");
                lcd_printf(ALG_CENTER,szTmp);
                gtk_printf(label1, "%s\n%s","ZİYARETCİ FİŞİ", "SUNUCU BAĞLANTISI YOK");
                lcd_flip();
                MSJ_Goster(MSJ_TIP_NK);
                break;
            }

            //PERSONEL_Lcd();
            //Log göndermem istenmiyor.
            //PERSONEL_Log(Okuyucu);
            free(tcp_Resp);
            free(szTmp);
            //free(szBuf);
            //free(strRcv);
            return;
        }
    }

    if (rec_HAVUZ.AKTIF)
    {
        HAVUZ(Okuyucu);
        free(tcp_Resp);
        free(szTmp);
        //free(szBuf);
        //free(strRcv);
        return;
    }

    if (PERSONEL_KayipCalinti(rec_Personel.KARTNO)==1) //Kayip Calinti Ise
    {
        rec_LogKart.GECTI_ONLINE=0;
        rec_LogKart.GECTI=0;
        rec_LogKart.CEVAP_ONLINE=0;
        rec_LogKart.CEVAP=ic_KayipCalinti;
        MSJ_Goster(rec_LogKart.CEVAP);
        MSJ_Ses(rec_TERM.SES_TANIMSIZ);
    }
    else
    {
        //2.4.0
        //sadece yemekhane terminali ise yap,
        //kapı vs modunda iken boşuna uğraşma.
        if(rec_TERM.TERM_TIP == TERM_YEMEKHANE)
        {
            res_TopUp = PERSONEL_TopUp(rec_Personel.KARTNO);

            switch(res_TopUp)
            {
            case 1: //TopUp_NoFile:
                break;
            case 2: //TopUp_NoCard:
                break;
            case 3: //TopUp_OK:
                lcd_clean();
                lcd_printf(ALG_CENTER, "KART BILGILERINIZ");
                lcd_printf(ALG_CENTER, "GUNCELLENDI");
                lcd_printf(ALG_CENTER, "TEKRAR OKUTUNUZ");
                lcd_flip();

                gtk_clean();
                gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);
                bg_color.alpha = 0.5;
                bg_color.blue = 0;
                bg_color.green = 1;
                bg_color.red = 0;
                gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
                GdkRGBA font_color;
                font_color.alpha = 1;
                font_color.blue = 0;
                font_color.green = 1;
                font_color.red = 0;
                gtk_widget_override_color(label1, GTK_STATE_FLAG_NORMAL, &font_color);
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12
                gtk_printf(label1, "KART BİLGİLERİNİZ\nGÜNCELLENDİ\nTEKRAR OKUTUNUZ");
                gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_QUESTION);//GTK_MESSAGE_ERROR
                while(gtk_events_pending())
                    gtk_main_iteration();
                MSJ_Ses(rec_TERM.SES_OKUMA_OK);
                MSJ_Ses(rec_TERM.SES_OKUMA_OK);

                sleep(2);
                MSJ_Goster(MSJ_TIP_LG);
                free(tcp_Resp);
                free(szTmp);
                //free(szBuf);
                //free(strRcv);
                return;
            //break;
            case 4: //TopUp_NOK:
                lcd_clean();
                lcd_printf(ALG_CENTER, "KART BILGILERINIZ");
                lcd_printf(ALG_CENTER, "GUNCELLENEMEDI");
                lcd_printf(ALG_CENTER, "TEKRAR OKUTUNUZ");
                lcd_flip();
                gtk_clean();
                gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);
                font_color.alpha = 1;
                font_color.blue = 1;
                font_color.green = 0;
                font_color.red = 0;
                gtk_widget_override_color(label1, GTK_STATE_FLAG_NORMAL, &font_color);
                gtk_printf(label1, "KART BİLGİLERİNİZ\nGÜNCELLENEMEDİ\nTEKRAR OKUTUNUZ");

                gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_ERROR);//GTK_MESSAGE_ERROR
                while(gtk_events_pending())
                    gtk_main_iteration();
                for (j=0; j<4; j++)
                    MSJ_Ses(rec_TERM.SES_OKUMA_NK);
                for (j=0; j<3; j++)
                {
                    sleep(1);
                    MSJ_Ses(rec_TERM.SES_OKUMA_NK);
                }

                MSJ_Goster(MSJ_TIP_LG);
                free(szTmp);
                free(tcp_Resp);
                //free(szBuf);
                //free(strRcv);
                return;
                //break;
            }
        }

        if (rec_TERM.TERM_TIP == TERM_KAPI)
        {
            //KAPI

            //1.9.5
            //retval = PERSONEL_IzinAl(rec_Personel.KARTNO_HEX, Okuyucu);
            //2.1.0
            //retval = PERSONEL_IzinAl(rec_Personel.KARTNO_HEX, Okuyucu-1);
            if(Okuyucu<3)//1-2 kart
                retval = PERSONEL_IzinAl(rec_Personel.KARTNO_HEX, Okuyucu-1);
            else if(Okuyucu == 3) //barcode
            {
                //2.3.2
                if(ISKUR)
                {
                    yemek_sayisi = (int) ini_getl(Barcode_Fis, "YEMEKSAYISI", -1, ISKUR_INI);
                    if(yemek_sayisi == -1)
                    {
                        yemek_sayisi = 0;
                        retval = PERSONEL_IzinAl(Barcode_Fis, Okuyucu-1);
                    }
                    else
                    {
                        yemek_sayisi = -1;
                        retval = -1;
                    }
                }
                else
                    retval = PERSONEL_IzinAl(Barcode_Fis, Okuyucu-1);
            }
            else if(Okuyucu == 4) //tckimlik
                retval = PERSONEL_IzinAl(TCKimlik, Okuyucu-1);
            /*
            if(Okuyucu == 4){//tckimlik
                if(ISKUR){
                    yemek_sayisi = (int) ini_getl(TCKimlik, "YEMEKSAYISI", -1, ISKUR_INI);
                    if(yemek_sayisi == -1){
                        yemek_sayisi = 0;
                        retval = PERSONEL_IzinAl(TCKimlik, Okuyucu-1);
                    }
                    else{
                        yemek_sayisi = -1;
                        retval = -1;
                    }
                }else
                retval = PERSONEL_IzinAl(TCKimlik, Okuyucu-1);
            }
            */
            //brk
            //unutma kaldır
            //retval = 1;
            switch (retval)
            {
            case 0:
                //izin verildi turnikeyi aç
                lcd_clean();
                //gtk_clean();
                //bg_color.alpha = 0.5;
                //bg_color.blue = 0;
                //bg_color.green = 1;
                //bg_color.red = 0;
                //gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
                //gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12
                //gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);

                sprintf(szTmp,"%s\n%s\n%s %s",rec_Personel.KARTNO_HEX, rec_Personel.ADI, rec_Personel.SOYAD, "Izin Verildi");
                lcd_printf(ALG_CENTER,szTmp);
                //gtk_printf(label1, "%s\n%s %s\n%s",rec_Personel.KARTNO_HEX, rec_Personel.ADI, rec_Personel.SOYAD, "İZİN VERİLDİ");
                gtk_label_set_text(GTK_LABEL(label_status), "                   İZİN VERİLDİ    ");
                gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_QUESTION);//GTK_MESSAGE_ERROR
                lcd_flip();
                //1.9.5
                while(gtk_events_pending())
                    gtk_main_iteration();
                //MSJ_Ses(rec_TERM.SES_IZIN_OK);
                MSJ_OK=0x00;
                //2.0.1                    /
                if((Okuyucu_TIP[SMM_PORT] == 2)||(Okuyucu_TIP[SMM_PORT] == 3))
                {
                    CN56B_Buzzer(ifd[SMM_PORT]);
                    CN56B_Led(ifd[SMM_PORT]);
                }
                //brk
                //2.1.0
                //burada sense bilgisine göre geçip geçmediğine bakılacak
                //personel_gectimi düzgün değerlendirilsin

                //MSJ_Goster(MSJ_TIP_OK);
                //Yemekhane iken hep 1. okuyucu gibi işlem yap
                //2.1.2
                //işkur için barkod okuyucu ise 1. okuyucu gibi davranması sağlandı
                //rec_LogKart.GECTI = PERSONEL_Gectimi(Okuyucu);
                //2.1.3
                //if(ISKUR && (Okuyucu == 3))
                //iskur icin barkod ve tckimlik ise 1. okuyucu gibi davranacak.
                if(ISKUR && ((Okuyucu == 3) ||(Okuyucu == 4)))
                    rec_LogKart.GECTI = PERSONEL_Gectimi(1);
                else
                    rec_LogKart.GECTI = PERSONEL_Gectimi(Okuyucu);
                //brk unutma kaldırmayı
                //rec_LogKart.GECTI = 1;
                //2.1.0
                if(rec_LogKart.GECTI)
                {
                    //rec_LogKart.GECTI_ONLINE=1;//hareket işle mi olacak acaba burada?
                    if(Okuyucu<3) //kart okutuldu
                    {
                        rec_LogKart.GECTI_ONLINE = PDKS_HareketIsle(rec_Personel.KARTNO_HEX, Okuyucu-1);
                        PDKS_AddLog(rec_Personel.KARTNO_HEX, rec_LogKart.GECTI_ONLINE, Okuyucu-1);//0 offline, 1 online 0 giris, 1 çıkış

                        //2.1.4
                        if(ISKUR)
                        {
                            //brk
                            //burada iskur.ini içinde guncelleme yapılması lazım.
                            //kartno ile aranacak, yemeksayısı guncellenecek, tckimlik bulunup, orası da guncellenecek.
                            //iskur.ini'de misafir yemek fiyatı olması gerekmiyor mu???
                            yemek_sayisi = (int) ini_getl(rec_Personel.KARTNO_HEX, "YEMEKSAYISI", 0, ISKUR_INI) + 1;
                            ini_putl(rec_Personel.KARTNO_HEX, "YEMEKSAYISI", yemek_sayisi, ISKUR_INI);

                            //kart sahibinin tckimliğini bul ve o tckimliğin yemek sayısını da guncelle
                            ini_gets(rec_Personel.KARTNO_HEX, "TCKIMLIK", "", TCKimlik, sizearray(TCKimlik), ISKUR_INI);
                            if(strlen(TCKimlik) == 11)
                                ini_putl(TCKimlik, "YEMEKSAYISI", yemek_sayisi, ISKUR_INI);
                        }
                    }
                    else if(Okuyucu == 4) //keypad'den girildi
                    {
                        //Okuyucu = 3; //Okuyucu 3 ise TCKimlik
                        rec_LogKart.GECTI_ONLINE = PDKS_HareketIsle(TCKimlik, Okuyucu);
                        PDKS_AddLog(TCKimlik, rec_LogKart.GECTI_ONLINE, Okuyucu-1);//0 offline, 1 online 0 giris, 1 çıkış
                        //2.1.4
                        if(ISKUR)
                        {
                            //brk
                            //burada iskur.ini içinde guncelleme yapılması lazım.
                            //yukardakinin tersi yapılacak.
                            //iskur.ini'de misafir yemek fiyatı olması gerekmiyor mu???
                            yemek_sayisi = (int) ini_getl(TCKimlik, "YEMEKSAYISI", 0, ISKUR_INI) + 1;
                            ini_putl(TCKimlik, "YEMEKSAYISI", yemek_sayisi, ISKUR_INI);

                            //tckimliğin kart numarasını bul ve o kartın da yemek sayısını guncelle
                            ini_gets(TCKimlik, "KARTNO", "", rec_Personel.KARTNO_HEX, sizearray(rec_Personel.KARTNO_HEX), ISKUR_INI);
                            if(strlen(rec_Personel.KARTNO_HEX) > 0)
                                ini_putl(rec_Personel.KARTNO_HEX, "YEMEKSAYISI", yemek_sayisi, ISKUR_INI);
                        }

                    }
                    else if(Okuyucu == 3) //barcode
                    {
                        rec_LogKart.GECTI_ONLINE = PDKS_HareketIsle(Barcode_Fis, Okuyucu);
                        PDKS_AddLog(Barcode_Fis, rec_LogKart.GECTI_ONLINE, Okuyucu-1);//0 offline, 1 online 0 giris, 1 çıkış
                    }
                }
                //2.1.0
                //MSJ_Ses(rec_TERM.SES_OKUMA_OK);
                //1.7.0
                //artık sadece izin verildiğinde loga kaydediyor
                //PDKS_AddLog(rec_LogKart.GECTI_ONLINE, Okuyucu-1);//0 offline, 1 online 0 giris, 1 çıkış
                time(&rawtime);
                timeinfo = localtime(&rawtime);
                lcd_clean();
                MSJ_Goster(MSJ_TIP_LG);
                lcd_printf_ex(ALG_CENTER, 16," %02d/%02d/%04d, %02d:%02d", timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min); //Her Dakika Zaman Goster
                lcd_printf_ex(ALG_CENTER, 19, "www.utarit.com 2017");
                lcd_flip();

                gtk_printf(label_status, "%02d/%02d/%04d       www.utarit.com ©2017       %02d:%02d", timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min);
                gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_WARNING);//GTK_MESSAGE_ERROR
                while(gtk_events_pending())
                    gtk_main_iteration();
                break;
            case 1:
                //izin verilmedi
                lcd_clean();
                lcd_flip();
                //2.1.1
                /*
                gtk_clean();
                bg_color.alpha = 1;
                bg_color.blue = 0;
                bg_color.green = 0;
                bg_color.red = 1;
                gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12
                gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);

                sprintf(szTmp,"%s\n%s\n%s %s",rec_Personel.KARTNO_HEX, rec_Personel.ADI, rec_Personel.SOYAD, "Izin Verilmedi");
                lcd_printf(ALG_CENTER,szTmp);
                gtk_printf(label1, "%s\n%s %s\n%s",rec_Personel.KARTNO_HEX, rec_Personel.ADI, rec_Personel.SOYAD, "İZİN VERİLMEDİ");
                gtk_label_set_text(GTK_LABEL(label_status), "                   İZİN VERİLMEDİ");

                gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_ERROR);//GTK_MESSAGE_ERROR
                lcd_flip();
                //1.9.5
                while(gtk_events_pending())
                    gtk_main_iteration();
                */
                sleep(rec_TERM.MSJ_SURE_SN);
                time(&rawtime);
                timeinfo = localtime(&rawtime);
                lcd_clean();
                MSJ_Goster(MSJ_TIP_LG);
                lcd_printf_ex(ALG_CENTER, 16," %02d/%02d/%04d, %02d:%02d", timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min); //Her Dakika Zaman Goster
                lcd_printf_ex(ALG_CENTER, 19, "www.utarit.com 2017");
                lcd_flip();

                gtk_printf(label_status, "%02d/%02d/%04d       www.utarit.com ©2017       %02d:%02d", timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min);
                gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_WARNING);//GTK_MESSAGE_ERROR
                while(gtk_events_pending())
                    gtk_main_iteration();
                //MSJ_Goster(MSJ_TIP_NK);
                //sleep(1);
                //1.8.1
                //MSJ_Ses(rec_TERM.SES_TANIMSIZ);
                break;
            //1.7.0
            case 0x33:
                //hatalı şifre, izin verilmedi
                lcd_clean();
                gtk_clean();
                gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);

                sprintf(szTmp,"%s\n%s\n%s %s",rec_Personel.KARTNO_HEX, rec_Personel.ADI, rec_Personel.SOYAD, "Izin Verilmedi");
                lcd_printf(ALG_CENTER,szTmp);
                gtk_printf(label1, "%s\n%s %s\n%s",rec_Personel.KARTNO_HEX, rec_Personel.ADI, rec_Personel.SOYAD, "İZİN VERİLMEDİ");
                gtk_label_set_text(GTK_LABEL(label_status), "                   ŞİFRE HATASI");
                gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_ERROR);//GTK_MESSAGE_ERROR
                lcd_flip();
                MSJ_Goster(MSJ_TIP_NK);
                //sleep(1);
                //1.8.1
                MSJ_Ses(rec_TERM.SES_TANIMSIZ);
                break;
            case -1:
                //izin verildi ama sunucu bağlantısı yok
                //brk
                //burada ini dosyasından aranacak,
                //ilk yemek ise devam edilecek, ikinci yemek ise sorulacak.
                lcd_clean();
                gtk_clean();
                bg_color.alpha = 0.5;
                bg_color.blue = 0;
                bg_color.green = 1;
                bg_color.red = 0;
                gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12
                gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);

                if(Okuyucu == 3) //barcode
                {
                    //2.1.4
                    if(ISKUR)
                    {
                        yemek_sayisi = (int) ini_getl(Barcode_Fis, "YEMEKSAYISI", -1, ISKUR_INI);
                        if(yemek_sayisi == -1)
                            yemek_sayisi = 0;
                        else
                            yemek_sayisi = -1;
                    }
                }
                else if(Okuyucu<3) //kart okutuldu
                {
                    //2.1.4
                    if(ISKUR)
                    {
                        yemek_sayisi = (int) ini_getl(rec_Personel.KARTNO_HEX, "YEMEKSAYISI", -1, ISKUR_INI);
                        ini_gets(rec_Personel.KARTNO_HEX, "AD", "", rec_Personel.ADI, sizearray(rec_Personel.ADI), ISKUR_INI);
                        ini_gets(rec_Personel.KARTNO_HEX, "SOYAD", "", rec_Personel.SOYAD, sizearray(rec_Personel.SOYAD), ISKUR_INI);
                    }
                    else//2.4.0
                    {
                        if(!PDKS_KartBul((char *)rec_Personel.KARTNO_HEX,&KART,0x00))
                        {
                            gtk_clean();
                            bg_color.alpha = 1;
                            bg_color.blue = 0;
                            bg_color.green = 0;
                            bg_color.red = 1;
                            gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
                            gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12
                            gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);
                            sprintf(szTmp,"%s\n%s\n%s %s",rec_Personel.KARTNO_HEX, rec_Personel.ADI, rec_Personel.SOYAD, "Tanimsiz Kart");
                            lcd_printf(ALG_CENTER,szTmp);
                            gtk_printf(label1, "%s\n%s %s\n%s",rec_Personel.KARTNO_HEX, rec_Personel.ADI, rec_Personel.SOYAD, "TANIMSIZ KART");
                            while(gtk_events_pending())
                                gtk_main_iteration();
                            MSJ_Ses(rec_TERM.SES_TANIMSIZ);
                            lcd_flip();
                            //2.2.6
                            while(gtk_events_pending())
                                gtk_main_iteration();

                            //sleep(rec_TERM.MSJ_SURE_SN);
                            time(&rawtime);
                            timeinfo = localtime(&rawtime);
                            lcd_clean();
                            MSJ_Goster(MSJ_TIP_LG);
                            lcd_printf_ex(ALG_CENTER, 16," %02d/%02d/%04d, %02d:%02d", timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min); //Her Dakika Zaman Goster
                            lcd_printf_ex(ALG_CENTER, 19, "www.utarit.com 2017");
                            lcd_flip();

                            gtk_printf(label_status, "%02d/%02d/%04d       www.utarit.com ©2017       %02d:%02d", timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min);
                            gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_WARNING);//GTK_MESSAGE_ERROR
                            while(gtk_events_pending())
                                gtk_main_iteration();
                            break;
                        }
                    }
                }
                else if(Okuyucu==4) //keypad'den girildi
                {
                    //Okuyucu = 3; //Okuyucu 3 ise TCKimlik
                    //2.1.4
                    if(ISKUR)
                    {
                        yemek_sayisi = (int) ini_getl(TCKimlik, "YEMEKSAYISI", -1, ISKUR_INI);
                        ini_gets(TCKimlik, "AD", "", rec_Personel.ADI, sizearray(rec_Personel.ADI), ISKUR_INI);
                        ini_gets(TCKimlik, "SOYAD", "", rec_Personel.SOYAD, sizearray(rec_Personel.SOYAD), ISKUR_INI);
                    }
                }
                //2.1.4
                //if(yemek_sayisi < 0){
                if(ISKUR && (yemek_sayisi < 0))
                {
                    gtk_clean();
                    bg_color.alpha = 1;
                    bg_color.blue = 0;
                    bg_color.green = 0;
                    bg_color.red = 1;
                    gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12
                    gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);
                    if(Okuyucu<3) //kart
                    {
                        sprintf(szTmp,"%s\n%s\n%s %s",rec_Personel.KARTNO_HEX, rec_Personel.ADI, rec_Personel.SOYAD, "Tanimsiz Kart");
                        lcd_printf(ALG_CENTER,szTmp);
                        gtk_printf(label1, "%s\n%s %s\n%s",rec_Personel.KARTNO_HEX, rec_Personel.ADI, rec_Personel.SOYAD, "TANIMSIZ KART");
                    }
                    else if(Okuyucu==4) //keypad
                    {
                        sprintf(szTmp,"%s\n%s",TCKimlik, "Tanimsiz T.C. KIMLIK NUMARASI");
                        lcd_printf(ALG_CENTER,szTmp);
                        gtk_printf(label1, "%s\n%s",TCKimlik, "TANIMSIZ T.C. KİMLİK NUMARASI");
                    }
                    if(Okuyucu==3) //barkod
                    {
                        sprintf(szTmp,"%s\n%s",Barcode_Fis, "Daha Once Kullanilmis");
                        lcd_printf(ALG_CENTER,szTmp);
                        gtk_printf(label1, "%s\n%s",Barcode_Fis, "DAHA ÖNCE KULLANILMIŞ");
                    }
                    gtk_label_set_text(GTK_LABEL(label_status), "                   İZİN VERİLMEDİ          ");
                    gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_QUESTION);//GTK_MESSAGE_ERROR
                    while(gtk_events_pending())
                        gtk_main_iteration();
                    MSJ_Ses(rec_TERM.SES_TANIMSIZ);
                    lcd_flip();
                    //2.2.6
                    while(gtk_events_pending())
                        gtk_main_iteration();

                    sleep(rec_TERM.MSJ_SURE_SN);
                    time(&rawtime);
                    timeinfo = localtime(&rawtime);
                    lcd_clean();
                    MSJ_Goster(MSJ_TIP_LG);
                    lcd_printf_ex(ALG_CENTER, 16," %02d/%02d/%04d, %02d:%02d", timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min); //Her Dakika Zaman Goster
                    lcd_printf_ex(ALG_CENTER, 19, "www.utarit.com 2017");
                    lcd_flip();

                    gtk_printf(label_status, "%02d/%02d/%04d       www.utarit.com ©2017       %02d:%02d", timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min);
                    gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_WARNING);//GTK_MESSAGE_ERROR
                    while(gtk_events_pending())
                        gtk_main_iteration();
                    break;
                }
                else
                {
                    //2.1.4
                    if(ISKUR)
                    {
                        if(yemek_sayisi>0)
                        {
                            //retval=0;//izin var
                            MSJ_Ses(rec_TERM.SES_IZIN_NK);
                            bg_color.alpha = 1;
                            bg_color.blue = 0;
                            bg_color.green = 0;
                            bg_color.red = 1;
                            gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
                            gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12
                            gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);
                            gtk_printf(label1, "MİSAFİR FİYATI\nONAYLAMAK İÇİN ENT");
                            while(gtk_events_pending())
                                gtk_main_iteration();


                            key=KEYB_getch(5);
                            if(key != KEY_ENTR)
                            {
                                gtk_printf(label1, "RED EDİLDİ");
                                while(gtk_events_pending())
                                    gtk_main_iteration();
                                MSJ_Ses(rec_TERM.SES_TANIMSIZ);
                                time(&rawtime);
                                timeinfo = localtime(&rawtime);
                                lcd_clean();
                                MSJ_Goster(MSJ_TIP_LG);
                                lcd_printf_ex(ALG_CENTER, 16," %02d/%02d/%04d, %02d:%02d", timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min); //Her Dakika Zaman Goster
                                lcd_printf_ex(ALG_CENTER, 19, "www.utarit.com 2017");
                                lcd_flip();

                                gtk_printf(label_status, "%02d/%02d/%04d       www.utarit.com ©2017       %02d:%02d", timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min);
                                gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_WARNING);//GTK_MESSAGE_ERROR
                                while(gtk_events_pending())
                                    gtk_main_iteration();
                                break;//onay yok
                            }
                        }
                    }
                    if(Okuyucu<3)
                    {
                        sprintf(szTmp,"%s\n%s\n%s %s",rec_Personel.KARTNO_HEX, rec_Personel.ADI, rec_Personel.SOYAD, "Offline Izin Verildi");
                        lcd_printf(ALG_CENTER,szTmp);
                        gtk_printf(label1, "%s\n%s %s\n%s",rec_Personel.KARTNO_HEX, rec_Personel.ADI, rec_Personel.SOYAD, "OFFLINE İZİN VERİLDİ");
                        gtk_label_set_text(GTK_LABEL(label_status), "               OFFLINE İZİN VERİLDİ");
                        gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_QUESTION);//GTK_MESSAGE_ERROR
                    }
                    else if(Okuyucu==4)
                    {
                        sprintf(szTmp,"%s\n%s\n%s %s",TCKimlik, rec_Personel.ADI, rec_Personel.SOYAD, "Offline Izin Verildi");
                        lcd_printf(ALG_CENTER,szTmp);
                        gtk_printf(label1, "%s\n%s %s\n%s",TCKimlik, rec_Personel.ADI, rec_Personel.SOYAD, "OFFLINE İZİN VERİLDİ");
                        gtk_label_set_text(GTK_LABEL(label_status), "               OFFLINE İZİN VERİLDİ");
                        gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_QUESTION);//GTK_MESSAGE_ERROR
                    }
                    else if(Okuyucu==3)
                    {
                        sprintf(szTmp,"%s\n%s",Barcode_Fis, "Offline Izin Verildi");
                        lcd_printf(ALG_CENTER,szTmp);
                        gtk_printf(label1, "%s\n%s",Barcode_Fis, "OFFLINE İZİN VERİLDİ");
                        gtk_label_set_text(GTK_LABEL(label_status), "               OFFLINE İZİN VERİLDİ");
                        gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_QUESTION);//GTK_MESSAGE_ERROR
                    }
                    while(gtk_events_pending())
                        gtk_main_iteration();
                    MSJ_Ses(rec_TERM.SES_IZIN_OK);
                    //sleep(rec_TERM.MSJ_SURE_SN);
                    lcd_flip();
                }
                //1.8.1
                //MSJ_Goster(MSJ_TIP_OK);
                //rec_LogKart.GECTI = PERSONEL_Gectimi(Okuyucu);
                //2.1.3
                //iskur icin barkod ve tckimlik ise 1. okuyucu gibi davranacak.
                //rec_LogKart.GECTI = PERSONEL_Gectimi(Okuyucu);
                if(ISKUR && ((Okuyucu == 3) ||(Okuyucu == 4)))
                    rec_LogKart.GECTI = PERSONEL_Gectimi(1);
                else
                    rec_LogKart.GECTI = PERSONEL_Gectimi(Okuyucu);
                rec_LogKart.GECTI_ONLINE=0;//sunucu bağlantısı yok... hareket işle mi olacak acaba burada?
                //1.8.1
                //MSJ_Goster(MSJ_TIP_OK);
                //1.7.0
                //artık sadece izin verildiğinde loga kaydediyor
                //2.1.0
                //PDKS_AddLog(rec_LogKart.GECTI_ONLINE, Okuyucu-1);//0 offline, 1 online 0 giris, 1 çıkış


                //brk unutma kaldırmayı
                //rec_LogKart.GECTI = 1;
                //2.1.0
                if(rec_LogKart.GECTI)
                {
                    //rec_LogKart.GECTI_ONLINE=1;//hareket işle mi olacak acaba burada?
                    if(Okuyucu<3)
                    {
                        //sunucu bağlantısı yok
                        //rec_LogKart.GECTI_ONLINE = PDKS_HareketIsle(rec_Personel.KARTNO_HEX, Okuyucu-1);
                        PDKS_AddLog(rec_Personel.KARTNO_HEX, rec_LogKart.GECTI_ONLINE, Okuyucu-1);//0 offline, 1 online 0 giris, 1 çıkış
                        //2.1.4
                        if(ISKUR)
                        {
                            //yemek_sayisi = (int) ini_getl(rec_Personel.KARTNO_HEX, "YEMEKSAYISI", 0, ISKUR_INI) + 1;
                            ini_putl(rec_Personel.KARTNO_HEX, "YEMEKSAYISI", yemek_sayisi+1, ISKUR_INI);
                            //kart sahibinin tckimliğini bul ve o tckimliğin yemek sayısını da guncelle
                            ini_gets(rec_Personel.KARTNO_HEX, "TCKIMLIK", "", TCKimlik, sizearray(TCKimlik), ISKUR_INI);
                            if(strlen(TCKimlik) == 11)
                                ini_putl(TCKimlik, "YEMEKSAYISI", yemek_sayisi+1, ISKUR_INI);
                        }
                    }
                    else if(Okuyucu==4)
                    {
                        //Okuyucu = 3; //Okuyucu 3 ise TCKimlik
                        //sunucu bağlantısı yok
                        //rec_LogKart.GECTI_ONLINE = PDKS_HareketIsle(TCKimlik, Okuyucu);
                        PDKS_AddLog(TCKimlik, rec_LogKart.GECTI_ONLINE, Okuyucu-1);//0 offline, 1 online 0 giris, 1 çıkış
                        //2.1.4
                        if(ISKUR)
                        {
                            //yemek_sayisi = (int) ini_getl(TCKimlik, "YEMEKSAYISI", 0, ISKUR_INI) + 1;
                            ini_putl(TCKimlik, "YEMEKSAYISI", yemek_sayisi+1, ISKUR_INI);
                            //tckimliğin kart numarasını bul ve o kartın da yemek sayısını guncelle
                            ini_gets(TCKimlik, "KARTNO", "", rec_Personel.KARTNO_HEX, sizearray(rec_Personel.KARTNO_HEX), ISKUR_INI);
                            if(strlen(rec_Personel.KARTNO_HEX) > 0)
                                ini_putl(rec_Personel.KARTNO_HEX, "YEMEKSAYISI", yemek_sayisi, ISKUR_INI);
                        }
                    }
                    if(Okuyucu==3) //barcode
                    {
                        //Okuyucu = 3; //Okuyucu 3 ise TCKimlik
                        //sunucu bağlantısı yok
                        //rec_LogKart.GECTI_ONLINE = PDKS_HareketIsle(TCKimlik, Okuyucu);
                        PDKS_AddLog(Barcode_Fis, rec_LogKart.GECTI_ONLINE, Okuyucu-1);//0 offline, 1 online 0 giris, 1 çıkış
                        //2.1.4
                        if(ISKUR)
                        {
                            //yemek_sayisi = (int) ini_getl(TCKimlik, "YEMEKSAYISI", 0, ISKUR_INI) + 1;
                            ini_putl(Barcode_Fis, "YEMEKSAYISI", yemek_sayisi+1, ISKUR_INI);
                        }
                    }
                }
                time(&rawtime);
                timeinfo = localtime(&rawtime);
                lcd_clean();
                MSJ_Goster(MSJ_TIP_LG);
                lcd_printf_ex(ALG_CENTER, 16," %02d/%02d/%04d, %02d:%02d", timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min); //Her Dakika Zaman Goster
                lcd_printf_ex(ALG_CENTER, 19, "www.utarit.com 2017");
                lcd_flip();

                gtk_printf(label_status, "%02d/%02d/%04d       www.utarit.com ©2017       %02d:%02d", timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min);
                gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_WARNING);//GTK_MESSAGE_ERROR
                while(gtk_events_pending())
                    gtk_main_iteration();
                break;
            }
            /*
            			rec_LogKart.CEVAP = PERSONEL_KapidanGecermi();
            			MSJ_Goster(rec_LogKart.CEVAP);
            			if (rec_LogKart.CEVAP==ic_Izin_Tamam)
            			{
            				rec_LogKart.GECTI = PERSONEL_Gectimi(Okuyucu);
            				if (rec_LogKart.GECTI)
            				{
            					rec_LogKart.GECTI_ONLINE=PERSONEL_HareketIsle();
            				}
            			}
            			PERSONEL_Lcd();
            			*/
            //PERSONEL_Log(Okuyucu);
            //1.7.0
            //burada izin verilse de verilmese de loga kaydediyordu o yüzden kaldırılıp,
            //yukarıda sadece izin verildiği durumlara eklendi
            //PDKS_AddLog(rec_LogKart.GECTI_ONLINE, Okuyucu-1);//0 offline, 1 online 0 giris, 1 çıkış

            //PERSONEL_Log(1);
        }
        else if (rec_TERM.TERM_TIP == TERM_HARCAMA)
        {
            gtk_printf(label1, "\nBU BİR SOLI-TERM'DİR\nHARCAMA İŞLEMLERİ\nSOLI-POS İLE YAPILMALIDIR!\n");
            bg_color.alpha = 1;
            bg_color.blue = 0;
            bg_color.green = 0;
            bg_color.red = 1;
            gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
            gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12
            gtk_label_set_text(GTK_LABEL(label_status), "   HARCAMA İŞLEMLERİ SOLI-POS İLE YAPILMALIDIR!");
            gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_ERROR);//GTK_MESSAGE_ERROR
            while(gtk_events_pending())
                gtk_main_iteration();
            MSJ_Ses(rec_TERM.SES_OKUMA_NK);
            MSJ_Ses(rec_TERM.SES_OKUMA_NK);
            MSJ_Ses(rec_TERM.SES_OKUMA_NK);
            MSJ_Ses(rec_TERM.SES_OKUMA_NK);

            sleep(1);
            time(&rawtime);
            timeinfo = localtime(&rawtime);
            lcd_clean();
            MSJ_Goster(MSJ_TIP_LG);
            lcd_printf_ex(ALG_CENTER, 16," %02d/%02d/%04d, %02d:%02d", timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min); //Her Dakika Zaman Goster
            lcd_printf_ex(ALG_CENTER, 19, "www.utarit.com 2017");
            lcd_flip();

            gtk_printf(label_status, "%02d/%02d/%04d       www.utarit.com ©2017       %02d:%02d", timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min);
            gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_WARNING);//GTK_MESSAGE_ERROR
            while(gtk_events_pending())
                gtk_main_iteration();


        }
        else if(rec_TERM.TERM_TIP == TERM_PARA)
        {
            // hem para duser, hem para yukler.
            gtk_printf(label1, "\nBU BİR SOLI-TERM'DİR\nPARA İŞLEMLERİ\nSOLI-POS İLE YAPILMALIDIR!\n");
            bg_color.alpha = 1;
            bg_color.blue = 0;
            bg_color.green = 0;
            bg_color.red = 1;
            gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
            gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12
            gtk_label_set_text(GTK_LABEL(label_status), "    PARA İŞLEMLERİ SOLI-POS İLE YAPILMALIDIR!");
            gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_ERROR);//GTK_MESSAGE_ERROR
            while(gtk_events_pending())
                gtk_main_iteration();
            MSJ_Ses(rec_TERM.SES_OKUMA_NK);
            MSJ_Ses(rec_TERM.SES_OKUMA_NK);
            MSJ_Ses(rec_TERM.SES_OKUMA_NK);
            MSJ_Ses(rec_TERM.SES_OKUMA_NK);

            sleep(1);
            time(&rawtime);
            timeinfo = localtime(&rawtime);
            lcd_clean();
            MSJ_Goster(MSJ_TIP_LG);
            lcd_printf_ex(ALG_CENTER, 16," %02d/%02d/%04d, %02d:%02d", timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min); //Her Dakika Zaman Goster
            lcd_printf_ex(ALG_CENTER, 19, "www.utarit.com 2017");
            lcd_flip();

            gtk_printf(label_status, "%02d/%02d/%04d       www.utarit.com ©2017       %02d:%02d", timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min);
            gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_WARNING);//GTK_MESSAGE_ERROR
            while(gtk_events_pending())
                gtk_main_iteration();

        }
        else if (rec_TERM.TERM_TIP == TERM_YEMEKHANE)
        {
            //YEMEKHANE
            yCevap=0x00;
            MSJ_OK=0;
            z_MSJ_SURE_SN=0;
            rec_LogKart.CEVAP_ONLINE=0;
            if ((Hata.HATA)&&(strcmp(Hata.KARTNO,rec_Personel.KARTNO_HEX)==0)) //Son Yazilan Hatali Kartsa
            {
                rec_LogKart.CEVAP=ic_Izin_Tamam;
                rec_Personel.HARCANAN = Hata.HARCAMA;
                if (!Hata.OK_B) //Bakiye Yazilamamis Gozukuyor
                {
                    if (rec_Personel.BAKIYE<Hata.BAKIYE) //Ama Yazilmis
                    {
                        Hata.OK_B=1;
                        rec_Personel.BAKIYE=Hata.BAKIYE; //Gosterim duzgun olsun
                    }
                }
                else
                {
                    rec_Personel.BAKIYE=Hata.BAKIYE;
                }
            }
            else
            {
                memset(&Hata,0,sizeof(Hata)); //Hatayi Bosalt
                rec_LogKart.CEVAP=PERSONEL_YemekYermi();
            }

            if (rec_LogKart.CEVAP == ic_Izin_Tamam)
            {
                if (rec_Personel.BAKIYE > 20000)
                {
                    PERSONEL_Lcd();
                    MSJ_Goster(rec_LogKart.CEVAP);
                    lcd_printf(ALG_CENTER,"IZIN VERILMEDI !");
                    lcd_printf(ALG_CENTER,"KART BOZULMUS !");
                    lcd_flip();
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

                    gtk_printf(label1, "%s\nİZİN VERİLMEDİ !\nKART BOZULMUS !", gtk_label_get_label(GTK_LABEL(label1)));
                    gtk_label_set_text(GTK_LABEL(label_status), "         İZİN VERİLMEDİ, KART BOZULMUŞ");
                    gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_ERROR);//GTK_MESSAGE_ERROR
                    while(gtk_events_pending())
                        gtk_main_iteration();
                    MSJ_Ses(rec_TERM.SES_OKUMA_NK);
                    MSJ_Ses(rec_TERM.SES_OKUMA_NK);
                    MSJ_Ses(rec_TERM.SES_OKUMA_NK);
                    MSJ_Ses(rec_TERM.SES_OKUMA_NK);

                    free(szTmp);
                    free(tcp_Resp);
                    //free(szBuf);
                    //free(strRcv);
                    return;
                }

                //7.6.0
                if(rec_LIMIT.AKTIF)
                {
                    if((rec_LIMIT.MISAFIR >0)&&(rec_LIMIT.MISAFIR <= rec_Personel.YB.SAYI))
                    {
                        PERSONEL_Lcd();
                        lcd_printf(ALG_CENTER, "MISAFIR FIYATINI");
                        lcd_printf(ALG_CENTER, "ONAYLIYOR MUSUNUZ?");
                        lcd_printf(ALG_CENTER, "BKSP:HAYIR  EVET:ENT");
                        lcd_flip();
                        bg_color.alpha = 1;
                        bg_color.blue = 0;
                        bg_color.green = 1;
                        bg_color.red = 0;
                        font_color.alpha = 1;
                        font_color.blue = .7;
                        font_color.green = .01;
                        font_color.red = 0.1;
                        gtk_widget_override_color(label1, GTK_STATE_FLAG_NORMAL, &font_color);
                        gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
                        gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12
                        //gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_ERROR);//GTK_MESSAGE_ERROR

                        gtk_printf(label1, "%s\nMİSAFİR FİYATINI ONAYLIYOR MUSUNUZ?\nBKSP:HAYIR  EVET:ENT", gtk_label_get_label(GTK_LABEL(label1)));
                        gtk_label_set_text(GTK_LABEL(label_status), "              BKSP:HAYIR  EVET:ENT!");
                        while(gtk_events_pending())
                            gtk_main_iteration();

                        //2.2.5
                        //timer5sn = Timer_Set(5000);
                        //while(!Timer_End(timer5sn)){
                        timer5sn = Set_Timer(5, 0);
                        while(!Check_Timer(timer5sn))
                        {
                            key=KEYB_pressup();
                            if(key != -1)
                            {
                                if(key==KEY_ENTR)
                                {
                                    //yemek yedir
                                    lcd_clean();
                                    lcd_printf_ex(ALG_CENTER, 5,"KARTINIZI OKUTUNUZ!");
                                    lcd_flip();
                                    bg_color.alpha = 1;
                                    bg_color.blue = 0;
                                    bg_color.green = 1;
                                    bg_color.red = 0;
                                    font_color.alpha = 1;
                                    font_color.blue = .7;
                                    font_color.green = .01;
                                    font_color.red = 0.1;
                                    gtk_widget_override_color(label1, GTK_STATE_FLAG_NORMAL, &font_color);
                                    gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
                                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12
                                    gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_ERROR);//GTK_MESSAGE_ERROR

                                    gtk_printf(label1, "\nKARTINIZI OKUTUNUZ!\n");
                                    gtk_label_set_text(GTK_LABEL(label_status), "               KARTINIZI OKUTUNUZ!");
                                    gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_ERROR);//GTK_MESSAGE_ERROR
                                    while(gtk_events_pending())
                                        gtk_main_iteration();

                                    MSJ_Ses(rec_TERM.SES_OKUMA_NK);
                                    MSJ_Ses(rec_TERM.SES_OKUMA_NK);
                                    MSJ_Ses(rec_TERM.SES_OKUMA_NK);
                                    //burada asagidaki yazma kism yapilacak
                                    //3-5 sn kart okutulmasini bekleyip okutulduysa
                                    //ve ayni kart ise asagi devam et.
                                    //2.2.5
                                    //timer55sn = Timer_Set(5000);
                                    timer55sn = Set_Timer(5, 0);
                                    memcpy(&onceki_kart, &rec_Personel, sizeof(struct_PERSONEL));
                                    key = KEY_BKSP;
                                    //2.2.5
                                    //while(!Timer_End(timer55sn)){
                                    while(!Check_Timer(timer55sn))
                                    {
                                        if(READER_Read())
                                        {
                                            if(strcmp(rec_Personel.KARTNO_HEX, onceki_kart.KARTNO_HEX) == 0)
                                            {
                                                memcpy(&rec_Personel, &onceki_kart, sizeof(struct_PERSONEL));
                                                key = KEY_ENTR;
                                                break;
                                            }
                                            else
                                                break;
                                        }
                                    }
                                    break;
                                }
                                else if(key==KEY_BKSP)
                                {
                                    //MSJ_Goster(MSJ_TIP_NK);
                                    lcd_printf_ex(ALG_CENTER, 5,"ONAY VERILMEDI !");
                                    lcd_flip();


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

                                    gtk_printf(label1, "\nMİSAFİR FİYATINA\nONAY VERİLMEDİ\n");
                                    gtk_label_set_text(GTK_LABEL(label_status), "                  ONAY VERİLMEDİ!");
                                    gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_ERROR);//GTK_MESSAGE_ERROR
                                    while(gtk_events_pending())
                                        gtk_main_iteration();

                                    MSJ_Ses(rec_TERM.SES_OKUMA_NK);
                                    MSJ_Ses(rec_TERM.SES_OKUMA_NK);
                                    MSJ_Ses(rec_TERM.SES_OKUMA_NK);
                                    MSJ_Ses(rec_TERM.SES_OKUMA_NK);
                                    free(szTmp);
                                    return;
                                }
                            }
                        }
                        if(key != KEY_ENTR)
                        {
                            //MSJ_Goster(MSJ_TIP_NK);
                            lcd_printf_ex(ALG_CENTER, 5,"ONAY VERILMEDI !");
                            lcd_flip();
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

                            gtk_printf(label1, "\nMİSAFİR FİYATINA\nONAY VERİLMEDİ\n");
                            gtk_label_set_text(GTK_LABEL(label_status), "                  ONAY VERİLMEDİ!");
                            gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_ERROR);//GTK_MESSAGE_ERROR
                            while(gtk_events_pending())
                                gtk_main_iteration();

                            MSJ_Ses(rec_TERM.SES_OKUMA_NK);
                            MSJ_Ses(rec_TERM.SES_OKUMA_NK);
                            MSJ_Ses(rec_TERM.SES_OKUMA_NK);
                            MSJ_Ses(rec_TERM.SES_OKUMA_NK);
                            free(szTmp);
                            return;
                        }
                    }
                }

                if (READER_Write(Okuyucu-1))
                {
                    memset(&Hata,0,sizeof(Hata)); //Hatayi Bosalt

                    PERSONEL_Lcd();

                    bg_color.alpha = 0.5;
                    bg_color.blue = 0;
                    bg_color.green = 1;
                    bg_color.red = 0;
                    gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12

                    sprintf(szTmp,"YEMEK : %4lu%c%02lu TL",(unsigned long) (rec_Personel.HARCANAN/100),'.',(unsigned long)rec_Personel.HARCANAN%100);
                    lcd_printf(ALG_CENTER,szTmp);//odroid left idi
                    gtk_printf(label1, "%s\n%s", gtk_label_get_label(GTK_LABEL(label1)), szTmp);
                    rec_Personel.BAKIYE = rec_Personel.BAKIYE - rec_Personel.HARCANAN;
                    sprintf(szTmp,"KALAN : %4lu%c%02lu TL",(unsigned long) (rec_Personel.BAKIYE/100),'.',(unsigned long)rec_Personel.BAKIYE%100);
                    lcd_printf(ALG_CENTER,szTmp);
                    gtk_printf(label1, "%s\n%s", gtk_label_get_label(GTK_LABEL(label1)), szTmp);
                    gtk_label_set_text(GTK_LABEL(label_status), "            İZİN VERİLDİ, AFİYET OLSUN");
                    gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_QUESTION);//GTK_MESSAGE_ERROR
                    lcd_printf(ALG_CENTER,"IZIN VERILDI");
                    lcd_printf(ALG_CENTER,"AFIYET OLSUN");
                    lcd_flip();
                    //şırnak için yukarı taşındı
                    SAYAC_Arti();
                    while(gtk_events_pending())
                        gtk_main_iteration();
                    if(rec_YAZICI.AKTIF)
                    {
                        //1.4.2
                        /*
                        if(rec_YAZICI.TIP)
                        	YAZICI_FisYemekhaneEx();
                        else
                        	YAZICI_FisYemekhane();
                        */
                        switch (rec_YAZICI.TIP)
                        {
                        case 1://Custom
                            YAZICI_FisYemekhane_Custom();
                            break;
                        case 2://Bixolon
                            YAZICI_FisYemekhane_Bixolon();
                            break;
                        case 3://Custom Bilecik
                            YAZICI_FisYemekhane_Custom_Bilecik();
                            break;
                        }
                    }

                    rec_LogKart.GECTI = PERSONEL_Gectimi(Okuyucu);
                    //if (rec_LogKart.GECTI) gecsede gecmesede
                    //{
                    rec_Personel.YB.SAYI=rec_LogKart.GECTI;
                    rec_LogKart.GECTI_ONLINE=PERSONEL_HareketIsle();
                    //SAYAC_Arti();
                    PERSONEL_Log(Okuyucu);
                    //}
                }
                else
                {

                    Hata.HATA=1;
                    strcpy(Hata.KARTNO,rec_Personel.KARTNO_HEX);
                    Hata.BAKIYE = rec_Personel.BAKIYE;
                    Hata.HARCAMA = rec_Personel.HARCANAN;
                    memcpy(&Hata.YB,&rec_Personel.YB,sizeof(struct_YEMEKBILGI));

                    rec_LogKart.CEVAP = ic_Izin_Yok;
                    PERSONEL_Lcd();
                    lcd_printf(ALG_CENTER,"YAZMA HATASI");//RESOURCE_ReadLine(400));// "KART OKUMA HATASI"
                    lcd_printf(ALG_CENTER,"KARTI OKUYUCUYA" );//RESOURCE_ReadLine(400));// "KART OKUMA HATASI"
                    lcd_printf(ALG_CENTER,"YAKIN TUTUNUZ" );//RESOURCE_ReadLine(400));// "KART OKUMA HATASI"
                    lcd_printf(ALG_CENTER,"3 SN SONRA OKUR" );//RESOURCE_ReadLine(400));// "KART OKUMA HATASI"
                    lcd_flip();

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
                    gtk_printf(label1, "%s\nYAZMA HATASI KARTI OKUYUCUYA\nYAKIN TUTUNUZ 3 SN SONRA OKUR", gtk_label_get_label(GTK_LABEL(label1)), szTmp);
                    gtk_label_set_text(GTK_LABEL(label_status), "           İZİN VERİLMEDİ, YAZMA HATASI");

                    while(gtk_events_pending())
                        gtk_main_iteration();
                    for (j=0; j<4; j++)
                    {
                        MSJ_Ses(rec_TERM.SES_OKUMA_NK);
                        sleep(1);
                    }
                    //1.8.1
                    /*
                    sleep(1);
                    for (j=0;j<3;j++)
                    {
                    	//sleep(1);
                    	MSJ_Ses(rec_TERM.SES_OKUMA_NK);
                    }*/
                    lcd_clean();
                    MSJ_OK = 0;
                    z_MSJ_SURE_SN = 0;
                }
            }
            else
            {
                PERSONEL_Lcd();
                MSJ_Goster(rec_LogKart.CEVAP);

                //lcd_printf_ex(ALG_CENTER, 160, "IZIN VERILMEDI !");
                //lcd_flip();
                //1.6.0
                //sebebi hemen göstersin diye aşağı alındı
                //MSJ_Ses(rec_TERM.SES_OKUMA_NK);
                //MSJ_Ses(rec_TERM.SES_OKUMA_NK);
            }
            if(yCevap)
            {
                font_color.alpha = 1;
                font_color.blue = .7;
                font_color.green = .01;
                font_color.red = 0.1;
                gtk_widget_override_color(label1, GTK_STATE_FLAG_NORMAL, &font_color);
                bg_color.alpha = 1;
                bg_color.blue = 0./255;//001
                bg_color.green = 0./255;
                bg_color.red = 255./255;
                gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12
                gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_ERROR);//GTK_MESSAGE_ERROR
                //1.8.1
                /*
                while(gtk_events_pending())
                    gtk_main_iteration();
                */
                switch (yCevap)
                {
                case ic_HakBitmis:
                    lcd_printf_ex(ALG_CENTER, 19,"YEMEK HAKKI YOK");
                    lcd_flip();
                    gtk_printf(label1, "%s\nYEMEK HAKKI YOK", gtk_label_get_label(GTK_LABEL(label1)));
                    gtk_label_set_text(GTK_LABEL(label_status), "         İZİN VERİLMEDİ, YEMEK HAKKI YOK");
                    break;
                case   ic_ParaBitmis:
                    //lcd_printf_ex(ALG_CENTER, 19,"BAKIYE YETERSIZ");
                    //lcd_flip();
                    gtk_printf(label1, "%s\nBAKİYE YETERSİZ", gtk_label_get_label(GTK_LABEL(label1)));
                    gtk_label_set_text(GTK_LABEL(label_status), "          İZİN VERİLMEDİ, BAKİYE YETERSİZ");
                    //while(gtk_events_pending())
                    //  gtk_main_iteration();

                    break;
                case   ic_GrupYetkisiz:
                    lcd_printf_ex(ALG_CENTER, 19,"GECIS YETKISI YOK");
                    lcd_flip();
                    gtk_printf(label1, "%s\nGEÇİŞ YETKİSİ YOK", gtk_label_get_label(GTK_LABEL(label1)));
                    gtk_label_set_text(GTK_LABEL(label_status), "        İZİN VERİLMEDİ, GEÇİŞ YETKİSİ YOK");
                    break;
                case   ic_OgunDisinda:
                    lcd_printf_ex(ALG_CENTER, 19,"OGUN DISINDASINIZ");
                    lcd_flip();
                    gtk_printf(label1, "%s\nÖĞÜN DIŞINDASINIZ", gtk_label_get_label(GTK_LABEL(label1)));
                    gtk_label_set_text(GTK_LABEL(label_status), "        İZİN VERİLMEDİ, ÖĞÜN DIŞINDASINIZ");
                    break;
                //6.9.0
                case   ic_RefakatBitmis:
                    lcd_printf_ex(ALG_CENTER, 5,"REFAKAT SURESI BITMIS");
                    lcd_flip();
                    gtk_printf(label1, "%s\nREFAKAT SÜRESİ BİTMİŞ", gtk_label_get_label(GTK_LABEL(label1)));
                    gtk_label_set_text(GTK_LABEL(label_status), "        İZİN VERİLMEDİ, REFAKAT SÜRESİ BİTMİŞ");
                    break;
                    break;
                case   ic_PDKSGirmemis:
                    lcd_printf_ex(ALG_CENTER, 5,"PDKS GIRISINIZ YOK");
                    lcd_flip();
                    gtk_printf(label1, "%s\nPDKS GİRİŞİNİZ YOK", gtk_label_get_label(GTK_LABEL(label1)));
                    gtk_label_set_text(GTK_LABEL(label_status), "        İZİN VERİLMEDİ, PDKS GİRİŞİNİZ YOK");
                    break;
                //7.0.0
                case   ic_RezYok:
                    lcd_printf_ex(ALG_CENTER, 5,"REZERVASYONUNUZ YOK");
                    lcd_flip();
                    gtk_printf(label1, "%s\nREZERVASYONUNUZ YOK", gtk_label_get_label(GTK_LABEL(label1)));
                    gtk_label_set_text(GTK_LABEL(label_status), "        İZİN VERİLMEDİ, REZERVASYONUNUZ YOK");
                    break;
                //7.2.0
                case   ic_YemekOkunamadi:
                    lcd_printf_ex(ALG_CENTER, 5,"YEMEK OKUNAMADI");
                    lcd_flip();
                    gtk_printf(label1, "%s\nYEMEK OKUNAMADI", gtk_label_get_label(GTK_LABEL(label1)));
                    gtk_label_set_text(GTK_LABEL(label_status), "        İZİN VERİLMEDİ, YEMEK OKUNAMADI");
                    break;
                //7.6.0
                case   ic_YemekhaneLimitiBitmis:
                    lcd_printf_ex(ALG_CENTER, 5,"YEMEKHANE LIMITI");
                    lcd_printf_ex(ALG_CENTER, 6,"GECILMIS");
                    lcd_flip();
                    gtk_printf(label1, "%s\nYEMEKHANE LİMİTİ GEÇİLMİŞ", gtk_label_get_label(GTK_LABEL(label1)));
                    gtk_label_set_text(GTK_LABEL(label_status), "     İZİN VERİLMEDİ, YEMEKHANE LİMİTİ GEÇİLMİŞ");
                    break;

                }
                //1.8.1
                while(gtk_events_pending())
                    gtk_main_iteration();
                //MSJ_Ses(rec_TERM.SES_OKUMA_NK);
                MSJ_Ses(rec_TERM.SES_OKUMA_NK);
            }
            while(gtk_events_pending())
                gtk_main_iteration();
        }
    }
    free(szTmp);
    free(tcp_Resp);
//	free(szBuf);
//	free(strRcv);
}


void REZERVASYON_Karsila(void)
{
    char Okuyucu;
//int j;
    char *szTmp;
//long t;
//unsigned long BAKIYE;


    szTmp=calloc(100,1);
    //2.2.3
    //memset(&rec_LogKart,0,sizeof(rec_LogKart));
    memset(&rec_LogKart,0,sizeof(struct_LOG_KART));

    //2.2.3
    memset(&rec_Personel, 0, sizeof(struct_PERSONEL));

    //t= AccTimer_SetTime(0);
    Okuyucu = READER_Rezervasyonlu_Read();
    if (Okuyucu==0)
    {
        free(szTmp);
        return;
    }

    if (PERSONEL_KayipCalinti(rec_Personel.KARTNO)==1) //Kayip Calinti Ise
    {
        rec_LogKart.GECTI_ONLINE=0;
        rec_LogKart.GECTI=0;
        rec_LogKart.CEVAP_ONLINE=0;
        rec_LogKart.CEVAP=ic_KayipCalinti;
        MSJ_Goster(rec_LogKart.CEVAP);
        MSJ_Ses(rec_TERM.SES_TANIMSIZ);
    }
    else
    {
        if (rec_TERM.TERM_TIP == TERM_YEMEKHANE)
        {
            //YEMEKHANE
            yCevap=0x00;
            MSJ_OK=0;
            z_MSJ_SURE_SN=0;
            rec_LogKart.CEVAP_ONLINE=0;

            rec_LogKart.CEVAP=REZERVASYON_YemekYermi();

            if (rec_LogKart.CEVAP == ic_Izin_Tamam)
            {
                //t = AccTimer_GetTime(t);
                //printf("MS:%lu\r\n",t);
                PERSONEL_Lcd();
                sprintf(szTmp,"YEMEK : %6lu%c%02lu TL",(unsigned long) (rec_Personel.HARCANAN/100),'.',(unsigned long)rec_Personel.HARCANAN%100);
                //LCD_PrintLeft(3,szTmp);
                lcd_printf(ALG_CENTER, szTmp);
                rec_Personel.BAKIYE = rec_Personel.BAKIYE - rec_Personel.HARCANAN;
                sprintf(szTmp,"KALAN : %6lu%c%02lu TL",(unsigned long) (rec_Personel.BAKIYE/100),'.',(unsigned long)rec_Personel.BAKIYE%100);
                //LCD_PrintLeft(4,szTmp);
                lcd_printf(ALG_CENTER, szTmp);
                lcd_printf_ex(ALG_CENTER, 5,"IZIN VERILDI");
                lcd_printf_ex(ALG_CENTER, 6,"AFIYET OLSUN");
                //6.5.1 Sirnak icin yukari tasindi
                //artik once fis sonra role
                //6.1.0 Sinop Unv icin eklendi
                //7.5.2
                //once sayac artar, sonra fis basilir
                SAYAC_Arti();
                if(rec_YAZICI.AKTIF)
                {
                    //6.5.0
                    //if(strncmp(rec_YAZICI.TIP, "CUSTOM", 6) == 0)
                    //7.5.1
                    //7.6.2
                    switch (rec_YAZICI.TIP)
                    {
                    case 1://Custom
                        YAZICI_FisYemekhane_Custom();
                        break;
                    case 2://Bixolon
                        YAZICI_FisYemekhane_Bixolon();
                        break;
                    case 3://Custom Bilecik
                        YAZICI_FisYemekhane_Custom_Bilecik();
                        break;
                    }
                }

                rec_LogKart.GECTI = PERSONEL_Gectimi(Okuyucu);

                //if (rec_LogKart.GECTI) gecsede gecmesede
                //{
                rec_Personel.YB.SAYI=rec_LogKart.GECTI;
                rec_LogKart.GECTI_ONLINE=PERSONEL_HareketIsle();
                //7.5.2
                //SAYAC_Arti();
                PERSONEL_Log(Okuyucu);
                //}
            }
            else
            {
                PERSONEL_Lcd();
                MSJ_Goster(rec_LogKart.CEVAP);
                lcd_printf_ex(ALG_CENTER, 5,"IZIN VERILMEDI !");
                MSJ_Ses(rec_TERM.SES_OKUMA_NK);
                MSJ_Ses(rec_TERM.SES_OKUMA_NK);
            }
            switch (yCevap)
            {
            case ic_HakBitmis:
                lcd_printf_ex(ALG_CENTER, 5,"YEMEK HAKKI YOK");
                delay(1000);
                break;
            case   ic_ParaBitmis:
                lcd_printf_ex(ALG_CENTER, 5,"BAKIYE YETERSIZ");
                delay(1000);
                break;
            case   ic_GrupYetkisiz:
                lcd_printf_ex(ALG_CENTER, 5,"GECIS YETKISI YOK");
                delay(1000);
                break;
            case   ic_OgunDisinda:
                lcd_printf_ex(ALG_CENTER, 5,"OGUN DISINDASINIZ");
                delay(1000);
                break;
            //6.9.0
            case   ic_RefakatBitmis:
                lcd_printf_ex(ALG_CENTER, 5,"REFAKAT SURESI BITMIS");
                delay(1000);
                break;
            case   ic_PDKSGirmemis:
                lcd_printf_ex(ALG_CENTER, 5,"PDKS GIRISINIZ YOK");
                delay(1000);
                break;
            //7.0.0
            case   ic_RezYok:
                lcd_printf_ex(ALG_CENTER, 5,"REZERVASYONUNUZ YOK");
                delay(1000);
                break;

            //7.2.0
            case   ic_YemekOkunamadi:
                lcd_printf_ex(ALG_CENTER, 5,"YEMEK OKUNAMADI");
                delay(1000);
                break;

            //7.6.0
            case   ic_YemekhaneLimitiBitmis:
                lcd_printf_ex(ALG_CENTER, 5,"YEMEKHANE LIMITI");
                lcd_printf_ex(ALG_CENTER, 5,"GECILMIS");
                delay(1000);
                break;

            }
        }
    }
    free(szTmp);
}

