#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "smm.h"
#include "main.h"
#include "com.h"
//#include "acctimer.h"
#include "serport.h"
#include "mesaj.h"
//#include "resource.h"
#include "personel.h"
#include "tool.h"
#include "config.h"
#include "reader.h"
#include "lcd.h"
//2.0.0
#include "cn56b.h"
//2.2.2
#include "minini.h"
#define   HATA_LOG "/home/odroid/Solidus/Hata.Log"
int SMM_PORT;
//2.2.0
//unsigned int SMM_COUNTER=0;
//2.5.1
unsigned int SMM_COUNTER=0;

//2.3.1
/*
extern unsigned char Personel_Sektor;
extern unsigned char Para_Sektor;
extern unsigned char Para_Blok;
extern unsigned char YB_Sektor;
extern unsigned char YB_Blok;
extern unsigned char HB_Blok;
*/

void Str2Hex(unsigned char *in, int len, unsigned char *out)
{
int i;
char *cHex;
char *sHex;
	sHex = (char*) malloc(len*2+1);
	cHex = (char*) malloc(5);
	memset(sHex,0x00,len*2+1);
	for (i=0;i<len;i++)
	{
		sprintf(cHex,"%02X",in[i]);
		strcat(sHex,cHex);
	}
	memcpy(out,sHex,len*2+1);
	free(sHex);
	free(cHex);
}

unsigned char SMM_ReadPacked(int Port,struct_SMM_PACKED *Packed)
{
	unsigned int max,crc;
	unsigned char i;
	WORD_Struct wcrc;
	int c;
	char* sz;
	unsigned char* szHex;

    //2.0.0
    if((Okuyucu_TIP[Port] == 2)||(Okuyucu_TIP[Port] == 3)){
        return CN56B_ReadPacked(Port, Packed);
    }
	memset(Packed,0x00,sizeof(struct_SMM_PACKED));

	max=COM_avail(Port,500);//odroid 20000 idi
//	printf("read_packed_max: %d\n", max);
//5.5.0
//	if (!max) return SMM_ERR_RECIEVE;
//5.5.1
	if (!max) return SMM_ERR_CARDNONE;

	sz=( char*) calloc(512, sizeof(char));
	//memset(sz,0x00,512);
	szHex=(unsigned char*) calloc(512, sizeof(char));
	//memset(szHex,0x00,512);

	//max=COM_ReadBuffer(Port,sz);

	//5.5.1
	//SerPort_RecvByte(Port,&c);
	//odroid
	//if(!SerPort_RecvByte(Port,&c))

	c = serialGetchar(Port);
	if(c == -1)
		printf("serport recv error\n");
	sz[0]= ( char)c;
	//odroid
	//SerPort_RecvByte(Port,&c);
	c = serialGetchar(Port);
	sz[1]=( char)c;

	if ((sz[0]!=0xFF)||(sz[1]!=0x00)) //Stx Etx yoksa bozuk
	{
		free(sz);
		free(szHex);
		return SMM_ERR_CHKSUM;
	}

	//SerPort_RecvByte(Port,&c);
	c = serialGetchar(Port);
	sz[2]=( char) c;
	for (i=0;i<sz[2]+1;i++)
	{
		//if (!COM_avail(Port,5)) break;
		//SerPort_RecvByte(Port,&c);
		c = serialGetchar(Port);
		sz[i+3]=( char)c;
	}

	//2.5.1
	//max=(sz[2])+4;
	max=(sz[2])+3;

	//Str2Hex(sz,max,szHex);
	//printf("--%s\r\n",szHex);
	crc = 0;
	//2.5.1
	//for (i=2;i<max-1;i++)
	for (i=2;i<max;i++)
		crc += sz[i];
	memcpy(&wcrc,&crc,2);

	Packed->LEN = sz[2];
	Packed->CMD = sz[3];
	memcpy(Packed->DATA,&sz[4],Packed->LEN-1);
	//2.5.1
	//Packed->CRC = sz[max-1];
	Packed->CRC = sz[max];

	//2.5.1
	//if (wcrc.low==sz[max-1])
	if (wcrc.low==sz[max])
	{
		free(sz);
		free(szHex);
		return SMM_ERR_NONE;
	} else {
		free(sz);
		free(szHex);
		return SMM_ERR_CHKSUM;
	}
}

unsigned char SMM_WritePacked(int Port,struct_SMM_PACKED *Packed)
{

 WORD_Struct wcrc;
 unsigned int i,crc;

 //5.5.1
 //SerPort_SendByte(Port, 0xFF);
// SerPort_SendBuf(Port, "Bunu Yazan Tosun\n", 17);
 //odroid
 //if(!SerPort_SendByte(Port, 0xFF))

    //2.0.0
    if((Okuyucu_TIP[Port] == 2)||(Okuyucu_TIP[Port] == 3)){
        CN56B_WritePacked(Port, Packed);
        return SMM_ERR_NONE;
    }

 serialPutchar(Port, 0xff);
	//printf("serport send error\n");

 serialPutchar(Port, 0x0);
 crc=0;
 serialPutchar(Port, Packed->LEN);
 crc+=Packed->LEN;
 serialPutchar(Port, Packed->CMD);
 crc+=Packed->CMD;
 for (i=0;i<Packed->LEN-1;i++)
 {
	serialPutchar(Port,Packed->DATA[i]);
	crc+=Packed->DATA[i];
 }
 memcpy(&wcrc,&crc,2);
 Packed->CRC = wcrc.low;
 serialPutchar(Port, Packed->CRC);
 return SMM_ERR_NONE;
}



