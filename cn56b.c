#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "cn56b.h"
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

uint8_t KartNo4Login[4];

unsigned char CN56B_WritePacked(int Port,struct_SMM_PACKED *Packed)
{
    uint8_t i;
    uint8_t crc;


    serialPutchar(Port, 0x2);
    serialPutchar(Port, 0x0);
    serialPutchar(Port, 0x0);
    crc=0;
    serialPutchar(Port, Packed->CMD);
    crc^=Packed->CMD;
    serialPutchar(Port, Packed->LEN);
    crc^=Packed->LEN;
    serialPutchar(Port, 0x0);

    for (i=0; i<Packed->LEN-1; i++)
    {
        serialPutchar(Port,Packed->DATA[i]);
        crc^=Packed->DATA[i];
    }

    Packed->CRC = crc;
    serialPutchar(Port, Packed->CRC);
    serialPutchar(Port, 0x3);
    return SMM_ERR_NONE;
}

unsigned char CN56B_ReadPacked(int Port,struct_SMM_PACKED *Packed)
{
    unsigned int max, crc;
    uint8_t i;

    int c;
    char* sz;
    uint8_t* szHex;

    memset(Packed,0x00,sizeof(struct_SMM_PACKED));

    max=COM_avail(Port,500);//odroid 20000 idi

    if (!max) return SMM_ERR_CARDNONE;

    sz=( char*) calloc(512, sizeof(char));

    szHex=(unsigned char*) calloc(512, sizeof(char));


    c = serialGetchar(Port);
    if(c == -1)
        printf("serport recv error\n");
    sz[0]= (char)c;
    c = serialGetchar(Port);
    sz[1]=(char)c;
    c = serialGetchar(Port);
    sz[2]=(char)c;

    if ((sz[0]!=0x2)||(sz[1]!=0x00)||(sz[2]!=0x00)) //Stx Etx yoksa bozuk
    {
        free(sz);
        free(szHex);
        return SMM_ERR_CHKSUM;
    }

    c = serialGetchar(Port);
    sz[3]=(char) c;//length

    c = serialGetchar(Port);
    sz[4]=(char) c;//status

    for (i=0; i<sz[3]+1; i++)
    {
        c = serialGetchar(Port);
        sz[i+5]=( char)c;
    }
    max=(sz[3])+4;

    crc = 0;
    for (i=1; i<max; i++)
        crc ^= sz[i];


    Packed->LEN = sz[3];
    Packed->CMD = sz[4];//cmd değil, status var burada
    memcpy(Packed->DATA,&sz[5],Packed->LEN-1);
    Packed->CRC = sz[max];

    free(sz);
    free(szHex);

    if (Packed->CRC !=crc)
    {
        return SMM_ERR_CHKSUM;
    }
    if (Packed->CMD ==0x0)  //burası status
    {
        return SMM_ERR_NONE;
    }
    else
    {
        return Packed->CMD;
    }
}

uint8_t CN56B_GetSerial(int srl_ifd, unsigned char *ser_no)
{
    struct_SMM_PACKED Packed;
    Packed.LEN = 0x1;
    Packed.CMD = 0x9;
    CN56B_WritePacked(srl_ifd,&Packed);
    //6.2.0
    COM_avail(srl_ifd, 500);
    if (CN56B_ReadPacked(srl_ifd,&Packed)==SMM_ERR_NONE)
    {
        if (Packed.LEN == 11)
        {
            memcpy(ser_no, Packed.DATA, 8);
            return SMM_ERR_NONE;
        }
    }
    return SMM_ERR_CARDNONE;

}

uint8_t CN56B_Select(int srl_ifd, unsigned char *KartNo, unsigned char *KartNoHex)
{

    struct_SMM_PACKED Packed;
    Packed.LEN = 0x4;
    Packed.CMD = 0x3d;//PN_InListPassiveTarget
    Packed.DATA[0] = 0x0;//read normal cards / 0x1: mifare plus
    Packed.DATA[1] = 0x1;//select max 1 card / 0x2: max 2 cards
    Packed.DATA[2] = 0x0;//106kbps / 0x1: 212kbps / 0x2: 424kbps
    CN56B_WritePacked(srl_ifd, &Packed);
    //6.2.0
    COM_avail(srl_ifd, 500);
    if (CN56B_ReadPacked(srl_ifd, &Packed) == SMM_ERR_NONE)
    {
        //brk
        //burada dönen datanın boyuna göre 4 yada 7-byte kart oldugunu anlayıp,
        //authentication vs işlerde kullanılacak kart numarası kısmı hazırlanır
        if (Packed.LEN == 11)
        {
            memcpy(KartNo4Login, Packed.DATA+6, 4);
        }
        else if (Packed.LEN == 14)
        {
            memcpy(KartNo4Login, Packed.DATA+9, 4);
        }
        memcpy(KartNo, Packed.DATA+6, 4);
        Str2Hex(KartNo, 4, KartNoHex);

        /*
        CN56B_Buzzer(srl_ifd);
        CN56B_Led(srl_ifd);
        */
        Packed.LEN = 0x02;
        Packed.CMD = 0x3e;//PN_SelectTarget
        Packed.DATA[0] = 0x1;//select only 1 target

        CN56B_WritePacked(srl_ifd, &Packed);
        //6.2.0
        COM_avail(srl_ifd, 500);
        return CN56B_ReadPacked(srl_ifd, &Packed);
    }
    return SMM_ERR_CARDNONE;

}

