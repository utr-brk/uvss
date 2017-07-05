#ifndef PERSONEL_H_INCLUDED
#define PERSONEL_H_INCLUDED


//#include <dos.h>
#include "main.h"
#pragma pack(push,1)
/*
typedef struct
{
	uint16_t da_year;
	char 	da_day;
	char 	da_mon;
} struct_date;

typedef struct
{
	unsigned char ti_min;
	unsigned char ti_hour;
	unsigned char ti_hund;
	unsigned char ti_sec;
} struct_time;
*/
typedef struct
	{
		uint32_t OGUN_ID; //4 Byte
		struct_date TARIH;	 //4 Byte
		char SAYI;		 //1 Byte //m8110 unsigned idi
	}
	struct_YEMEKBILGI;

//6.0.0

typedef struct
	{
		struct_date		BASLANGIC;		//4 Byte Uyelik Baslangic Tarihi
		struct_date		BITIS;			//4 Byte Uyelik Bitis Tarihi
		char			SNRSZ;			//1 Byte Sinirsiz Uyelik:1
		char			CINSIYET;		//1 Byte Cinsiyet: K, E, C
		uint16_t		SEANS_ID;		//2 Byte Girdigi Seans ID //m8110 int idi unsigned yapýldý
		uint32_t		FIYAT;			//4 Byte Girdigi Seansin Fiyati //m8110 long idi unsigned long yapildi
		//6.4.0
		struct_date		GIRIS_TARIHI;	//4 Byte Havuza Giris Yaptigi Tarih
		char			IN_OUT; 		//1 Byte 1=> Iceride, 0=> Disarida
	}
	struct_HAVUZ_BILGI;

typedef struct
	{
		int_least16_t	ID;
		int_least16_t 	GRUP[100];
		int_least16_t	GRUP_FIYAT[100][2];//6.4.0 ikinci fiyat eklendi, tek kullanimlik
		int_least16_t 	FIYAT;
		char			CINSIYET[10];//K,E,C
	}
	struct_SEANS_BILGI;

//brk
//Vakifbank icin eklendi
//02/08/2012
typedef struct
		{
        char KARTNO[4];//m8110 unsigned idi
        uint32_t TOPUP;
        uint16_t  SIRANO;
        uint16_t  GRUP;
    }
    struct_TOPUP;

typedef struct
    {
        char KARTNO[4];//m8110 unsigned idi
        uint32_t TOPUP;
        uint16_t  SIRANO;
        uint16_t  GRUP;
    }
    struct_HAVUZ_TOPUP;

typedef struct
	{
		char KARTNO[4];//m8110 unsigned idi
		//unsigned char KARTNO_HEX[9]; m8110
		char KARTNO_HEX[9];//m8110 unsigned idi
		char ADI[17];//m8110 unsigned idi
		char SOYAD[17];//m8110 unsigned idi
		char UNVAN[17];//m8110 unsigned idi
		char GRUP[4];//m8110 unsigned idi
		uint32_t HARCANAN;
		uint32_t BAKIYE;
		struct_date   OKUMA_TARIHI;
		struct_time	  OKUMA_SAATI;
		struct_YEMEKBILGI YB;
		uint32_t OGV;
		struct_date     REFAKAT_SKT;
		struct_date     PDKS_TARIHI;
	}
	struct_PERSONEL;

typedef struct
	{
		char CEVAP;//m8110 hepsi unsigned char idi
		char MESAJ;
		char MSJ_LINE_1[21];
		char MSJ_LINE_2[21];
		char MSJ_SES[11];
	}
	struct_IZINCEVAP;

typedef struct
	{
		char GUN; //m8110 hepsi unsigned char idi
		char SAAT_BASLA;
		char DAKIKA_BASLA;
		char SAAT_BITIR;
		char DAKIKA_BITIR;
	}
	struct_ZAMANDETAY;

typedef struct
	{
		char GRUP_KODU[4];//m8110 hepsi unsigned char idi
		char ZAMAN_KODU[4];
		char HAFTA_PLAN[7];
		struct_ZAMANDETAY DETAY[200];
	}
	struct_GRUPGECIS;

typedef struct
	{
		uint32_t ID;
		char	SAAT_BASLA;//m8110 hepsi unsigned char idi
		char 	DAKIKA_BASLA;
		char 	SAAT_BITIR;
		char 	DAKIKA_BITIR;
	}
	struct_OGUN;

typedef struct
	{
		char	GRUP_KODU[4];//m8110 unsigned idi
		uint32_t		OGUN_ID;
		char 	HAK;//m8110 unsigned idi
		uint32_t 		FIYAT[3];
	}
	struct_GRUPOGUN;


typedef struct
	{
		char 	HAK;//m8110 unsigned idi
		uint32_t 		FIYAT;
	}
	struct_YEMEKHAK;

typedef struct
{
    char HATA;
    char KARTNO[9];
	struct_YEMEKBILGI YB;
    uint32_t HARCAMA;
    uint32_t BAKIYE;
    char OK_B;
}
struct_HATA;

typedef struct
{
		char 				HATA;
		char 				KARTNO[9];
		struct_HAVUZ_BILGI 	HB;
		uint32_t 			HARCAMA;
		uint32_t 			BAKIYE;
		char 				OK_B;
		//6.4.0
		struct_date			GIRIS_TARIHI;
		char				IN_OUT;
}
struct_HAVUZ_HATA;

typedef struct
{
	char HATA;
	char KARTNO[9];//m8110 unsigned idi
	unsigned char OGV[16];//m8110 unsigned idi
	unsigned char YB[16];//m8110 unsigned idi
	uint16_t  FILEGRUP;
	uint32_t TOPUP;
	uint32_t BAKIYE;
}
struct_TOPUP_ERR;

typedef struct
{
	char HATA;
	char KARTNO[9];//m8110 unsigned idi
	unsigned char OGV[16];//m8110 unsigned idi
	unsigned char YB[16];//m8110 unsigned idi
	uint16_t  FILEGRUP;
	uint32_t TOPUP;
	uint32_t BAKIYE;
}
struct_HAVUZ_TOPUP_ERR;

//7.0.0
typedef struct
{
	char KART_NO[8];
	char ADI[30];
	char SOYADI[30];
}
struct_KART_Config;

extern struct_KART_Config KART_Config;


extern struct_TOPUP_ERR TopUP_ERR;
extern struct_HAVUZ_TOPUP_ERR Havuz_TopUP_ERR;

extern struct_HATA Hata;
extern struct_PERSONEL rec_Personel;
extern struct_TOPUP rec_TopUp;
extern struct_HAVUZ_TOPUP rec_Havuz_TopUp;
extern struct_HAVUZ_HATA Havuz_Hata;
extern struct_SEANS_BILGI rec_Seans;
extern struct_HAVUZ_BILGI rec_HB;

//1.0.0
extern char Barcode_Fis[18];

extern void PERSONEL_Karsila(void);
extern unsigned long PERSONEL_YemekOgun_EkranAc(void);
//7.0.0
extern void REZERVASYON_Karsila(void);

//2.3.1
//extern int get_keyms(int timeout);
#pragma pack(pop)

#endif // PERSONEL_H_INCLUDED