unsigned char SMM_ReadCard_Tmp(unsigned char *KartNo)
{
	unsigned char ret;
	struct_SMM_PACKED Packed;
	//2.2.0
	//static unsigned char smm_RESET[2]={0,0};
	//2.5.1
	static unsigned char smm_RESET[2]={0,0};
//	static unsigned char smm_SEEK[2]={0,0}; //5.6.0 seek etmiyor artk


   /* int qq = 0;
	while(qq != 'A'){
        if(COM_avail(ifd[SMM_PORT], 2000)){
            qq = serialGetchar(ifd[SMM_PORT]);
            lcd_printf_ex(ALG_LEFT, 7, "In: %c", qq);
            serialPutchar(ifd[SMM_PORT], qq);
            lcd_flip();
        }
    }
*/
    //2.2.0
	//2.5.1
	if (!smm_RESET[SMM_PORT])
	{
		Packed.LEN = 1;
		Packed.CMD = SMM_RESET;
		SMM_WritePacked(ifd[SMM_PORT],&Packed);
//		delay(1000);
		//if(!COM_avail(com_port,10))
		//	return 0x00;
		//6.2.0
		COM_avail(ifd[SMM_PORT],500);
//		printf("read_card_max: %d\n",COM_avail(SMM_PORT,15000) );
		//while(1)
		SMM_ReadPacked(ifd[SMM_PORT],&Packed);
		smm_RESET[SMM_PORT]=1;
		//lcd_printf(ALG_CENTER, "SMM(%d) RESET", SMM_PORT);
		//MSJ_Ses("2121");
		//smm_SEEK[SMM_PORT]=0;//5.6.0 Seek Etmiyor artik
		//5.7.0 burayi test et
		//aþaðýda 5000'lik counter'i kucultup, iki okuyucu ile test et
		//printf("SMM_RESET[%d]: %s\r\n", SMM_PORT+1, Packed.DATA);
	}


/*	if (!smm_SEEK[SMM_PORT])
	{

		Packed.LEN = 1;
		Packed.CMD = SMM_HALT; //Halt;
		SMM_WritePacked(SMM_PORT,&Packed);
		SMM_ReadPacked(SMM_PORT,&Packed);
		Packed.LEN = 1;
		Packed.CMD = SMM_SEEK; //Seek Tag;
		SMM_WritePacked(SMM_PORT,&Packed);
		SMM_ReadPacked(SMM_PORT,&Packed);
		smm_SEEK[SMM_PORT]=1;
		printf("SMM_SEEK\r\n");
	}
*/

    //2.2.0
    memset(&Packed, 0, sizeof(struct_SMM_PACKED));
    //2.5.1
    /*
    //1.6.0
    Packed.LEN = 1;
	Packed.CMD = SMM_HALT; //Seek Tag;
	SMM_WritePacked(ifd[SMM_PORT],&Packed);
	//6.2.0
	COM_avail(ifd[SMM_PORT],500);
	ret=SMM_ReadPacked(ifd[SMM_PORT],&Packed);
	if((ret==SMM_ERR_NONE)&&(Packed.LEN==2) && (Packed.DATA[0] == 'L'))
		ret = SMM_ERR_NONE; //no card
    */
	Packed.LEN = 1;
	Packed.CMD = SMM_SELECT; //Seek Tag;
	SMM_WritePacked(ifd[SMM_PORT],&Packed);
	//6.2.0
	COM_avail(ifd[SMM_PORT],500);
	ret=SMM_ReadPacked(ifd[SMM_PORT],&Packed);
	if((ret==SMM_ERR_NONE)&&(Packed.LEN==2) && (Packed.DATA[0] == 'N'))
		ret = SMM_ERR_CARDNONE; //no card
	else{
        //1.9.2
        //7-byte seri numaralı kartalrı da okusun diye
		//if ((ret==SMM_ERR_NONE)&&(Packed.LEN==6)) //&&(Packed.DATA[0]==0x02))
		if ((ret==SMM_ERR_NONE)&&((Packed.LEN==6) || (Packed.LEN==9))) //&&(Packed.DATA[0]==0x02))
		{
			//1.9.2
			//7-byte kartlarıda okusun diye
			//memcpy(KartNo,Packed.DATA+1,4);
			//1.9.3
			//memcpy(KartNo,Packed.DATA+1,Packed.LEN-2);
			//kart numarasını sadece 4-byte olarak oku.
			memcpy(KartNo,Packed.DATA+1,4);
			//smm_SEEK[SMM_PORT]=0; //5.6.0 seek etmiyor artik
			ret=SMM_ERR_NONE;
		} else {
			//5.5.0
			//if (ret!=SMM_ERR_RECIEVE) //Data Bozuk Geldi Ise
			if (ret == SMM_ERR_CHKSUM)//(ret==SMM_ERR_RECIEVE) //Data Bozuk Geldi Ise
			{
				//printf("data bozuk gelmis\n");
				//2.2.0
				//2.5.1
				smm_RESET[SMM_PORT]=0;
			}
			//5.5.0
			//ret=SMM_ERR_CARDNONE;
		}
	}
	//2.2.0
	//2.5.1
	SMM_COUNTER++;
//	printf("smm_counter: %d\n", SMM_COUNTER);
	if (SMM_COUNTER==5000) //5000 idi
	{
		smm_RESET[COM1]=0;
		smm_RESET[COM2]=0;
		SMM_COUNTER=0;
	}

	return ret;
}

unsigned char SMM_ReadCardNumber(int ComPort,unsigned char *KartNo,unsigned char *KartNoHex)
{
	unsigned char ret;
	SMM_PORT = ComPort;

    //2.0.0
    if((Okuyucu_TIP[SMM_PORT] == 2)||(Okuyucu_TIP[SMM_PORT] == 3)){
        ret = CN56B_Select(ifd[SMM_PORT], KartNo, KartNoHex);
    }else{
        ret=SMM_ReadCard_Tmp(KartNo);

        if (ret==SMM_ERR_NONE)
        {
            Str2Hex(KartNo, 4, KartNoHex);
            //2.2.0
            if(memcmp(KartNoHex, "00000000",8) == 0){
                ret = SMM_ERR_RECIEVE;
            }
        }
    }
	return ret;
}

