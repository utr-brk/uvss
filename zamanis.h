#ifndef ZAMANIS_H_INCLUDED
#define ZAMANIS_H_INCLUDED
#pragma pack(push,1)

/*
extern unsigned char z_ONLINE_TEKRAR_DK;
extern unsigned char z_TS_GUNCELLE_SAAT;
extern unsigned char z_LOG_KART_SAAT;
extern unsigned char z_LOG_ALARM_SAAT;
extern unsigned char z_LOG_TERM_SAAT;
*/
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
extern void ZAMANIS_Init(void);
extern void ZAMANIS_Kontrol(void);

extern char reboot;
#pragma pack(pop)

#endif // ZAMANIS_H_INCLUDED
