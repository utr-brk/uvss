#ifndef TCP_H_INCLUDED
#define TCP_H_INCLUDED

#include "personel.h"
//#include "dos.h"
#include "log.h"
//#include "posapi.h"
#include "main.h"
#pragma pack(push,1)


typedef struct
{
	uint16_t 		Command;
	uint16_t 		Len;
	unsigned char	CheckSum;
	unsigned char	Data[512];
} struct_TCP_Packet;

typedef struct
{
	unsigned char	KARTNO[4];
	struct_date		OKUMA_TARIH;
	struct_time		OKUMA_ZAMAN;
	uint32_t		OGUN_ID;
	uint32_t		BAKIYE;
	uint32_t		TOPLAM_YUKLENEN;
	uint32_t		TOPLAM_HARCANAN;
}
struct_TCP_Hareket;


int TCP_Connect(const char * addr, int port);
extern char TCP_GetDateTime(struct_date *d, struct_time *t);
extern void TCP_SendData(struct_TCP_Packet * sndPacket);
extern char TCP_RecieveData(struct_TCP_Packet * rcvPacket,long toMiliSec);
extern char TCP_PersonelIzinAl(struct_PERSONEL * recPersonel,unsigned char *Response);
extern char TCP_PersonelHareketIsle(struct_PERSONEL * recPersonel,unsigned char *Response);
extern char TCP_LogKart(struct_LOG_KART *recLOG);
extern char TCP_LogPara(struct_LOG_PARA *recLOG);
extern char TCP_PersonelParaIsle(struct_TCP_Para * recPara, unsigned char *Response);

#pragma pack(pop)

#endif // TCP_H_INCLUDED