/*
void SMM_SetBaud(int Comport,enum Baud_t BaudRate)
{
	 SerPort_Close(Comport);
	 switch(Comport){
	 case COM1: SerPort_Open(Comport, 4096, 0x3f8, 4); break;
	 case COM2: SerPort_Open(Comport, 4096, 0x2f8, 3); break;
   }
	 SerPort_SetParam(Comport, BaudRate, PARITY_NO, LENGTH_8_BIT, STOPBIT_1_BIT);
}

unsigned char SMM_Ok(int Comport)
{
unsigned char r;
struct_SMM_PACKED Packed;

	Packed.LEN = 1;
	Packed.CMD = SMM_FIRMWARE;

	SMM_WritePacked(Comport,&Packed);
	//6.2.0
	COM_avail(SMM_PORT,15000);
	r = SMM_ReadPacked(Comport,&Packed);
	if(r == SMM_ERR_NONE){
		Packed.LEN=2;
		Packed.CMD=0x94;     //SETBAUD
		Packed.DATA[0]=0x04; //115200
		SMM_WritePacked(Comport,&Packed);
		delay(250);
		SMM_SetBaud(Comport,BAUD_115200);
		delay(250);
		r = SMM_ReadPacked(Comport,&Packed);
		if(r != SMM_ERR_NONE){
			SMM_SetBaud(Comport,BAUD_9600);
		}

	} else {

			//TEST ICIN 9600 e cek

			//Packed.LEN=2;
			//Packed.CMD=0x94;
			//Packed.DATA[0]=0x00;
			//SMM_WritePacked(Comport,&Packed);
			//return SMM_ERR_CARDNONE;

		SMM_SetBaud(Comport,BAUD_115200);
		delay(500);
		Packed.LEN=1;
		Packed.CMD=SMM_FIRMWARE;
		SMM_WritePacked(Comport,&Packed);
		//6.2.0
		COM_avail(SMM_PORT,15000);
		r = SMM_ReadPacked(Comport,&Packed);
		if(r != SMM_ERR_NONE){
			SMM_SetBaud(Comport,BAUD_9600);
		}
	}
	return r;
}
*/

unsigned char SMM_Select(int srl_ifd,unsigned char *KART)
{

	struct_SMM_PACKED Packed;
	Packed.LEN=1;
	Packed.CMD=SMM_SELECT;
	SMM_WritePacked(srl_ifd,&Packed);
	//6.2.0
	COM_avail(srl_ifd,500);
	if (SMM_ReadPacked(srl_ifd,&Packed)==SMM_ERR_NONE)
    {
        if (Packed.LEN>2)
        {
            Str2Hex(Packed.DATA+1,4,KART);
            return SMM_ERR_NONE;
        }
    }
    return SMM_ERR_CARDNONE;

}

unsigned char SMM_Login(unsigned char Sector,unsigned char TIP,unsigned char *KEY)
{
	unsigned char ret;
	struct_SMM_PACKED Packed;
	//1.6.0

    //2.0.0
    if((Okuyucu_TIP[SMM_PORT] == 2)||(Okuyucu_TIP[SMM_PORT] == 3)){
        //2.0.3
        //return CN56B_Login(Sector, TIP, KEY, (uint8_t *)rec_Personel.KARTNO, SMM_PORT);
        ret = CN56B_Login(Sector, TIP, KEY, (uint8_t *)rec_Personel.KARTNO, SMM_PORT);
        if (ret)
            ret = CN56B_Login(Sector, TIP, KEY, (uint8_t *)rec_Personel.KARTNO, SMM_PORT);
        return ret;
    }
    //2.2.0
    /*
	Packed.LEN=1;
	Packed.CMD=SMM_SELECT;
	SMM_WritePacked(ifd[SMM_PORT],&Packed);
	//6.2.0
	COM_avail(ifd[SMM_PORT],500);
	ret=SMM_ReadPacked(ifd[SMM_PORT],&Packed);
    */
	Packed.LEN=9;
	Packed.CMD=SMM_AUTH;
	Packed.DATA[0]=(Sector*4);
	Packed.DATA[1]=TIP;
	memcpy(Packed.DATA+2,KEY,6);
	SMM_WritePacked(ifd[SMM_PORT],&Packed);
	//6.2.0
	COM_avail(ifd[SMM_PORT],500);	//6.2.0
	ret=SMM_ReadPacked(ifd[SMM_PORT],&Packed);
	if (ret==SMM_ERR_NONE)
	{
		if (Packed.DATA[0]!=0x4C)
		{
			ret=SMM_ERR_KEY;
		}
	}
	return ret;
}

unsigned char SMM_WriteBlock(unsigned char Sector,unsigned char Block,unsigned char *DATA)
{
	unsigned char ret;
	struct_SMM_PACKED Packed;

	//2.0.0
    if((Okuyucu_TIP[SMM_PORT] == 2)||(Okuyucu_TIP[SMM_PORT] == 3)){
        return CN56B_WriteBlock(Sector, Block, (uint8_t *) DATA, SMM_PORT);
    }
    //2.2.0
	memset(&Packed, 0, sizeof(struct_SMM_PACKED));

	Packed.LEN=18;
	Packed.CMD=SMM_WRITEBLOCK;
	Packed.DATA[0]=(Sector*4)+Block;
	memcpy(Packed.DATA+1,DATA,16);
	SMM_WritePacked(ifd[SMM_PORT],&Packed);
	//6.2.0
	COM_avail(ifd[SMM_PORT],500);
	//2.2.0
	memset(&Packed, 0, sizeof(struct_SMM_PACKED));
	ret=SMM_ReadPacked(ifd[SMM_PORT],&Packed);
	if (ret==SMM_ERR_NONE)
	{
		if (Packed.LEN!=18)
		{
			ret=SMM_ERR_CARDERR;
		}else//2.2.0
		{
            if(memcmp(DATA, Packed.DATA+1, 16) != 0)
                ret = SMM_ERR_CARDERR;
		}
	}
	return ret;
}


unsigned char SMM_WriteValue(unsigned char Sector,unsigned char Block,uint32_t *VALUE)
{
	unsigned char ret;
	struct_SMM_PACKED Packed;
	Packed.LEN=6;
	Packed.CMD=SMM_WRITEVALUE;
	Packed.DATA[0]=(Sector*4)+Block;
	memcpy(Packed.DATA+1,VALUE,4);
	SMM_WritePacked(ifd[SMM_PORT],&Packed);
	//6.2.0
	COM_avail(ifd[SMM_PORT],500);
	ret=SMM_ReadPacked(ifd[SMM_PORT],&Packed);
	if (ret==SMM_ERR_NONE)
	{
		if (Packed.LEN!=6)
		{
			ret=SMM_ERR_CARDERR;
		}
	}
	return ret;
}


