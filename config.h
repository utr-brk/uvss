#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include <gtk/gtk.h>

#pragma pack(push,1)

#define	TERM_YEMEKHANE	0x00
#define	TERM_KAPI		0x01
#define TERM_PARA       0x02
#define TERM_HARCAMA    0x03
//0.0.1
//TERM_BAKIYE           0x04
//TERM_SOSYALTESIS      0x05
#define TERM_UVSS       0x06

//aslinda buraya havuz giris ve
//havuz cikis terminalleri eklenebilirdi.

//1.8.0
#define TIP_MIFARE      0x0
#define TIP_RF_ID       0x1
//2.0.0
#define TIP_CN_MFR      0x2
#define TIP_CN_KYB      0x3

typedef struct
{
    char iface[16];
    int dhcp;
    char IP[16];
    char GW[16];
    char Mask[16];
    char Dns1[16];
    char Dns2[16];
} IPCONFIG_t;

typedef struct
{
    char	AKTIF;
    unsigned char	ROLE;
    unsigned char	ROLE_SN;
    unsigned char	SENS;
    unsigned char   SENS_SN;
} struct_READER;

typedef struct
{
    char			KOD[4];
    char			ACIKLAMA[51];
    char			TERM_TIP;
    char			KAPI_KOD[4];
    char			IP_TERM[16];
    char			IP_SUBNET[16];
    char			IP_GATEWAY[16];
    char			IP_SERVER[16];
    unsigned long	PORT_SERVER;
    struct_READER	READERS[2];
    unsigned char	SENS_CEVAP;
    unsigned char	ALARM_SENS[2];
    char			SES_OKUMA_OK[11];
    char			SES_OKUMA_NK[11];
    char			SES_IZIN_OK[11];
    char			SES_IZIN_NK[11];
    char			SES_TANIMSIZ[11];
    char			MSJ_IZIN_OK[21];
    char			MSJ_IZIN_NK[21];
    char			MSJ_TANIMSIZ[21];
    unsigned char	MSJ_SURE_SN;
    unsigned char	ONLINE_TEKRAR_DK;
    unsigned char	TS_GUNCELLE_SAAT;
    unsigned char	LOG_KART_SAAT;
    unsigned char	LOG_ALARM_SAAT;
    unsigned char	LOG_TERM_SAAT;
    char			ADMIN_CARD[11];
    char			ADMIN_PASS[5];
    unsigned char	KEY_PERSONEL[6];
    unsigned char	KEY_MASTER[6];
    unsigned char	KEY_BAKIYE[6];
    unsigned char	KEY_TOPUP[6];
} struct_TERM;

typedef struct
{
    char		AKTIF;
    //6.2.0 gereksizdi, kaldirdim.
//	unsigned char	ROLE;
//	unsigned char	ROLE_SN;
//	unsigned char	SENS;
//	unsigned char SENS_SN;
    char		GIRIS; 		//1=>Giris, 0=> Cikis
    int			FIYAT; 		//sifirdan kucukse seans fiyatini al, sifir ve yukarisi ise bu fiyati al
    int			CARPANI; 	//girerken kac kati para olsun kartta
    unsigned int 	CEZA; 		//6.4.0 cikis yapmazsa ertesi gun kesilecek ceza
} struct_HAVUZ; //6.0.0

typedef struct
{
    char	AKTIF;
    char 	TIP; //POS/CUSTOM
    char  	ACIKLAMA1[24];
    char  	ACIKLAMA2[24];
    char  	ACIKLAMA3[24];
    //int  	KART_NO_BAS;//6.3.1
    //int		TIP; //0: POS, 1: EXTERNAL
    //char 	KART_NO_BAS[10]; //BAS/BASMA
    char 	KART_NO_BAS;//1:BAS, 0:BASMA
    char 	SIRA_NO_BAS;//1:BAS, 0:BASMA
    //char 	TIP[10]; //POS/CUSTOM

    char	TEKNUSHA; //0: 2 Nusha, 1: 1 Nusha bas //0.2.0
} struct_YAZICI; //6.1.0

typedef struct
{
    char	AKTIF;
    int		TIP; //reserved for future use:)
} struct_PINPAD;

//7.4.0
typedef struct
{
    char	Grup_ID; //1=>Kontrollu, 0=>Kontrolsuz
    char	Refakatci_Kontrol;//7.6.2
    char	Giris_Kontrol;//7.6.2
} struct_AYARLAR;

//7.6.0
//TURNIKE: 0'dan buyukse o gn satilacak max yemek sayisi
//MISAFIR: 0'dan buyukse, kacinci yemekte misafir fiyati
//icin onay istenecegi
typedef struct
{
    char AKTIF;
    int	 TURNIKE;
    int  MISAFIR;
} struct_LIMITS;

//3.0.0
typedef struct
{
    //char			AKTIF;
    char			IP_UVSS[16];
    unsigned long	PORT_UVSS;
} struct_UVSS;

extern struct_AYARLAR rec_Ayarlar;

extern struct_PINPAD	rec_PINPAD;
extern struct_YAZICI 	rec_YAZICI;
extern struct_HAVUZ 	rec_HAVUZ;
extern struct_TERM 		rec_TERM;
//3.0.0
extern struct_UVSS 		rec_UVSS;


extern char				net_ONLINE;
extern char				net_ONLINE_PARA;//0.0.7


extern unsigned char	Personel_Sektor;
extern unsigned char	Para_Sektor;
extern unsigned char	Para_Blok;
extern unsigned char	YB_Sektor;
extern unsigned char	YB_Blok;

//6.0.0
extern unsigned char	HB_Blok; //Havuz Bilgisi Blogu

//7.6.0
extern struct_LIMITS	rec_LIMIT;

extern unsigned         okuma_bekleme;
extern int              refakatci[128];
extern int              kontrolsuz[128];
extern int              rezervasyon;

extern void				CONFIG_Load(void);
extern void				CONFIG_Save(void);


extern  GtkWidget *window;
extern  GtkWidget *main_layout;

extern  GtkWidget *label1;
extern  GtkWidget *label2;
extern  GtkWidget *label3;
extern  GtkWidget *label4;
extern  GtkWidget *label5;
extern  GtkWidget *label_status;
extern  GtkWidget *logo;
extern  GtkWidget *infobarr;
extern  GtkWidget *content_area;

//0.60
extern gint window_width, window_height;
extern gint logo_x, logo_y;
extern gint logo_x_top, logo_y_top;
extern gint label_x, label_y;
extern gint label_width, label_height;
extern gint infobar_x, infobar_y;

//0.61
extern char *small_font_name;
extern char *big_font_name;

//1.0.0
extern char KEYPAD_AKTIF;
extern int  Okuyucu_HIZ[2];

//1.7.0
extern char KART_SIFRE_AKTIF;
extern char Kart_Sifre[10];

extern char OKUYUCU_SIFRE_AKTIF[2];

//1.8.0
extern int             Okuyucu_TIP[2];
extern int             RF_ID_MASK_AKTIF;
extern char            RF_ID_MASK[13];

//2.1.0
extern char ISKUR;

//2.2.1
extern char TERM_AKTIF;

//1.0.5
extern  GtkWidget *txtBarcode;
//0.54
//extern  GtkWidget *cancel_logo;

//2.4.0
extern int KAPI_TCP_WAIT_TIME;

#pragma pack(pop)
#endif // CONFIG_H_INCLUDED
