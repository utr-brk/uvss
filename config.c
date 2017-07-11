#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <unistd.h>
//1.0.0
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>

//1.4.1
//#include "resource.h"
#include "main.h"
#include "config.h"
#include "minini.h" //6.0.0 havuz.ini icin
#include "lcd.h"
#include "tool.h"

#define  MAIN_CFG 	"/home/odroid/Solidus/MAIN.CFG"
#define  MASTER_KEY "/home/odroid/Solidus/MASTER.KEY"

#define  SEKTOR_DAT "/home/odroid/Solidus/SEKTOR.DAT"

#define	 HAVUZ_INI	"/home/odroid/Solidus/HAVUZ.INI" 	//6.0.0
#define	 YAZICI_INI	"/home/odroid/Solidus/YAZICI.INI" 	//6.1.0

#define	 PINPAD_INI	"/home/odroid/Solidus/PINPAD.INI" 	//m8110

//1.0.0
#define AYARLAR_INI "/home/odroid/Solidus/AYARLAR.INI"
//2.1.0
#define ISKUR_INI "/home/odroid/Solidus/ISKUR.INI"

//6.1.0
#define sizearray(a)	(sizeof(a) / sizeof((a)[0]))

struct_HAVUZ 	rec_HAVUZ; 	//6.0.0
struct_YAZICI	rec_YAZICI; //6.1.0
struct_TERM		rec_TERM;
struct_PINPAD	rec_PINPAD;
char			net_ONLINE;
char			net_ONLINE_PARA;

//1.0.0
char            KEYPAD_AKTIF;
int             Okuyucu_HIZ[2];

//1.8.0
int             Okuyucu_TIP[2];
int             RF_ID_MASK_AKTIF;
char            RF_ID_MASK[13];
//1.7.0
char            KART_SIFRE_AKTIF;
char            Kart_Sifre[10];
//2.0.0
char            OKUYUCU_SIFRE_AKTIF[2];

//2.1.0
char            ISKUR;

//2.2.1
char            TERM_AKTIF;

struct_AYARLAR rec_Ayarlar;//7.4.0
//7.6.0
struct_LIMITS	rec_LIMIT;

unsigned        okuma_bekleme = 0;

int refakatci[128];
int kontrolsuz[128];

int rezervasyon;
//m8110
//bunların hepsi unsigned char idi
uint8_t  Personel_Sektor;
uint8_t  Para_Sektor;
uint8_t  Para_Blok;
uint8_t  YB_Sektor;
uint8_t  YB_Blok;
uint8_t  HB_Blok;

//2.4.0
int KAPI_TCP_WAIT_TIME;