unsigned char SMM_ReadBlock(unsigned char Sector,unsigned char Block,unsigned char *DATA)
{
	unsigned char ret;
	struct_SMM_PACKED Packed;

	//2.0.0
    if((Okuyucu_TIP[SMM_PORT] == 2)||(Okuyucu_TIP[SMM_PORT] == 3)){
        return CN56B_ReadBlock(Sector, Block, (uint8_t *) DATA, SMM_PORT);
    }

	Packed.LEN=2;
	Packed.CMD=SMM_READBLOCK;
	Packed.DATA[0]=(Sector*4)+Block;
	SMM_WritePacked(ifd[SMM_PORT],&Packed);
	//6.2.0
	COM_avail(ifd[SMM_PORT],500);
	ret=SMM_ReadPacked(ifd[SMM_PORT],&Packed);
	if (ret==SMM_ERR_NONE)
	{
		if (Packed.LEN!=18)
		{
			ret=SMM_ERR_CARDERR;
		} else {
			memcpy(DATA,Packed.DATA+1,16);
		}
	}
	return ret;
}

unsigned char SMM_ReadValue(unsigned char Sector,unsigned char Block,uint32_t *VALUE)
{
	unsigned char ret;
	struct_SMM_PACKED Packed;

	//2.0.0
    if((Okuyucu_TIP[SMM_PORT] == 2)||(Okuyucu_TIP[SMM_PORT] == 3)){
        return CN56B_ReadValue(Sector, Block, VALUE, SMM_PORT);
    }

	Packed.LEN=2;
	Packed.CMD=SMM_READVALUE;
	Packed.DATA[0]=(Sector*4)+Block;
	SMM_WritePacked(ifd[SMM_PORT],&Packed);
	//6.2.0
	COM_avail(ifd[SMM_PORT],500);
	ret=SMM_ReadPacked(ifd[SMM_PORT],&Packed);
	if (ret==SMM_ERR_NONE)
	{
		if (Packed.LEN!=6)
		{
			ret=SMM_ERR_CARDERR;
		} else {
			memcpy(VALUE,Packed.DATA+1,4);
		}
	}
	return ret;
}

unsigned char SMM_Increment(unsigned char Sector,unsigned char Block,uint32_t *VALUE)
{
	unsigned char ret;
	struct_SMM_PACKED Packed;

	//2.0.0
    if((Okuyucu_TIP[SMM_PORT] == 2)||(Okuyucu_TIP[SMM_PORT] == 3)){
        return CN56B_Increment(Sector, Block, VALUE, SMM_PORT);
    }

	Packed.LEN=6;
	Packed.CMD=SMM_INCREMENT;
	Packed.DATA[0]=(Sector*4)+Block;
	memcpy(Packed.DATA+1,VALUE,4);
	SMM_WritePacked(ifd[SMM_PORT],&Packed);
//6.2.0
	COM_avail(ifd[SMM_PORT],500);
	ret=SMM_ReadPacked(ifd[SMM_PORT],&Packed);
	if (ret==SMM_ERR_NONE)
	{
		if (Packed.LEN!=6)
		{
			ret=SMM_ERR_CARDERR;
		} else {
			memcpy(VALUE,Packed.DATA+1,4);
		}
	}
	return ret;
}

unsigned char SMM_Decrement(unsigned char Sector,unsigned char Block,uint32_t *VALUE)
{
	unsigned char ret;
	struct_SMM_PACKED Packed;

	//2.0.0
    if((Okuyucu_TIP[SMM_PORT] == 2)||(Okuyucu_TIP[SMM_PORT] == 3)){
        return CN56B_Decrement(Sector, Block, VALUE, SMM_PORT);
    }

	Packed.LEN=6;
	Packed.CMD=SMM_DECREMENT;
	Packed.DATA[0]=(Sector*4)+Block;
	memcpy(Packed.DATA+1,VALUE,4);
	SMM_WritePacked(ifd[SMM_PORT],&Packed);
//6.2.0
	COM_avail(ifd[SMM_PORT],500);
	ret=SMM_ReadPacked(ifd[SMM_PORT],&Packed);
	if (ret==SMM_ERR_NONE)
	{
		if (Packed.LEN!=6)
		{
            //if (Packed.DATA[0]!=0x46) //Yazm Okuyamam Dsnda
            printf("DCR:%02X\r\n",Packed.DATA[0]);
  		    ret=SMM_ERR_CARDERR;
		} else {
			memcpy(VALUE,Packed.DATA+1,4);
		}
	}
	return ret;
}


