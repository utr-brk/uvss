#ifndef SMM_H_INCLUDED
#define SMM_H_INCLUDED
#include <stdint.h>
#pragma pack(push,1)

#define SMM_ERR_NONE       0x00
#define SMM_ERR_CHKSUM     0xF2
#define SMM_ERR_SEND       0xF3
#define SMM_ERR_RECIEVE    0xF4
#define SMM_ERR_TYPE       0xF5
#define SMM_ERR_KEY	       0xF6
#define SMM_ERR_CARDNONE   0x01
#define SMM_ERR_CARDERR    0xFF

//1.6.0
#define SMM_ERR_SHUTDOWN    0x11
#define SMM_ERR_REBOOT      0x22
//2.2.5
#define SMM_ERR_DATETIME    0x23

//1.7.0
#define SMM_ERR_SIFRE       0x33
#define SMM_ERR_UNV         0x44

#define SMM_RESET 			0x80
#define SMM_FIRMWARE		0x81
#define SMM_SEEK			0x82
#define SMM_SELECT			0x83
#define SMM_AUTH			0x85
#define SMM_READBLOCK		0x86
#define SMM_READVALUE		0x87
#define SMM_WRITEBLOCK	    0x89
#define SMM_WRITEVALUE	    0x8A
#define SMM_INCREMENT		0x8D
#define SMM_DECREMENT		0x8E
#define SMM_ANTENNA			0x90
#define SMM_READPORT		0x91
#define SMM_WRITEPORT		0x92
#define SMM_HALT			0x93

typedef struct
	{
		unsigned char low;
		unsigned char hi;
	}
	WORD_Struct;

typedef struct
	{
		unsigned char LEN;
		unsigned char CMD;
		unsigned char DATA[255];
		unsigned char CRC;
	}
	struct_SMM_PACKED;


extern unsigned char SMM_Ok(int Comport);
extern unsigned char SMM_ReadCardNumber(int ComPort,unsigned char *KartNo,unsigned char *KartNoHex);
extern unsigned char SMM_Login(unsigned char Sector,unsigned char TIP,unsigned char *KEY);
extern unsigned char SMM_WriteBlock(unsigned char Sector,unsigned char Block,unsigned char *DATA);
extern unsigned char SMM_WriteValue(unsigned char Sector,unsigned char Block,uint32_t *VALUE);
extern unsigned char SMM_ReadBlock(unsigned char Sector,unsigned char Block,unsigned char *DATA);
extern unsigned char SMM_ReadValue(unsigned char Sector,unsigned char Block,uint32_t *VALUE);
extern unsigned char SMM_Increment(unsigned char Sector,unsigned char Block,uint32_t *VALUE);
extern unsigned char SMM_Decrement(unsigned char Sector,unsigned char Block,uint32_t *VALUE);
extern unsigned char SMM_ReadMaster(int ComPort);
extern unsigned char SMM_ReadPersonel(int ComPort);
extern unsigned char SMM_WritePersonel(int ComPort);
extern unsigned char SMM_WriteHavuz(int ComPort);

extern unsigned char SMM_WritePacked(int Port,struct_SMM_PACKED *Packed);
extern unsigned char SMM_ReadPacked(int Port,struct_SMM_PACKED *Packed);

//1.8.0
extern void Str2Hex(unsigned char *in, int len, unsigned char *out);

//2.0.0
extern int SMM_PORT;

#pragma pack(pop)


#endif // SMM_H_INCLUDED
