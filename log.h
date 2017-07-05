#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED

//#include <dos.h>
#include "main.h"
#pragma pack(push,1)
#define LOG_KART_MAX	100000
#define LOG_ALARM_MAX 	1000
#define	LOG_TERM_MAX	1000
#define LOG_PARA_MAX 	100000



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
    unsigned long   ILK;
    unsigned long   SON;
    unsigned long	SAY;
}
struct_LOG_INFO;

typedef struct
{
    char 			KART_NO[4]; //m8110 unsigned char idi
    char            TERM_TIP;
    char			OKUYUCU;
    struct_date		OKUMA_TARIH;
    struct_time		OKUMA_ZAMAN;
    char			CEVAP;
    char			CEVAP_ONLINE;
    char			GECTI;
    char			GECTI_ONLINE;
    uint32_t		KART_HARCANAN; //m8810 unsigned long idiler
    uint32_t		KART_BAKIYE;
    uint32_t   		KART_OGV;
}
struct_LOG_KART;

typedef struct
{
	char 			KART_NO[4]; //m8110 unsigned char idi
	struct_date		OKUMA_TARIH;
	struct_time		OKUMA_ZAMAN;
	char			CEVAP;
	char			CEVAP_ONLINE;
	char			ISLEM;
	uint32_t		BAKIYE; //m8110 unsigned long idiler
	uint32_t		TUTAR;
}
struct_LOG_PARA;


typedef struct
{
    char			ALARM_NO;
    char			DURUM;
    struct_date		TARIH;
    struct_time		ZAMAN;
}
struct_LOG_ALARM;

typedef struct
{
    unsigned char	TIP; //0-Network 1-Okuyucu
    unsigned char	DURUM; //0-OFF 1-ON
    unsigned char	OKUYUCU; //Okuyucu Numaras
    struct_date		TARIH;
    struct_time		ZAMAN;
}
struct_LOG_TERM;



extern char flag_LOG_KART;
extern char flag_LOG_PARA;

extern char LOG_Add_PARA(struct_LOG_PARA *rlPARA);
extern char LOG_Read_PARA(struct_LOG_PARA *rlPARA);
extern char LOG_Del_PARA(void);
extern void LOG_Run_Para(void);

extern void LOG_Init(void);
extern char LOG_Add_KART(struct_LOG_KART *rlKART);
extern char LOG_Read_KART(struct_LOG_KART *rlKART);
extern char LOG_Del_KART(void);
extern char LOG_Add_TERM(struct_LOG_TERM *rlTERM);
extern char LOG_Read_TERM(struct_LOG_TERM *rlTERM);
extern char LOG_Del_TERM(void);
extern char LOG_Add_ALARM(struct_LOG_ALARM *rlALARM);
extern char LOG_Read_ALARM(struct_LOG_ALARM *rlALARM);
extern char LOG_Del_ALARM(void);
extern void LOG_Run(void);

extern char Log_Run_Kapi(void);


#pragma pack(pop)
#endif // LOG_H_INCLUDED