unsigned char SMM_ReadMaster(int ComPort)
{
	unsigned char ret=SMM_ERR_CARDERR;
	unsigned char * Data;
	//2.2.0
	struct_SMM_PACKED Packed;

	Data = (unsigned char *) malloc(20);
	SMM_PORT=ComPort;

    //2.2.0
    Packed.LEN = 1;
	Packed.CMD = SMM_SELECT; //Seek Tag;
	SMM_WritePacked(ifd[SMM_PORT],&Packed);
	//6.2.0
	COM_avail(ifd[SMM_PORT],500);
	ret=SMM_ReadPacked(ifd[SMM_PORT],&Packed);

    if(ret != SMM_ERR_NONE){
        return ret;
    }
    ret = SMM_ERR_CARDERR;
    //1.6.0
    //master kart 2. blok'da SHUTDOWN yazıyorsa terminali kapa,
    //REBOOT yazıyorsa reboot et.
    if (SMM_Login(2,0xAA,rec_TERM.KEY_MASTER)==SMM_ERR_NONE)
	{
        memset(Data,0,20);
        //1.9.0
        //1.8.1
        //once 2|1'de personel key'i ya da UTARIT-ADMIN yazıyor mu diye bak,
        //sonra SHUTDOWN/REBOOT kontrol et
        //if(SMM_ReadBlock(2,1,Data)==SMM_ERR_NONE){
            //if((memcmp(Data, rec_TERM.KEY_PERSONEL, 6) == 0) || (memcmp(Data, "UTARIT-ADMIN", 12) == 0)){
                //memset(Data,0,20);
                if (SMM_ReadBlock(2,0,Data)==SMM_ERR_NONE)
                {
                    if(memcmp("SHUTDOWN", Data, 8) == 0){
                        free(Data);
                        return SMM_ERR_SHUTDOWN;
                    }
                    if(memcmp("REBOOT", Data, 6) == 0){
                        free(Data);
                        return SMM_ERR_REBOOT;
                    }
                    //2.2.5
                    if(memcmp("DATETIME", Data, 8) == 0){
                        free(Data);
                        return SMM_ERR_DATETIME;
                    }
                }
            //}
		//}
    }

    //2.2.0
    Packed.LEN = 1;
	Packed.CMD = SMM_SELECT; //Seek Tag;
	SMM_WritePacked(ifd[SMM_PORT],&Packed);
	//6.2.0
	COM_avail(ifd[SMM_PORT],500);
	ret=SMM_ReadPacked(ifd[SMM_PORT],&Packed);

    if(ret != SMM_ERR_NONE){
        return ret;
    }
    ret = SMM_ERR_CARDERR;

	if (SMM_Login(1,0xAA,rec_TERM.KEY_MASTER)==SMM_ERR_NONE)
	{
		memset(Data,0,20);
		if (SMM_ReadBlock(1,0,Data)==SMM_ERR_NONE)
		{
			memcpy(rec_TERM.KEY_BAKIYE,Data,6);
			memcpy(rec_TERM.KEY_TOPUP,Data+6,6);
			ret=SMM_ERR_NONE;
		}
		//brk
		//5.3.3 personel key'ini de master karttan okumak icin
		memset(Data,0,20); //5.7.0
		if (SMM_ReadBlock(1,1,Data)==SMM_ERR_NONE)
		{
			//6.9.1
			if(memcmp("\x00\x00\x00\x00\x00\x00", Data, 6) != 0)
                memcpy(rec_TERM.KEY_PERSONEL,Data,6);
			ret=SMM_ERR_NONE;
		}
		//5.7.0 sektor bilgilerini de master karttan okumak icin
		memset(Data,0,20);
		if (SMM_ReadBlock(1,2,Data) == SMM_ERR_NONE)
		{
			memcpy(&Para_Sektor, Data, 1);
			ret=SMM_ERR_NONE;

			if((Para_Sektor >3) && (Para_Sektor < 10))
			{
					Para_Blok = '\x0';
					YB_Sektor = Para_Sektor+1;
					YB_Blok = '\x0';
					//6.9.1
					memcpy(&Personel_Sektor, Data+1, 1);
					if((Personel_Sektor > 15) || (Personel_Sektor == Para_Sektor)
						|| (Personel_Sektor == YB_Sektor) || (Personel_Sektor == 0))
						Personel_Sektor = 2;
			}else
			{
					printf("sektor bilgisi hatali\n");
					printf("standard sektor kullanilacak!\n");
                    //0.54
                    gtk_clean();
                    //0.60
                    //gtk_layout_move(GTK_LAYOUT(main_layout), logo, 50, 5);
                    gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);
                    GdkRGBA bg_color;
                    bg_color.alpha = 1;
                    bg_color.blue = 0;
                    bg_color.green = 0;
                    bg_color.red = 1;
                    gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
                    //0.61
                    //gtk_widget_override_font(label1, pango_font_description_from_string("Droid Sans Bold 12"));//Ubuntu Medium Italic 12
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12
                    gtk_printf(label1, "SEKTÖR BİLGİSİ HATALI\nSTANDARD SEKTÖRLER KULLANILACAK");
                    while(gtk_events_pending())
                        gtk_main_iteration();
					Para_Sektor = '\x1';
					Para_Blok = '\x0';
					YB_Sektor = '\x3';
					YB_Blok = '\x0';
					//6.9.1
					Personel_Sektor = 2;
					napms(750);
			}
			//6.9.1
			/*
			memcpy(&Personel_Sektor, Data+1, 1);
			if((Personel_Sektor > 15) || (Personel_Sektor == Para_Sektor))
				Personel_Sektor = 2;
            */
		}else
		{
					Personel_Sektor = '\x2';
					Para_Sektor = '\x1';
					Para_Blok = '\x0';
					YB_Sektor = '\x3';
					YB_Blok = '\x0';
		}


	}else{
        ret = SMM_ERR_UNV;
    }
	free(Data);
	return ret;
}