uint8_t CN56B_Login(uint8_t Sector, uint8_t TIP, uint8_t *KEY, uint8_t *KartNo4Key, int Port)
{
    uint8_t ret;
    struct_SMM_PACKED Packed;
    //2.0.0
    unsigned char KartNo[4];
    unsigned char KartNoHex[10];

    Packed.LEN = 0x7;
    Packed.CMD = 0x45; //LoadKey
    memcpy(Packed.DATA, KEY, 6);
    CN56B_WritePacked(ifd[Port], &Packed);
    //6.2.0
    COM_avail(ifd[Port],500);
    ret = CN56B_ReadPacked(ifd[Port], &Packed);
    if(ret != SMM_ERR_NONE)
    {
        return SMM_ERR_KEY;
    }

    ret = CN56B_Select(ifd[Port], KartNo, KartNoHex);
    if(ret != SMM_ERR_NONE)
    {
        return SMM_ERR_KEY;
    }

    Packed.LEN=7;
    Packed.CMD=0x40; //Authenticate
    if(TIP == 0xAA)
        Packed.DATA[0] = 0x60;//KeyA
    else
        Packed.DATA[0] = 0x61;//KeyB
    Packed.DATA[5]=(Sector*4);

    memcpy(Packed.DATA+1, KartNo4Login, 4);
    CN56B_WritePacked(ifd[Port],&Packed);
    //6.2.0
    COM_avail(ifd[Port],500);	//6.2.0
    ret=CN56B_ReadPacked(ifd[Port],&Packed);
    if (ret==SMM_ERR_NONE)
    {
        //if (Packed.DATA[0]!=0x4C)
        //{
        //	ret=SMM_ERR_KEY;
        //}
    }
    return ret;
}


uint8_t CN56B_ReadBlock(uint8_t Sector, uint8_t Block, uint8_t *DATA, int Port)
{
    uint8_t ret;
    struct_SMM_PACKED Packed;



    Packed.LEN = 0x3;
    Packed.CMD = 0x41;//ReadBlock
    Packed.DATA[0] = (Sector*4)+Block;//Starting Block Address
    Packed.DATA[1] = 0x1; //Number of Blocks to Read

    CN56B_WritePacked(ifd[Port],&Packed);
    //6.2.0
    COM_avail(ifd[Port],500);
    ret=CN56B_ReadPacked(ifd[Port], &Packed);
    if (ret==SMM_ERR_NONE)
    {
        //if (Packed.LEN!=18)
        //{
        //	ret = SMM_ERR_CARDERR;
        //} else {
        memcpy(DATA, Packed.DATA, 16);//Data Read
        //}
    }
    return ret;
}

uint8_t CN56B_WriteBlock(uint8_t Sector,uint8_t Block, uint8_t *DATA, int Port)
{
    uint8_t ret;
    struct_SMM_PACKED Packed;


    Packed.LEN=0x13;
    Packed.CMD=0x42;//WriteBlock
    Packed.DATA[0]=(Sector*4)+Block;//Starting Block
    Packed.DATA[1]=0x1;//Number of Blocks to Write
    memcpy(Packed.DATA+2,DATA,16);
    CN56B_WritePacked(ifd[Port],&Packed);
    //6.2.0
    COM_avail(ifd[Port],500);
    ret=CN56B_ReadPacked(ifd[Port],&Packed);
    if (ret==SMM_ERR_NONE)
    {
        //if (Packed.LEN!=18)
        //{
        //	ret=SMM_ERR_CARDERR;
        //}
    }
    return ret;
}

uint8_t CN56B_Transfer(uint8_t Sector, uint8_t Block, int Port)
{
    uint8_t ret;
    struct_SMM_PACKED Packed;



    Packed.LEN = 0x2;
    Packed.CMD = 0x43;//Transfer
    Packed.DATA[0] = (Sector*4)+Block;//SAddress of the Value Block

    CN56B_WritePacked(ifd[Port], &Packed);
    //6.2.0
    COM_avail(ifd[Port],500);
    ret=CN56B_ReadPacked(ifd[Port], &Packed);
    if (ret==SMM_ERR_NONE)
    {
        //if (Packed.LEN!=18)
        //{
        //	ret = SMM_ERR_CARDERR;
        //} else {
        //memcpy(DATA, Packed.DATA, 16);//Data Read
        //}
    }
    return ret;
}