void CONFIG_Load(void)
{
    FILE *inp;

    char *ptr;
    int   i;
    char  rfkt[512], kntrl[512];
    char  rez_file[13];
    struct_date d;
    //1.8.0
    char tipp[17];

    //IPCONFIG_t IP_CONFIG;


    memset(&rec_TERM,0x00,sizeof(rec_TERM));
    memset(&rec_LIMIT,0x00,sizeof(rec_LIMIT));
    //1.7.0
    memset(Kart_Sifre, 0x00, sizeof(Kart_Sifre));

    if(access(AYARLAR_INI, 0) == 0)
    {
        //2.2.1
        TERM_AKTIF = (char) ini_getl("TERMINAL", "AKTIF", 1, AYARLAR_INI);

        ini_gets("TERMINAL", "KOD", "000", rec_TERM.KOD, sizearray(rec_TERM.KOD), AYARLAR_INI);
        ini_gets("TERMINAL", "ACIKLAMA", "UTARIT SOLIDUS TERMINAL", rec_TERM.ACIKLAMA, sizearray(rec_TERM.ACIKLAMA), AYARLAR_INI);
        rec_TERM.TERM_TIP = (char) ini_getl("TERMINAL", "TIPI", 0, AYARLAR_INI);
        //brk
        //unutma kaldır
        //0.0.1
        rec_TERM.TERM_TIP = TERM_UVSS;
        ini_gets("TERMINAL", "KAPIKODU", "000", rec_TERM.KAPI_KOD, sizearray(rec_TERM.KAPI_KOD), AYARLAR_INI);

        ini_gets("BAGLANTI", "IP", "192.168.1.150", rec_TERM.IP_TERM, sizearray(rec_TERM.IP_TERM), AYARLAR_INI);
        ini_gets("BAGLANTI", "SUBNET", "255.255.255.0", rec_TERM.IP_SUBNET, sizearray(rec_TERM.IP_SUBNET), AYARLAR_INI);
        ini_gets("BAGLANTI", "GATEWAY", "192.168.1.1", rec_TERM.IP_GATEWAY, sizearray(rec_TERM.IP_GATEWAY), AYARLAR_INI);
        ini_gets("BAGLANTI", "SERVER", "192.168.1.111", rec_TERM.IP_SERVER, sizearray(rec_TERM.IP_SERVER), AYARLAR_INI);
        //1.9.4
        //rec_TERM.PORT_SERVER = (unsigned long) ini_getl("TERMINAL", "PORT", 4444, AYARLAR_INI);
        rec_TERM.PORT_SERVER = (unsigned long) ini_getl("BAGLANTI", "PORT", 4444, AYARLAR_INI);

        rec_TERM.READERS[0].AKTIF = (char) ini_getl("OKUYUCU1", "AKTIF", 1, AYARLAR_INI);
        rec_TERM.READERS[0].ROLE = (unsigned char) ini_getl("OKUYUCU1", "ROLE_AKTIF", 1, AYARLAR_INI);
        rec_TERM.READERS[0].ROLE_SN = (unsigned char) ini_getl("OKUYUCU1", "ROLE_SURE", 2, AYARLAR_INI);
        rec_TERM.READERS[0].SENS = (unsigned char) ini_getl("OKUYUCU1", "SENS_AKTIF", 1, AYARLAR_INI);
        rec_TERM.READERS[0].SENS_SN = (unsigned char) ini_getl("OKUYUCU1", "SENS_SURE", 5, AYARLAR_INI);
        Okuyucu_HIZ[0] = (int) ini_getl("OKUYUCU1", "HIZ", 115200, AYARLAR_INI);
        //1.8.0
        ini_gets("OKUYUCU1", "TIP", "MIFARE", tipp, sizearray(tipp), AYARLAR_INI);
        Okuyucu_TIP[0] = 0x0;
        if(strncmp("MIFARE", tipp, 6) == 0)
            Okuyucu_TIP[0] = 0x0;
        else if(strncmp("RF-ID", tipp, 5) == 0)
            Okuyucu_TIP[0] = 0x1;
        //2.0.0
        else if(strncmp("CN-MFR", tipp, 6) == 0)
            Okuyucu_TIP[0] = 0x2;
        else if(strncmp("CN-KYB", tipp, 6) == 0)
            Okuyucu_TIP[0] = 0x3;

        //2.0.0
        OKUYUCU_SIFRE_AKTIF[0] = (char) ini_getl("OKUYUCU1", "SIFRE_AKTIF", 0, AYARLAR_INI);


        //2.0.0
        //brk
        //silmeyi unutma
        //Okuyucu_TIP[0] = 0x3;

        okuma_bekleme = (unsigned) ini_getl("SURELER", "LOGIN_OLMA_BEKLEME_SURESI", 0, AYARLAR_INI);
        rec_TERM.READERS[1].AKTIF = (char) ini_getl("OKUYUCU2", "AKTIF", 0, AYARLAR_INI);
        rec_TERM.READERS[1].ROLE = (unsigned char) ini_getl("OKUYUCU2", "ROLE_AKTIF", 2, AYARLAR_INI);
        rec_TERM.READERS[1].ROLE_SN = (unsigned char) ini_getl("OKUYUCU2", "ROLE_SURE", 2, AYARLAR_INI);
        rec_TERM.READERS[1].SENS = (unsigned char) ini_getl("OKUYUCU2", "SENS_AKTIF", 2, AYARLAR_INI);
        rec_TERM.READERS[1].SENS_SN = (unsigned char) ini_getl("OKUYUCU2", "SENS_SURE", 5, AYARLAR_INI);
        Okuyucu_HIZ[1] = (int) ini_getl("OKUYUCU2", "HIZ", 115200, AYARLAR_INI);
        //1.8.0
        ini_gets("OKUYUCU2", "TIP", "MIFARE", tipp, sizearray(tipp), AYARLAR_INI);
        Okuyucu_TIP[1] = 0x0;
        if(strncmp("MIFARE", tipp, 6) == 0)
            Okuyucu_TIP[1] = 0x0;
        else if(strncmp("RF-ID", tipp, 5) == 0)
            Okuyucu_TIP[1] = 0x1;
        //2.0.0
        else if(strncmp("CN-MFR", tipp, 6) == 0)
            Okuyucu_TIP[1] = 0x2;
        else if(strncmp("CN-KYB", tipp, 6) == 0)
            Okuyucu_TIP[1] = 0x3;

        //2.0.0
        OKUYUCU_SIFRE_AKTIF[1] = (char) ini_getl("OKUYUCU2", "SIFRE_AKTIF", 0, AYARLAR_INI);

        //1.8.0
        RF_ID_MASK_AKTIF =  (int) ini_getl("RF-ID", "MASK_AKTIF", 1, AYARLAR_INI);
        ini_gets("RF-ID", "MASK", "701100037AF8", RF_ID_MASK, sizearray(RF_ID_MASK), AYARLAR_INI);

        rec_TERM.SENS_CEVAP = (unsigned char) ini_getl("OKUYUCU_SENS", "CEVAP", 0, AYARLAR_INI);

        ini_gets("SES", "OKUMA_BASARILI", "21", rec_TERM.SES_OKUMA_OK, sizearray(rec_TERM.SES_OKUMA_OK), AYARLAR_INI);
        ini_gets("SES", "OKUMA_BASARISIZ", "41", rec_TERM.SES_OKUMA_NK, sizearray(rec_TERM.SES_OKUMA_NK), AYARLAR_INI);
        ini_gets("SES", "IZIN_VERILDI", "213121", rec_TERM.SES_IZIN_OK, sizearray(rec_TERM.SES_IZIN_OK), AYARLAR_INI);
        ini_gets("SES", "OKUMA_BASARILI", "413141", rec_TERM.SES_IZIN_NK, sizearray(rec_TERM.SES_IZIN_NK), AYARLAR_INI);
        ini_gets("SES", "TANIMSIZ_KART", "514151", rec_TERM.SES_TANIMSIZ, sizearray(rec_TERM.SES_TANIMSIZ), AYARLAR_INI);

        ini_gets("MESAJ", "IZIN_VERILDI", "İZİN VERİLDİ", rec_TERM.MSJ_IZIN_OK, sizearray(rec_TERM.MSJ_IZIN_OK), AYARLAR_INI);
        ini_gets("MESAJ", "IZIN_VERILMEDI", "İZİN VERİLMEDİ", rec_TERM.MSJ_IZIN_NK, sizearray(rec_TERM.MSJ_IZIN_NK), AYARLAR_INI);
        ini_gets("MESAJ", "TANIMSIZ_KART", "TANIMSIZ KART", rec_TERM.MSJ_TANIMSIZ, sizearray(rec_TERM.MSJ_TANIMSIZ), AYARLAR_INI);

        rec_TERM.MSJ_SURE_SN = (unsigned char) ini_getl("SURELER", "MESAJ_GOSTERIM", 2, AYARLAR_INI);
        rec_TERM.ONLINE_TEKRAR_DK = (unsigned char) ini_getl("SURELER", "ONLINE_DENEME", 30, AYARLAR_INI);
        rec_TERM.TS_GUNCELLE_SAAT = (unsigned char) ini_getl("SURELER", "SAAT_GUNCELLEME", 6, AYARLAR_INI);
        rec_TERM.LOG_KART_SAAT = (unsigned char) ini_getl("SURELER", "KART_LOG_GONDERME", 2, AYARLAR_INI);
        rec_TERM.LOG_ALARM_SAAT = (unsigned char) ini_getl("SURELER", "ALARM_LOG_GONDERME", 2, AYARLAR_INI);
        rec_TERM.LOG_TERM_SAAT = (unsigned char) ini_getl("SURELER", "TERMINAL_LOG_GONDERME", 6, AYARLAR_INI);

        //2.4.0
        KAPI_TCP_WAIT_TIME = (int) ini_getl("SURELER", "KAPI_TCP_BEKLEME_SURESI", 2, AYARLAR_INI);

        rec_YAZICI.AKTIF = (char) ini_getl("YAZICI", "AKTIF", 0, AYARLAR_INI);
        ini_gets("YAZICI", "ACIKLAMA1", "UTARIT UNIVERSITE YEMEKHANE TERMINALI", rec_YAZICI.ACIKLAMA1, sizearray(rec_YAZICI.ACIKLAMA1), AYARLAR_INI);
        ini_gets("YAZICI", "ACIKLAMA2", "", rec_YAZICI.ACIKLAMA2, sizearray(rec_YAZICI.ACIKLAMA2), AYARLAR_INI);
        ini_gets("YAZICI", "ACIKLAMA3", "", rec_YAZICI.ACIKLAMA3, sizearray(rec_YAZICI.ACIKLAMA3), AYARLAR_INI);
        rec_YAZICI.TIP = (char) ini_getl("YAZICI", "TIPI", 0, AYARLAR_INI);
        rec_YAZICI.KART_NO_BAS = (char) ini_getl("YAZICI", "KARTNO_BAS", 0, AYARLAR_INI);
        //7.5.2
        rec_YAZICI.SIRA_NO_BAS = (char) ini_getl("YAZICI", "SIRANO_BAS", 0, AYARLAR_INI);
        rec_YAZICI.TEKNUSHA = (char) ini_getl("YAZICI","TEKNUSHA", 0, AYARLAR_INI); //yoksa ya da 0 ise 2 nusha basar, varsa ve 1 ise tek nusha basar

        KEYPAD_AKTIF = (char) ini_getl("KEYPAD","AKTIF", 0, AYARLAR_INI);
        KART_SIFRE_AKTIF = (char) ini_getl("KEYPAD","KART_SIFRE_AKTIF", 0, AYARLAR_INI);

        rec_HAVUZ.AKTIF = (char) ini_getl("HAVUZ", "AKTIF", 0, AYARLAR_INI);;
        Para_Blok = '\x2'; //Havuzun parası 2. blokda duruyor.
        HB_Blok = '\x1'; //Havuz Bilgisi Blogu = 1
        rec_HAVUZ.CARPANI = (int) ini_getl("HAVUZ", "CARPANI", 1, AYARLAR_INI);
        rec_HAVUZ.GIRIS = (char) ini_getl("HAVUZ","GIRIS", 1, AYARLAR_INI); //1=> Giris, 0=> Cikis
        rec_HAVUZ.CEZA = (unsigned long) ini_getl("HAVUZ", "CEZASI", 0, AYARLAR_INI);

        //7.6.0
        rec_LIMIT.AKTIF = (char) ini_getl("YEMEK_LIMITLERI", "AKTIF", 0, AYARLAR_INI);
        rec_LIMIT.TURNIKE = (int) ini_getl("YEMEK_LIMITLERI", "TURNIKE_LIMIT", 0, AYARLAR_INI);
        rec_LIMIT.MISAFIR = (int) ini_getl("YEMEK_LIMITLERI", "MISAFIR_LIMIT", 0, AYARLAR_INI);


//ini_gets("SIFRELER", "PERSONEL", "\xFE\xFB\x11\xAA\xBB\xCC", ( char *)rec_TERM.KEY_PERSONEL, sizearray(rec_TERM.KEY_PERSONEL), AYARLAR_INI);
//ini_gets("SIFRELER", "MASTER", "\xFF\xFE\xFF\x11\x22\x33", ( char *)rec_TERM.KEY_MASTER, sizearray(rec_TERM.KEY_MASTER), AYARLAR_INI);
//ini_gets("SIFRELER", "DUSME", "\xFF\xFE\xFF\x11\x22\x33", ( char *)rec_TERM.KEY_BAKIYE, sizearray(rec_TERM.KEY_BAKIYE), AYARLAR_INI);
//ini_gets("SIFRELER", "YUKLEME", "\xFF\xFE\xFF\x11\x22\x33", ( char *)rec_TERM.KEY_TOPUP, sizearray(rec_TERM.KEY_TOPUP), AYARLAR_INI);
        ini_gets("SIFRELER", "ADMIN", "1111", rec_TERM.ADMIN_PASS, sizearray(rec_TERM.ADMIN_PASS), AYARLAR_INI);

        //1.7.0
        ini_gets("SIFRELER", "ADMIN_KART_NO", "DB04DD28", rec_TERM.ADMIN_CARD, sizearray(rec_TERM.ADMIN_CARD), AYARLAR_INI);

        //1.1.1
        //master key hicbir yerde olusturulmuyordu.
        memcpy(rec_TERM.KEY_MASTER,"\xFF\xFE\xFF\x11\x22\x33",6);
    }
    else
    {
        inp=fopen(MAIN_CFG,"rb");
        if (inp==NULL)
        {
            /*strcpy(rec_TERM.KOD,RESOURCE_ReadLine(100));
            strcpy(rec_TERM.ACIKLAMA,RESOURCE_ReadLine(101));
            rec_TERM.TERM_TIP = TERM_YEMEKHANE;
            strcpy(rec_TERM.KAPI_KOD,RESOURCE_ReadLine(100));
            strcpy(rec_TERM.IP_TERM,RESOURCE_ReadLine(102));
            strcpy(rec_TERM.IP_SUBNET,RESOURCE_ReadLine(103));
            strcpy(rec_TERM.IP_GATEWAY,RESOURCE_ReadLine(104));
            strcpy(rec_TERM.IP_SERVER,RESOURCE_ReadLine(104));*/
            strcpy(rec_TERM.KOD,"001");
            strcpy(rec_TERM.ACIKLAMA,"TERMINAL 001");
            rec_TERM.TERM_TIP = TERM_YEMEKHANE;
            strcpy(rec_TERM.KAPI_KOD,"001");
            strcpy(rec_TERM.IP_TERM,"192.168.1.150");
            strcpy(rec_TERM.IP_SUBNET,"255.255.255.0");
            strcpy(rec_TERM.IP_GATEWAY,"192.168.1.1");
            strcpy(rec_TERM.IP_SERVER,"192.168.1.111");
            rec_TERM.PORT_SERVER = 4444;
            rec_TERM.READERS[0].AKTIF=1;
            rec_TERM.READERS[0].ROLE=5;
            rec_TERM.READERS[0].ROLE_SN=1;
            /*strcpy(rec_TERM.SES_OKUMA_OK,RESOURCE_ReadLine(105));
            strcpy(rec_TERM.SES_OKUMA_NK,RESOURCE_ReadLine(106));
            strcpy(rec_TERM.SES_IZIN_OK,RESOURCE_ReadLine(107));
            strcpy(rec_TERM.SES_IZIN_NK,RESOURCE_ReadLine(108));
            strcpy(rec_TERM.SES_TANIMSIZ,RESOURCE_ReadLine(109));
            strcpy(rec_TERM.MSJ_IZIN_OK,RESOURCE_ReadLine(110));
            strcpy(rec_TERM.MSJ_IZIN_NK,RESOURCE_ReadLine(111));
            strcpy(rec_TERM.MSJ_TANIMSIZ,RESOURCE_ReadLine(112));*/
            strcpy(rec_TERM.SES_OKUMA_OK,"21");
            strcpy(rec_TERM.SES_OKUMA_NK,"41");
            strcpy(rec_TERM.SES_IZIN_OK,"213121");
            strcpy(rec_TERM.SES_IZIN_NK,"413141");
            strcpy(rec_TERM.SES_TANIMSIZ,"514151");
            strcpy(rec_TERM.MSJ_IZIN_OK,"IZIN VERILDI");
            strcpy(rec_TERM.MSJ_IZIN_NK,"IZIN VERILMEDI");
            strcpy(rec_TERM.MSJ_TANIMSIZ,"TANIMSIZ KART");
            rec_TERM.MSJ_SURE_SN=3;
            rec_TERM.ONLINE_TEKRAR_DK=30;
            rec_TERM.TS_GUNCELLE_SAAT=12;
            rec_TERM.LOG_KART_SAAT=3;
            rec_TERM.LOG_ALARM_SAAT=3;
            rec_TERM.LOG_TERM_SAAT=3;
            //utr
            memcpy(rec_TERM.KEY_PERSONEL,"\xFE\xFB\x11\xAA\xBB\xCC",6);
            memcpy(rec_TERM.KEY_MASTER,"\xFF\xFE\xFF\x11\x22\x33",6);
            memcpy(rec_TERM.KEY_BAKIYE,"\xFF\xFF\xFF\xFF\xFF\xFF",6);
            memcpy(rec_TERM.KEY_TOPUP,"\xFF\xFF\xFF\xFF\xFF\xFF",6);
            //strcpy(rec_TERM.ADMIN_PASS,RESOURCE_ReadLine(113));
            strcpy(rec_TERM.ADMIN_PASS,"1111");
        }
        else
        {
            fseek(inp,0,SEEK_SET);
            fread(&rec_TERM,sizeof(rec_TERM),1,inp);
            fclose(inp);
        }
    }
    /*
        //odroid
        //buraları düzeltmeyi unutma
    	rec_TERM.READERS[0].AKTIF = 0x1;
    	rec_TERM.READERS[1].AKTIF = 0x0;
        rec_TERM.READERS[0].ROLE = 0x1;
        rec_TERM.READERS[0].ROLE_SN = 0x1;
    	net_ONLINE=0;
    	net_ONLINE_PARA = 0x01;

    	strcpy(rec_TERM.IP_TERM,"192.168.2.215");
        strcpy(rec_TERM.IP_SUBNET,"255.255.255.0");
        strcpy(rec_TERM.IP_GATEWAY,"192.168.2.1");
        strcpy(rec_TERM.IP_SERVER,"192.168.2.38");
        //rec_TERM.TERM_TIP = TERM_KAPI;

    	//strcpy(IP_CONFIG.IP, rec_TERM.IP_TERM);
    	//strcpy(IP_CONFIG.Mask, rec_TERM.IP_SUBNET);
    	//strcpy(IP_CONFIG.GW, rec_TERM.IP_GATEWAY);
    	//strcpy(IP_CONFIG.Dns1, rec_TERM.IP_GATEWAY);
    	//IP_CONFIG.dhcp = 0;
    	//strcpy(IP_CONFIG.iface, "eth0");
    */
    //0.2.2
    //Para Terminali ise her seferinde master kart okutulmalı,
    //Yoksa Default Utarit keylerine döner.
    if(rec_TERM.TERM_TIP != TERM_PARA)
    {
        inp=fopen(MASTER_KEY,"rb");
        if (inp!=NULL)
        {
            fseek(inp,0,SEEK_SET);
            fread(rec_TERM.KEY_BAKIYE,6,1,inp);
            fread(rec_TERM.KEY_TOPUP,6,1,inp);
            fread(rec_TERM.KEY_PERSONEL, 6, 1, inp);
            fclose(inp);
        }
    }
    //0.2.2
    //main.cfg'den gelen key'leri degistirmek icin
    if(rec_TERM.TERM_TIP == TERM_PARA)
    {
        memcpy(rec_TERM.KEY_PERSONEL,"\xFE\xFB\x11\xAA\xBB\xCC",6);
        memcpy(rec_TERM.KEY_BAKIYE,"\xFF\xFF\xFF\xFF\xFF\xFE",6);
        memcpy(rec_TERM.KEY_TOPUP,"\xFF\xFF\xFF\xFF\xFF\xFD",6);
    }

    Personel_Sektor = 99;
    inp=fopen(SEKTOR_DAT,"rb");
    if (inp!=NULL)
    {
        fseek(inp,0,SEEK_SET);
        fread(&Para_Sektor,1,1,inp);
        if((Para_Sektor >3) && (Para_Sektor < 10))
        {
            Para_Blok = '\x0';
            YB_Sektor = Para_Sektor+1;
            YB_Blok = '\x0';
            //6.9.1
            fread(&Personel_Sektor, 1, 1, inp);
            if((Personel_Sektor > 15) || (Personel_Sektor == Para_Sektor)
                    || (Personel_Sektor == YB_Sektor) || (Personel_Sektor == 0))
                Personel_Sektor = 2;
        }
        else
        {
            lcd_clean();
            lcd_flip();
            lcd_printf_ex(ALG_CENTER, 8, "SEKTOR BILGISI HATALI");
            lcd_printf_ex(ALG_CENTER, 10, "STANDARD SEKTORLER");
            lcd_printf_ex(ALG_CENTER, 12,  "KULLANILACAK!");
            lcd_flip();
            //MSJ_Ses("5343332313");
            sleep(1);
            //odroid
            //lcd_set_bk_color(COLOR_BLUE);
            //renk ayarını unutma!
            Para_Sektor = '\x1';
            Para_Blok = '\x0';
            YB_Sektor = '\x3';
            YB_Blok = '\x0';
            //6.9.1
            Personel_Sektor = '\x2';
        }
        //6.9.1 yukari alindi
        /*
        fread(&Personel_Sektor, 1, 1, inp);
        if((Personel_Sektor > 15) || (Personel_Sektor == Para_Sektor))
        	Personel_Sektor = 2;
        */
        fclose(inp);
    }
    else
    {
        Personel_Sektor = '\x2';
        Para_Sektor = '\x1';
        Para_Blok = '\x0';
        YB_Sektor = '\x3';
        YB_Blok = '\x0';
    }

//1.0.0
//ayarlar.ini içine alındı
    /*
    	//6.0.0 havuz icin
    	memset(&rec_HAVUZ, 0x00,sizeof(rec_HAVUZ));
    	if(access(HAVUZ_INI, 0) == 0){
    		rec_HAVUZ.AKTIF = 1;
    		Para_Blok = '\x2'; //Havuzun parası 2. blokda duruyor.
    		HB_Blok = '\x1'; //Havuz Bilgisi Blogu = 1
    		rec_HAVUZ.CARPANI = (int) ini_getl(NULL, "CARPANI", 1, HAVUZ_INI);
    		rec_HAVUZ.GIRIS = (char) ini_getbool(NULL,"GIRIS", 1, HAVUZ_INI); //1=> Giris, 0=> Cikis
    		//6.4.0
    		rec_HAVUZ.CEZA = (unsigned long) ini_getl(NULL, "CEZASI", 0, HAVUZ_INI);
    	}
    */

    /*
    //1.0.0
    //ayarlar.ini içine alındı
    	//6.1.0 yazici icin
    	memset(&rec_YAZICI, 0x00,sizeof(rec_YAZICI));
    	if(access(YAZICI_INI, 0) == 0){
    		rec_YAZICI.AKTIF = 1;
    		ini_gets("YAZICI", "ACIKLAMA1", "UTARIT UNIVERSITE YEMEKHANE TERMINALI", rec_YAZICI.ACIKLAMA1, sizearray(rec_YAZICI.ACIKLAMA1), YAZICI_INI);
    		ini_gets("YAZICI", "ACIKLAMA2", "", rec_YAZICI.ACIKLAMA2, sizearray(rec_YAZICI.ACIKLAMA2), YAZICI_INI);
    		ini_gets("YAZICI", "ACIKLAMA3", "", rec_YAZICI.ACIKLAMA3, sizearray(rec_YAZICI.ACIKLAMA3), YAZICI_INI);
    		ini_gets("YAZICI", "KART_NO_BAS", "BAS", rec_YAZICI.KART_NO_BAS, sizearray(rec_YAZICI.KART_NO_BAS), YAZICI_INI);
    		//rec_YAZICI.KART_NO_BAS = (int) ini_getl("YAZICI", "KART_NO_BAS", 0, YAZICI_INI); //1=> BAS, 0=> BASMA
    		//rec_YAZICI.TIP = (int)ini_getl("YAZICI","TIP", 5, YAZICI_INI); //0=> POS, 1=> EXTERNAL
    		ini_gets("YAZICI", "TIP", "POS", rec_YAZICI.TIP, sizearray(rec_YAZICI.TIP), YAZICI_INI);
    		rec_YAZICI.TEKNUSHA = (char)ini_getbool("YAZICI","TEKNUSHA", 0, YAZICI_INI); //0.2.0 yoksa ya da 0 ise 2 nusha basar, varsa ve 1 ise tek nusha basar
    		//debug_print("yazici tip: %s", rec_YAZICI.TIP);
    	}
    */
    memset(&rec_Ayarlar, 0x00,sizeof(rec_Ayarlar));//burası mercure'de hatalı.
    rec_Ayarlar.Refakatci_Kontrol = (char)ini_getl("HASTANE", "REFAKATCIKONTROL", 0, AYARLAR_INI);
    if(rec_Ayarlar.Refakatci_Kontrol)
    {
        memset(rfkt, 0x0, sizeof(rfkt));
        //7.5.0
        //ini_gets(NULL, "REFAKATCI", "", rfkt, sizearray(rfkt), KONTROL_INI);
        ini_gets("HASTANE", "REFAKATCI", "", rfkt, sizearray(rfkt), AYARLAR_INI);

        //disaridan iceri alinip, 128 ile sinirlandi
        i = 0;
        ptr = strtok(rfkt, ",");
        while(ptr != NULL)
        {
            refakatci[i++] = atoi(ptr);
            ptr = strtok(NULL, ",");
            if(i ==128)
                break;
        }
    }
    rec_Ayarlar.Giris_Kontrol = (char)ini_getl("HASTANE", "GIRISKONTROL", 0, AYARLAR_INI);
    if(rec_Ayarlar.Giris_Kontrol)
    {
        memset(kntrl, 0x0, sizeof(kntrl));
        ini_gets("HASTANE", "KONTROLSUZ", "", kntrl, sizearray(kntrl), AYARLAR_INI);
        i = 0;
        ptr = strtok(kntrl, ",");
        while(ptr != NULL)
        {
            kontrolsuz[i++] = atoi(ptr);
            ptr = strtok(NULL, ",");
            if(i ==128)
                break;
        }
    }


//7.0.0
    get_date(&d);
    sprintf(rez_file, "%02d%02d%04d.rez", d.da_day, d.da_mon, d.da_year);
    if(access(rez_file, 0) == 0)//rezervasyon dosyasi var
        rezervasyon = 1;
    else
        rezervasyon = 0;


//7.4.0
    //memset(&rec_Ayarlar, 0x00,sizeof(rec_Ayarlar));
    //7.5.0
    //rec_Ayarlar.Grup_ID = (char) ini_getl("GRUP_ID","KONTROLLU", 5, AYARLAR_INI); //1=> Kontrollu, 0=> Kontrolsuz
    //2.0.2
    //rec_Ayarlar.Grup_ID = (char) ini_getl("OGUNKONTROLU","KONTROLLU", 0, AYARLAR_INI); //1=> Kontrollu, 0=> Kontrolsuz
    rec_Ayarlar.Grup_ID = (char) ini_getl("OGUNKONTROLU","AKTIF", 0, AYARLAR_INI); //1=> Kontrollu, 0=> Kontrolsuz

    //2.1.0
    if(access(ISKUR_INI, 0)==0)
    {
        ISKUR = 1;
    }
    else
    {
        ISKUR = 0;
    }

    //rec_TERM.ONLINE_TEKRAR_DK=3;
    //rec_TERM.TS_GUNCELLE_SAAT=1;
    //silmeyi unutma m8110
    //rec_TERM.TERM_TIP = TERM_YEMEKHANE;
    //rec_TERM.TERM_TIP = TERM_PARA;
    //rec_TERM.TERM_TIP = TERM_HARCAMA;
    //strcpy(rec_TERM.IP_SERVER,"10.10.10.1");
    //rec_TERM.PORT_SERVER = 4444;
}