unsigned char SMM_ReadPersonel(int ComPort)
{

	unsigned char ret=SMM_ERR_CARDERR;
	unsigned char * Data;
	Data = (unsigned char *) malloc(20);

	memset(&rec_Personel,0,sizeof(rec_Personel));
	SMM_PORT=ComPort;
	if (SMM_Login(Personel_Sektor,0xAA,rec_TERM.KEY_PERSONEL)==SMM_ERR_NONE)
	{
		memset(Data,0,20);
		if (SMM_ReadBlock(Personel_Sektor,0,Data)==SMM_ERR_NONE)
		{
			memcpy(rec_Personel.ADI,Data,16);
			memset(Data,0,20);
			if (SMM_ReadBlock(Personel_Sektor,1,Data)==SMM_ERR_NONE)
			{
				memcpy(rec_Personel.SOYAD,Data,16);
				memset(Data,0,20);
				if (SMM_ReadBlock(Personel_Sektor,2,Data)==SMM_ERR_NONE)
				{
					memcpy(rec_Personel.UNVAN,Data,16);
					memset(Data,0,20);

					//1.7.0
					//kapı modunda, keypad aktif ve kart şifresi ile çalışılacaksa, 14|2'den şifreyi oku
					memset(Kart_Sifre, 0, 10);
					//2.0.0
					//if((rec_TERM.TERM_TIP == TERM_KAPI) && (KEYPAD_AKTIF) && (KART_SIFRE_AKTIF))
				    if((rec_TERM.TERM_TIP == TERM_KAPI) && KEYPAD_AKTIF && KART_SIFRE_AKTIF && OKUYUCU_SIFRE_AKTIF[SMM_PORT])
                    {
                        if (SMM_Login(14,0xAA, (unsigned char *)"\xFF\xFF\xFF\xFF\xFF\xFE")==SMM_ERR_NONE)
                        {
                             if (SMM_ReadBlock(14, 2, Data)==SMM_ERR_NONE){
                                memcpy(&Kart_Sifre,Data,4);
                                free(Data);
                                return SMM_ERR_NONE;
                            }
                        }
                        free(Data);
                        return SMM_ERR_SIFRE;
                    }
                    //kapı ama keypad/şifre yoksa çık git.
                    if(rec_TERM.TERM_TIP == TERM_KAPI){
                        free(Data);
                        return SMM_ERR_NONE;
                    }

					if (SMM_Login(YB_Sektor,0xAA,rec_TERM.KEY_PERSONEL)==SMM_ERR_NONE)
					{
						if (SMM_ReadBlock(YB_Sektor,YB_Blok,Data)==SMM_ERR_NONE)
						{
							memcpy(rec_Personel.GRUP,Data,3);
							memcpy(&rec_Personel.YB,Data+3,9);
							if (rec_HAVUZ.AKTIF)//6.0.0
							{
								memset(Data,0,20);
								if (SMM_ReadBlock(YB_Sektor,HB_Blok,Data)==SMM_ERR_NONE)//Havuz Bilgisi Blogunu Oku
								{
									//6.4.0
									//memcpy(&rec_HB,Data,16);

									memcpy(&rec_HB.BASLANGIC,Data,4);	//Uyelik Baslangic
									memcpy(&rec_HB.BITIS,Data+4,4);		//Uyelik Bitis
									memcpy(&rec_HB.SNRSZ,Data+8,1);	//Sinirsiz Uyelik mi
									memcpy(&rec_HB.CINSIYET,Data+9,1);	//Cinsiyet (K/E/C)
									memcpy(&rec_HB.SEANS_ID,Data+10,2);//Girdigi Seansin ID'si
									memcpy(&rec_HB.FIYAT,Data+12,4);		//Girdigi Seansin Fiyati

								}else
								{
									//6.4.0
									//memset(&rec_HB,0,16);
									memset(&rec_HB,0,sizeof(rec_HB));
									rec_Personel.OGV=0;
									rec_Personel.BAKIYE=0;
									free(Data);
									return ret;
								}
								//6.4.0
								//HB_Blok+1'de Havuz giris tarihi ve icerde mi disarda mi bilgisi var.
								memset(Data,0,20);
								if (SMM_ReadBlock(YB_Sektor,HB_Blok+1,Data)==SMM_ERR_NONE)//Havuz Bilgisi Blogunu Oku
								{
									//memcpy(&rec_HB,Data,16);

									memcpy(&rec_HB.GIRIS_TARIHI,Data,4);	//Havuza Giris Yaptigi Tarih
									memcpy(&rec_HB.IN_OUT,Data+4,1);		//Iceride mi(1), Disarida mi(0)

								}else
								{
									//6.4.0
									//memset(&rec_HB,0,16);
									memset(&rec_HB,0,sizeof(rec_HB));
									rec_Personel.OGV=0;
									rec_Personel.BAKIYE=0;
									free(Data);
									return ret;
								}
							}
							if (rec_TERM.TERM_TIP!=TERM_KAPI)
							{
								if(SMM_Login(Para_Sektor,0xAA,rec_TERM.KEY_BAKIYE)==SMM_ERR_NONE)
								{
									if (SMM_ReadValue(Para_Sektor,Para_Blok,&rec_Personel.BAKIYE)==SMM_ERR_NONE) //havuz ise para sektoru farkli
									{
										if (SMM_ReadBlock(Para_Sektor,1,Data)==SMM_ERR_NONE)
										{
											memcpy(&rec_Personel.OGV,Data,4);
											ret=SMM_ERR_NONE;
											//2.2.0
											if(rec_Ayarlar.Refakatci_Kontrol || rec_Ayarlar.Giris_Kontrol){
                                                if (SMM_Login(14,0xAA, (unsigned char *)"\xFF\xFF\xFF\xFF\xFF\xFE")==SMM_ERR_NONE)
                                                {
                                                    if (SMM_ReadBlock(14,1,Data)==SMM_ERR_NONE){
                                                        memcpy(&rec_Personel.REFAKAT_SKT,Data,4);
                                                    }
                                                    if (SMM_ReadBlock(14,0,Data)==SMM_ERR_NONE){
                                                        memcpy(&rec_Personel.PDKS_TARIHI,Data,4);
                                                    }
                                                }
											}
										}
									}
								}
							} else {
								rec_Personel.OGV=0;
								rec_Personel.BAKIYE=0;
								ret=SMM_ERR_NONE;
							}
						}
					}
				}
			}
		}
	}else
	{
        ret = SMM_ERR_UNV;
    }
	free(Data);
	//lcd_printf(ALG_CENTER, "perso");
	//lcd_flip();
	return ret;
}