uint8_t CN56B_Decrement(uint8_t Sector, uint8_t Block, uint32_t *VALUE, int Port)
{
    uint8_t ret;
    struct_SMM_PACKED Packed;


    Packed.LEN = 0x7;
    Packed.CMD = 0x44;
    Packed.DATA[0] = 0xC0;//Decrement
    Packed.DATA[1] = (Sector*4)+Block;//Address of the Value Block
    memcpy(Packed.DATA+2, VALUE, 4);
    CN56B_WritePacked(ifd[Port],&Packed);
//6.2.0
    COM_avail(ifd[Port],500);
    ret=CN56B_ReadPacked(ifd[Port], &Packed);
    if (ret==SMM_ERR_NONE)
    {
        /*if (Packed.LEN!=6)
        {
            //if (Packed.DATA[0]!=0x46) //Yazm Okuyamam Dsnda
            printf("DCR:%02X\r\n",Packed.DATA[0]);
            ret=SMM_ERR_CARDERR;
        } else {
        	memcpy(VALUE,Packed.DATA+1,4);
        }*/
        ret = CN56B_Transfer(Sector, Block, Port);
    }
    return ret;
}

uint8_t CN56B_Increment(uint8_t Sector, uint8_t Block, uint32_t *VALUE, int Port)
{
    uint8_t ret;
    struct_SMM_PACKED Packed;


    Packed.LEN = 0x7;
    Packed.CMD = 0x44;
    Packed.DATA[0] = 0xC1;//Increment
    Packed.DATA[1] = (Sector*4)+Block;//Address of the Value Block
    memcpy(Packed.DATA+2, VALUE, 4);
    CN56B_WritePacked(ifd[Port],&Packed);
//6.2.0
    COM_avail(ifd[Port],500);
    ret=CN56B_ReadPacked(ifd[Port], &Packed);
    if (ret==SMM_ERR_NONE)
    {
        /*if (Packed.LEN!=6)
        {
            //if (Packed.DATA[0]!=0x46) //Yazm Okuyamam Dsnda
            printf("DCR:%02X\r\n",Packed.DATA[0]);
            ret=SMM_ERR_CARDERR;
        } else {
        	memcpy(VALUE,Packed.DATA+1,4);
        }*/
        ret = CN56B_Transfer(Sector, Block, Port);
    }
    return ret;
}

uint8_t CN56B_ReadValue(uint8_t Sector, uint8_t Block, uint32_t *VALUE, int Port)
{
    uint8_t ret;
    struct_SMM_PACKED Packed;


    Packed.LEN = 0x3;
    //Packed.CMD = 0x44;//Read Value
    //Packed.DATA[0] = 0xC3;//Read Value
    //Packed.DATA[1] = (Sector*4)+Block;//Address of the Value Block
    Packed.CMD = 0x41;//ReadBlock
    Packed.DATA[0] = (Sector*4)+Block;//Starting Block Address
    Packed.DATA[1] = 0x1; //Number of Blocks to Read
    CN56B_WritePacked(ifd[Port], &Packed);
    //6.2.0
    COM_avail(ifd[Port],500);
    ret=CN56B_ReadPacked(ifd[Port], &Packed);
    if (ret==SMM_ERR_NONE)
    {
        //if (Packed.LEN!=6)
        //{
        //	ret=SMM_ERR_CARDERR;
        //} else {
        memcpy(VALUE, Packed.DATA, 4);
        //}
    }
    return ret;
}


int CN56B_Keyb(int srl_ifd, char *sfr)
{
    uint8_t ret;
    struct_SMM_PACKED Packed;


    Packed.LEN = 0x1;
    Packed.CMD = 0x17;//GetKey
    CN56B_WritePacked(srl_ifd, &Packed);
    COM_avail(srl_ifd,500);
    ret=CN56B_ReadPacked(srl_ifd, &Packed);
    if (ret==SMM_ERR_NONE)
    {
        if (Packed.LEN>1)
        {
            memcpy(sfr, Packed.DATA, Packed.LEN -1);
            return Packed.LEN -1;
        }
        else
            return -1;
    }
    return -1;
}

void CN56B_Buzzer(int srl_ifd)
{
    uint8_t ret;
    struct_SMM_PACKED Packed;

    Packed.LEN = 0x7;

    Packed.CMD = 0x26;//ActiveBuzzer
    Packed.DATA[0] = 0x04;//pattern buzzer
    Packed.DATA[1] = 0x01;//on
    Packed.DATA[2] = 0x01;//off
    Packed.DATA[3] = 0x00;//on
    Packed.DATA[4] = 0x00;//off
    Packed.DATA[5] = 0x01;//cycles

    CN56B_WritePacked(srl_ifd, &Packed);
    COM_avail(srl_ifd,500);
    ret=CN56B_ReadPacked(srl_ifd, &Packed);
    if (ret==SMM_ERR_NONE)
    {

    }
}

void CN56B_Led(int srl_ifd)
{
    uint8_t ret;
    struct_SMM_PACKED Packed;

    Packed.LEN = 0x7;

    Packed.CMD = 0x25;//ActiveLED
    Packed.DATA[0] = 0x02;//Green //0x1: Red
    Packed.DATA[1] = 0x01;//on
    Packed.DATA[2] = 0x01;//cycles

    CN56B_WritePacked(srl_ifd, &Packed);
    COM_avail(srl_ifd,500);
    ret=CN56B_ReadPacked(srl_ifd, &Packed);
    if (ret==SMM_ERR_NONE)
    {

    }
}