void	CONFIG_Save(void)
{
    FILE * out;
//	char szBuf[7];

    remove(MAIN_CFG);
    out=fopen(MAIN_CFG,"a+b");
    fwrite(&rec_TERM,sizeof(rec_TERM),1,out);
    fclose(out);

//1.0.0
    ini_puts("TERMINAL", "KOD", rec_TERM.KOD, AYARLAR_INI);
    ini_puts("TERMINAL", "ACIKLAMA", rec_TERM.ACIKLAMA, AYARLAR_INI);
    ini_putl("TERMINAL", "TIPI", rec_TERM.TERM_TIP, AYARLAR_INI);
    ini_puts("TERMINAL", "KAPIKODU", rec_TERM.KAPI_KOD, AYARLAR_INI);
//2.2.1
    ini_putl("TERMINAL", "AKTIF", TERM_AKTIF, AYARLAR_INI);

    ini_puts("BAGLANTI", "IP", rec_TERM.IP_TERM, AYARLAR_INI);
    ini_puts("BAGLANTI", "SUBNET", rec_TERM.IP_SUBNET, AYARLAR_INI);
    ini_puts("BAGLANTI", "GATEWAY", rec_TERM.IP_GATEWAY, AYARLAR_INI);
    ini_puts("BAGLANTI", "SERVER", rec_TERM.IP_SERVER, AYARLAR_INI);
    //1.9.5
    //ini_putl("TERMINAL", "PORT", rec_TERM.PORT_SERVER, AYARLAR_INI);
    ini_putl("BAGLANTI", "PORT", rec_TERM.PORT_SERVER, AYARLAR_INI);

    ini_putl("OKUYUCU1", "AKTIF", rec_TERM.READERS[0].AKTIF, AYARLAR_INI);
    ini_putl("OKUYUCU1", "ROLE_AKTIF", rec_TERM.READERS[0].ROLE, AYARLAR_INI);
    ini_putl("OKUYUCU1", "ROLE_SURE", rec_TERM.READERS[0].ROLE_SN , AYARLAR_INI);
    ini_putl("OKUYUCU1", "SENS_AKTIF", rec_TERM.READERS[0].SENS, AYARLAR_INI);
    ini_putl("OKUYUCU1", "SENS_SURE", rec_TERM.READERS[0].SENS_SN, AYARLAR_INI);
    ini_putl("OKUYUCU1", "HIZ", Okuyucu_HIZ[0], AYARLAR_INI);

    //2.0.0
    /*
    //1.8.0
    if(Okuyucu_TIP[0] == 0x00)
         ini_puts("OKUYUCU1", "TIP", "MIFARE", AYARLAR_INI);
    else
        ini_puts("OKUYUCU1", "TIP", "RF-ID", AYARLAR_INI);
    */
    switch(Okuyucu_TIP[0])
    {
    case 0:
        ini_puts("OKUYUCU1", "TIP", "MIFARE", AYARLAR_INI);
        break;
    case 1:
        ini_puts("OKUYUCU1", "TIP", "RF-ID", AYARLAR_INI);
        break;
    case 2:
        ini_puts("OKUYUCU1", "TIP", "CN-MFR", AYARLAR_INI);
        break;
    case 3:
        ini_puts("OKUYUCU1", "TIP", "CN-KYB", AYARLAR_INI);
        break;
    }

    //2.0.0
    ini_putl("OKUYUCU1", "SIFRE_AKTIF", OKUYUCU_SIFRE_AKTIF[0], AYARLAR_INI);

    ini_putl("SURELER", "LOGIN_OLMA_BEKLEME_SURESI", okuma_bekleme, AYARLAR_INI);


    ini_putl("OKUYUCU2", "AKTIF", rec_TERM.READERS[1].AKTIF, AYARLAR_INI);
    ini_putl("OKUYUCU2", "ROLE_AKTIF", rec_TERM.READERS[1].ROLE, AYARLAR_INI);
    ini_putl("OKUYUCU2", "ROLE_SURE", rec_TERM.READERS[1].ROLE_SN , AYARLAR_INI);
    ini_putl("OKUYUCU2", "SENS_AKTIF", rec_TERM.READERS[1].SENS, AYARLAR_INI);
    ini_putl("OKUYUCU2", "SENS_SURE", rec_TERM.READERS[1].SENS_SN, AYARLAR_INI);
    ini_putl("OKUYUCU2", "HIZ", Okuyucu_HIZ[1], AYARLAR_INI);

    //2.0.0
    /*//1.8.0
    if(Okuyucu_TIP[1] == 0x00)
         ini_puts("OKUYUCU2", "TIP", "MIFARE", AYARLAR_INI);
    else
        ini_puts("OKUYUCU2", "TIP", "RF-ID", AYARLAR_INI);
    */
    switch(Okuyucu_TIP[1])
    {
    case 0:
        ini_puts("OKUYUCU2", "TIP", "MIFARE", AYARLAR_INI);
        break;
    case 1:
        ini_puts("OKUYUCU2", "TIP", "RF-ID", AYARLAR_INI);
        break;
    case 2:
        ini_puts("OKUYUCU2", "TIP", "CN-MFR", AYARLAR_INI);
        break;
    case 3:
        ini_puts("OKUYUCU2", "TIP", "CN-KYB", AYARLAR_INI);
        break;
    }

    //2.0.0
    ini_putl("OKUYUCU2", "SIFRE_AKTIF", OKUYUCU_SIFRE_AKTIF[1], AYARLAR_INI);

    //1.8.0
    if(RF_ID_MASK_AKTIF)
        ini_putl("RF_ID", "MASK_AKTIF", 1, AYARLAR_INI);
    else
        ini_putl("RF_ID", "MASK_AKTIF", 0, AYARLAR_INI);

    ini_puts("RF_ID", "MASK", RF_ID_MASK, AYARLAR_INI);

    ini_putl("OKUYUCU_SENS", "CEVAP", rec_TERM.SENS_CEVAP, AYARLAR_INI);

    ini_puts("SES", "OKUMA_BASARILI", rec_TERM.SES_OKUMA_OK, AYARLAR_INI);
    ini_puts("SES", "OKUMA_BASARISIZ", rec_TERM.SES_OKUMA_NK, AYARLAR_INI);
    ini_puts("SES", "IZIN_VERILDI", rec_TERM.SES_IZIN_OK, AYARLAR_INI);
    ini_puts("SES", "OKUMA_BASARILI", rec_TERM.SES_IZIN_NK, AYARLAR_INI);
    ini_puts("SES", "TANIMSIZ_KART", rec_TERM.SES_TANIMSIZ, AYARLAR_INI);

    ini_puts("MESAJ", "IZIN_VERILDI", rec_TERM.MSJ_IZIN_OK, AYARLAR_INI);
    ini_puts("MESAJ", "IZIN_VERILMEDI", rec_TERM.MSJ_IZIN_NK, AYARLAR_INI);
    ini_puts("MESAJ", "TANIMSIZ_KART", rec_TERM.MSJ_TANIMSIZ, AYARLAR_INI);

    ini_putl("SURELER", "MESAJ_GOSTERIM", rec_TERM.MSJ_SURE_SN, AYARLAR_INI);
    ini_putl("SURELER", "ONLINE_DENEME", rec_TERM.ONLINE_TEKRAR_DK, AYARLAR_INI);
    ini_putl("SURELER", "SAAT_GUNCELLEME", rec_TERM.TS_GUNCELLE_SAAT, AYARLAR_INI);
    ini_putl("SURELER", "KART_LOG_GONDERME", rec_TERM.LOG_KART_SAAT , AYARLAR_INI);
    ini_putl("SURELER", "ALARM_LOG_GONDERME", rec_TERM.LOG_ALARM_SAAT, AYARLAR_INI);
    ini_putl("SURELER", "TERMINAL_LOG_GONDERME", rec_TERM.LOG_TERM_SAAT, AYARLAR_INI);

    //2.4.0
    ini_putl("SURELER", "KAPI_TCP_BEKLEME_SURESI", KAPI_TCP_WAIT_TIME, AYARLAR_INI);

    ini_putl("YAZICI", "AKTIF", rec_YAZICI.AKTIF, AYARLAR_INI);
    ini_puts("YAZICI", "ACIKLAMA1", rec_YAZICI.ACIKLAMA1, AYARLAR_INI);
    ini_puts("YAZICI", "ACIKLAMA2", rec_YAZICI.ACIKLAMA2, AYARLAR_INI);
    ini_puts("YAZICI", "ACIKLAMA3", rec_YAZICI.ACIKLAMA3, AYARLAR_INI);
    ini_putl("YAZICI", "TIPI", rec_YAZICI.TIP, AYARLAR_INI);
    ini_putl("YAZICI", "KARTNO_BAS", rec_YAZICI.KART_NO_BAS , AYARLAR_INI);
    //7.5.2
    ini_putl("YAZICI", "SIRANO_BAS", rec_YAZICI.SIRA_NO_BAS , AYARLAR_INI);
    ini_putl("YAZICI","TEKNUSHA", rec_YAZICI.TEKNUSHA, AYARLAR_INI); //yoksa ya da 0 ise 2 nusha basar, varsa ve 1 ise tek nusha basar

    ini_putl("KEYPAD","AKTIF", KEYPAD_AKTIF, AYARLAR_INI);
    //1.7.0
    ini_putl("KEYPAD","KART_SIFRE_AKTIF", KART_SIFRE_AKTIF, AYARLAR_INI);

    ini_putl("OGUNKONTROLU","KONTROLLU", rec_Ayarlar.Grup_ID, AYARLAR_INI); //1=> Kontrollu, 0=> Kontrolsuz

    ini_putl("HAVUZ", "AKTIF", rec_HAVUZ.AKTIF, AYARLAR_INI);
    ini_putl("HAVUZ", "CARPANI", 1, AYARLAR_INI);
    ini_putl("HAVUZ","GIRIS", rec_HAVUZ.CARPANI, AYARLAR_INI); //1=> Giris, 0=> Cikis
    ini_putl("HAVUZ", "CEZASI", rec_HAVUZ.CEZA , AYARLAR_INI);

//memset(szBuf, 0, 7);
//memcpy(szBuf,rec_TERM.KEY_PERSONEL, 6);
//ini_puts("SIFRELER", "PERSONEL", (const char *)szBuf, AYARLAR_INI);

//memset(szBuf, 0, 7);
//memcpy(szBuf,rec_TERM.KEY_MASTER, 6);
//ini_puts("SIFRELER", "MASTER", (const char *)szBuf, AYARLAR_INI);

//memset(szBuf, 0, 7);
//memcpy(szBuf,rec_TERM.KEY_BAKIYE, 6);
//ini_puts("SIFRELER", "DUSME", (const char *)szBuf, AYARLAR_INI);

//memset(szBuf, 0, 7);
//memcpy(szBuf,rec_TERM.KEY_TOPUP, 6);
//ini_puts("SIFRELER", "YUKLEME", (const char *)szBuf, AYARLAR_INI);

    ini_puts("SIFRELER", "ADMIN", rec_TERM.ADMIN_PASS, AYARLAR_INI);

    //7.6.0
    ini_putl("YEMEK_LIMITLERI", "AKTIF", rec_LIMIT.AKTIF, AYARLAR_INI);
    ini_putl("YEMEK_LIMITLERI", "TURNIKE_LIMIT", rec_LIMIT.TURNIKE, AYARLAR_INI);
    ini_putl("YEMEK_LIMITLERI", "MISAFIR_LIMIT", rec_LIMIT.MISAFIR, AYARLAR_INI);

    remove(MASTER_KEY);
    //0.2.2
    //para terminaliyse keyleri saklama.
    if(rec_TERM.TERM_TIP != TERM_PARA)
    {
        out=fopen(MASTER_KEY,"a+b");
        fwrite(rec_TERM.KEY_BAKIYE,6,1,out);
        fwrite(rec_TERM.KEY_TOPUP,6,1,out);
        fwrite(rec_TERM.KEY_PERSONEL,6,1,out);
        fclose(out);
    }
    if((Para_Sektor >3) && (Para_Sektor < 10)) //0.1.0 standard sektorlerse sektor.dat yapma
    {
        //5.7.0 sektor bilgilerini yazmak icin
        remove(SEKTOR_DAT);
        out = fopen(SEKTOR_DAT,"a+b");
        fwrite(&Para_Sektor,1,1,out);
        fwrite(&Personel_Sektor,1,1,out);
        fclose(out);
    }
}