unsigned char SMM_WritePersonel(int ComPort)
{
	unsigned char ret = SMM_ERR_CARDERR;
	unsigned char * Data;
	unsigned char * Kart;
	uint32_t HARCAMA;

	//2.0.0
	unsigned char KartNo[4];
	unsigned char KartNoHex[10];

	//2.2.2
	char qq[21];
	char qw[76];

	memset(qq, 0, 21);
	memset(qw, 0, 76);

	Data = calloc(20,1);
	Kart = calloc(20,1);

	SMM_PORT = ComPort;

	//2.0.0
    if((Okuyucu_TIP[SMM_PORT] == 2)||(Okuyucu_TIP[SMM_PORT] == 3)){
        ret = CN56B_Select(ifd[SMM_PORT], KartNo, KartNoHex);
    }

    //1.6.0
	//if (SMM_Select(ifd[ComPort],Kart) == SMM_ERR_NONE) //Kart Varsa
	//{
		if (Hata.HATA)
		{
            //2.2.2
            sprintf(qq, "%02d/%02d/%04d, %02d:%02d:%02d",
                        rec_Personel.OKUMA_TARIHI.da_day,
                        rec_Personel.OKUMA_TARIHI.da_mon,
                        rec_Personel.OKUMA_TARIHI.da_year,
                        rec_Personel.OKUMA_SAATI.ti_hour,
                        rec_Personel.OKUMA_SAATI.ti_min,
                        rec_Personel.OKUMA_SAATI.ti_sec);
            sprintf(qw, "KART_NO: %s, HARCAMA: %lu, BAKIYE: %lu, OGUN_ID: %lu, YEMEK_SAYISI: %d",
                        Hata.KARTNO,
                        (unsigned long)Hata.HARCAMA,
                        (unsigned long)Hata.BAKIYE,
                        (unsigned long)Hata.YB.OGUN_ID,
                        Hata.YB.SAYI);
			if (!Hata.OK_B) //Para Dusemediyse
			{
				if (SMM_Login(Para_Sektor,0xAA,rec_TERM.KEY_BAKIYE)==SMM_ERR_NONE)
				{
					HARCAMA = Hata.HARCAMA;
					ret=SMM_Decrement(Para_Sektor,Para_Blok,&HARCAMA);
					if (ret==SMM_ERR_NONE)
					{
						Hata.OK_B=1;
						//2.2.2
						//PARA DUSME HATASI DUZELDI
						ini_puts(qq, "ERR1 OK", qw, HATA_LOG);
					}
				}
			} else {
				ret = SMM_ERR_NONE;
			}
            //2.2.2
            if(ret!=SMM_ERR_NONE){
                //2.2.2
                //PARA DUSME HATASI DUZELTILEMEDI
                ini_puts(qq, "ERR1 NOK", qw, HATA_LOG);
            }else
			if (ret==SMM_ERR_NONE)
			{
				memcpy(Data,rec_Personel.GRUP,3);
				memcpy(Data+3,&Hata.YB,9);
				//1.6.0
				//if ( (ret=SMM_Select(ifd[ComPort],Kart)) ==SMM_ERR_NONE) //Kart Varsa
				//{
					if ( (ret=SMM_Login(YB_Sektor,0xAA,rec_TERM.KEY_PERSONEL)) == SMM_ERR_NONE)
					{
						ret = SMM_WriteBlock(YB_Sektor,YB_Blok,Data);
					}
				//}
                    //2.2.2
                    if(ret!=SMM_ERR_NONE){
                        //YB HATASI DUZELTILEMEDI
                        ini_puts(qq, "ERR2 NOK", qw, HATA_LOG);
                    }else{
                        //YB HATASI DUZELTILDI
                        ini_puts(qq, "ERR2 OK", qw, HATA_LOG);
                    }

			}


		} else {
			memcpy(Data,rec_Personel.GRUP,3);
			memcpy(Data+3,&rec_Personel.YB,9);
			if (SMM_Login(Para_Sektor,0xAA,rec_TERM.KEY_BAKIYE)==SMM_ERR_NONE)
			{
				Hata.OK_B=0;
				HARCAMA = rec_Personel.HARCANAN;
				ret=SMM_Decrement(Para_Sektor,Para_Blok,&HARCAMA);
				//printf("PARA HATA :%d\r\n",ret);
				if (ret==SMM_ERR_NONE)
				{
					Hata.OK_B=1;
					//1.6.0
					//if ( (ret=SMM_Select(ifd[ComPort],Kart)) ==SMM_ERR_NONE) //Kart Varsa
					//{
						if ( (ret=SMM_Login(YB_Sektor,0xAA,rec_TERM.KEY_PERSONEL)) == SMM_ERR_NONE)
						{
							//printf("Login OK\r\n");
							ret= SMM_WriteBlock(YB_Sektor,YB_Blok,Data);
						}
					//}
                        //2.2.2
                        if(ret!=SMM_ERR_NONE){
                            sprintf(qq, "%02d/%02d/%04d, %02d:%02d:%02d",
                                        rec_Personel.OKUMA_TARIHI.da_day,
                                        rec_Personel.OKUMA_TARIHI.da_mon,
                                        rec_Personel.OKUMA_TARIHI.da_year,
                                        rec_Personel.OKUMA_SAATI.ti_hour,
                                        rec_Personel.OKUMA_SAATI.ti_min,
                                        rec_Personel.OKUMA_SAATI.ti_sec);
                            sprintf(qw, "KART_NO: %s, HARCAMA: %lu, BAKIYE: %lu, OGUN_ID: %lu, YEMEK_SAYISI: %d",
                                        rec_Personel.KARTNO_HEX,
                                        (unsigned long)rec_Personel.HARCANAN,
                                        (unsigned long)rec_Personel.BAKIYE,
                                        (unsigned long)rec_Personel.YB.OGUN_ID,
                                        rec_Personel.YB.SAYI);
                            //YB YAZILAMADI
                            ini_puts(qq, "ERR2", qw, HATA_LOG);
                        }
				}else{//2.2.2
                    sprintf(qq, "%02d/%02d/%04d, %02d:%02d:%02d",
                                rec_Personel.OKUMA_TARIHI.da_day,
                                rec_Personel.OKUMA_TARIHI.da_mon,
                                rec_Personel.OKUMA_TARIHI.da_year,
                                rec_Personel.OKUMA_SAATI.ti_hour,
                                rec_Personel.OKUMA_SAATI.ti_min,
                                rec_Personel.OKUMA_SAATI.ti_sec);
                    sprintf(qw, "KART_NO: %s, HARCAMA: %lu, BAKIYE: %lu, OGUN_ID: %lu, YEMEK_SAYISI: %d",
                                rec_Personel.KARTNO_HEX,
                                (unsigned long)rec_Personel.HARCANAN,
                                (unsigned long)rec_Personel.BAKIYE,
                                (unsigned long)rec_Personel.YB.OGUN_ID,
                                rec_Personel.YB.SAYI);
                    //DECREMENT EDEMEDI
                    ini_puts(qq, "ERR1", qw, HATA_LOG);

				}

			}//2.2.2
			else{
                sprintf(qq, "%02d/%02d/%04d, %02d:%02d:%02d",
                            rec_Personel.OKUMA_TARIHI.da_day,
                            rec_Personel.OKUMA_TARIHI.da_mon,
                            rec_Personel.OKUMA_TARIHI.da_year,
                            rec_Personel.OKUMA_SAATI.ti_hour,
                            rec_Personel.OKUMA_SAATI.ti_min,
                            rec_Personel.OKUMA_SAATI.ti_sec);
                sprintf(qw, "KART_NO: %s, HARCAMA: %lu, BAKIYE: %lu, OGUN_ID: %lu, YEMEK_SAYISI: %d",
                            rec_Personel.KARTNO_HEX,
                            (unsigned long)rec_Personel.HARCANAN,
                            (unsigned long)rec_Personel.BAKIYE,
                            (unsigned long)rec_Personel.YB.OGUN_ID,
                            rec_Personel.YB.SAYI);
                //PARA SEKTORUNE LOGIN OLAMADI
                ini_puts(qq, "ERR0", qw, HATA_LOG);
			}
		}
	//}
	free(Data);
	free(Kart);
	return ret;
}

