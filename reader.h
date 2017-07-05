#ifndef READER_H_INCLUDED
#define READER_H_INCLUDED
#pragma pack(push,1)
#define reader_name  "/dev/nfc"

#define MFR_ERR_NONE       	0x00
#define MFR_ERR_CHKSUM     	0xF2
#define MFR_ERR_SEND       	0xF3
#define MFR_ERR_RECIEVE    	0xF4
#define MFR_ERR_TYPE       	0xF5
#define MFR_ERR_KEY	       	0xF6
#define MFR_ERR_CARDNONE   	0x01
#define MFR_ERR_CARDERR		0xFF

#define READER_MIFARE       0x01
#define READER_BARCODE      0X02
//1.8.0
#define READER_RFID         0x03

//2.0.4
typedef struct{
	char KART_NO[21];
	char SICIL_NO[21];
	char ADISOYADI[21];
	char TZ_INOUT;
	char TZ_ONOFF;
	char MSJ_NO;
} PDKS_KART_Config;

//2.4.0
extern PDKS_KART_Config KART;

extern void READER_Init(void);
extern char READER_Read(void);
//extern char READER_Write(char * KartNo);//m8110 unsigned idi
extern char READER_Write(int Okuyucu);//m8110 unsigned idi
//extern char READER_Havuz_Write(char * KartNo);
extern char READER_Havuz_Write(int Okuyucu);
extern void READER_Uninit(int COM_Port);
extern int	COM_Init(void);

extern char READER_TYPE;
extern unsigned char POS_ReadCardNumber(char *KartNo ,char  *KartNoHex);//m8110 unsigned idi
extern unsigned char POS_ReadPersonel(char * KartNo);//m8110 unsigned idi
extern char POS_ReadMaster(void);//m8110 unsigned char idi
extern unsigned char POS_WritePersonel(char * KartNo);//m8110 unsigned idi
extern unsigned char POS_WriteHavuz(char *KartNo);

//7.0.0
extern char READER_Rezervasyonlu_Read(void);
//2.4.0
// Personel Bilgisi
extern char PDKS_KartBul (char* KartNo,PDKS_KART_Config *KART,char MSJSIL);

extern int ifd[2];
extern int pinpad;

#pragma pack(pop)

#endif // READER_H_INCLUDED