unsigned char SMM_WriteHavuz(int ComPort)
{
	unsigned char ret = SMM_ERR_CARDERR;
	unsigned char * Data;
	unsigned char * Kart;
	uint32_t HARCAMA;
	int Seans_Fark = 0;
	//struct date dd; //6.4.0
	struct_date dd;
	//2.0.0
	unsigned char KartNo[4];
	unsigned char KartNoHex[10];

	Data = calloc(20,1);
	Kart = calloc(20,1);

	SMM_PORT = ComPort;


	//2.0.0
	//brk
	//havuz kısmı denendi mi acaba?
    if((Okuyucu_TIP[SMM_PORT] == 2)||(Okuyucu_TIP[SMM_PORT] == 3)){
        ret = CN56B_Select(ifd[SMM_PORT], KartNo, KartNoHex);
    }

	//2.2.0
	//if (SMM_Select(ifd[ComPort],Kart) == SMM_ERR_NONE) //Kart Varsa
	//{
		if (Havuz_Hata.HATA)
		{
			if (!Havuz_Hata.OK_B) //Para Dusemediyse
			{
				if (SMM_Login(Para_Sektor,0xAA,rec_TERM.KEY_BAKIYE)==SMM_ERR_NONE)
				{
					HARCAMA = Havuz_Hata.HARCAMA;
					ret=SMM_Decrement(Para_Sektor,Para_Blok,&HARCAMA);
					if (ret==SMM_ERR_NONE)
					{
						Havuz_Hata.OK_B=1;
					}
				}
			} else {
				ret = SMM_ERR_NONE;
			}

			if (ret==SMM_ERR_NONE)
			{
				//memcpy(Data,rec_Personel.GRUP,3);
				//memcpy(Data+3,&Havuz_Hata.YB,9);

				//memcpy(Data,&rec_HB.BASLANGIC,4);
				//memcpy(Data+4,&rec_HB.BITIS,4);
				//memcpy(Data+8,&rec_HB.SINIRSIZ,1);
				//memcpy(Data+9,&rec_HB.CINSIYET,1);
				//memcpy(Data+10,&rec_HB.SEANS_ID,2);
				//memcpy(Data+12,&Havuz_Hata.HARCAMA,4);
				memcpy(Data,&Havuz_Hata.HB,16);
				//2.2.0
				//if ( (ret=SMM_Select(ifd[ComPort],Kart)) ==SMM_ERR_NONE) //Kart Varsa
				//{
					if ( (ret=SMM_Login(YB_Sektor,0xAA,rec_TERM.KEY_PERSONEL)) == SMM_ERR_NONE)
					{ //6.4.0
						if((ret = SMM_WriteBlock(YB_Sektor,HB_Blok,Data)) == SMM_ERR_NONE){
							memset(Data, 0x0, 16);
							memcpy(Data, &Havuz_Hata.GIRIS_TARIHI, 4);
							memcpy(Data+4, &Havuz_Hata.IN_OUT, 1);
							ret = SMM_WriteBlock(YB_Sektor,HB_Blok+1,Data);
						}
					}
				//}
			}
		} else {
			memcpy(Data,&rec_HB.BASLANGIC,4);
			memcpy(Data+4,&rec_HB.BITIS,4);
			memcpy(Data+8,&rec_HB.SNRSZ,1);
			memcpy(Data+9,&rec_HB.CINSIYET,1);
			//memcpy(Data+10,&rec_HB.SEANS_ID,2);
			memcpy(Data+10,&rec_Seans.ID,2);//islemin yapildigi seans id
			//memcpy(Data+12,&rec_Personel.HARCANAN,4);
			if (SMM_Login(Para_Sektor,0xAA,rec_TERM.KEY_BAKIYE)==SMM_ERR_NONE)
			{
				Havuz_Hata.OK_B=0;
				if(rec_HAVUZ.GIRIS){//giris turnikesiyse
					HARCAMA = rec_Personel.HARCANAN;
					memcpy(Data+12,&rec_Personel.HARCANAN,4);
				}
				else{//cikis turnikesiyse
					if(rec_HB.SNRSZ != 0x1){//6.4.0 sinirsiz uyelik degilse seans farki al
						//odroid
						//getdate(&dd);
						get_date(&dd);
						if(memcmp(&rec_HB.GIRIS_TARIHI,&dd, 4) != 0)
							HARCAMA = rec_HAVUZ.CEZA;
						else{
							Seans_Fark = rec_Seans.ID - rec_HB.SEANS_ID;
							if(Seans_Fark > 0) //farkli bir seansta ciktiysa
								HARCAMA = Seans_Fark * rec_HB.FIYAT;//girdigi seans ucreti kadar ekle
							else
								HARCAMA = 0;
						}
					}else
						HARCAMA = 0;//6.4.0 sinirsiz uyelikse seans farki alma

					memcpy(Data+12,&rec_HB.FIYAT,4);//ilk girdigi (kartta yazan fiyat)
					rec_Personel.HARCANAN = HARCAMA;//karttan dusmeye calisilan para
				}
				//6.3.2
				//SINIRSIZ kartsa para dusme.
				if(rec_HB.SNRSZ == 0x1)
					ret = SMM_ERR_NONE;
				else
					ret=SMM_Decrement(Para_Sektor,Para_Blok,&HARCAMA);
				//printf("PARA HATA :%d\r\n",ret);
				if (ret==SMM_ERR_NONE)
				{
					Havuz_Hata.OK_B=1;
					//2.2.0
					//if ( (ret=SMM_Select(ifd[ComPort],Kart)) ==SMM_ERR_NONE) //Kart Varsa
					//{
						if ( (ret=SMM_Login(YB_Sektor,0xAA,rec_TERM.KEY_PERSONEL)) == SMM_ERR_NONE)
						{
							//printf("Login OK\r\n");
							//6.4.0
							if((ret= SMM_WriteBlock(YB_Sektor,HB_Blok,Data)) == SMM_ERR_NONE)
							{
								memset(Data, 0x0, 20);
								memcpy(Data, &rec_HB.GIRIS_TARIHI, 4);
								if(rec_HAVUZ.GIRIS)
									memset(Data+4, 1, 1); //giris yapiliyor, karta 1 yaz
								else
									memset(Data+4, 0, 1); //cikis yapiliyor, karta 0 yaz
								ret = SMM_WriteBlock(YB_Sektor,HB_Blok+1,Data);
							}
						}
					//}
				}
			}
		}
	//}
	free(Data);
	free(Kart);
	return ret;
}
