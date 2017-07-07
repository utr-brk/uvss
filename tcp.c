/*
 * tcp.c
 *
 *  Created on: 24 Aðu 2013
 *      Author: tuzun
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
//#include <dos.h>

//#include "acctimer.h"
//#include "main.h"
//#include "dsock.h"
//#include "tool.h"
#include "log.h"
//#include "resource.h"
#include "config.h"
#include "personel.h"
#include "tcp.h"
#include "main.h"
#include "tool.h"
#include "lcd.h"
#include "reader.h"

#define  packet_Start		'['
#define  packet_End			']'

#define	 tcp_cmd_Izin			0x0001
#define	 tcp_cmd_Hareket		0x0002
#define	 tcp_cmd_Para			0x0003

#define	 tcp_cmd_Log_Kart		0xAA01
#define	 tcp_cmd_Log_Para		0xAA03
#define	 tcp_cmd_Log_Term		0xAA02

#define	 tcp_cmd_TarihSaat		0xBB01

#define	 tcp_cmd_CloseTcp 		0xEE00

#define	 tcp_err_None			0xFF00
#define	 tcp_err_Protocol		0xFF01
#define	 tcp_err_Checksum		0xFF02
#define	 tcp_err_Len			0xFF03
#define	 tcp_err_Data			0xFF04
#define	 tcp_err_Database		0xFFD1
#define	 tcp_err_Undefined		0xFFFF

#define	 tcp_err_Connection		0x9999

void TCP_Init(void)
{
    static int inited = 0;
    if (!inited)
    {
        signal(SIGPIPE, SIG_IGN);
        inited = 1;
    }
}
/*
int TCP_Connect(void)
{
	int s;
	//int flags;
	struct sockaddr_in serv_addr;
	//struct timeval Timeout;
	//Timeout.tv_sec = 2;
	//Timeout.tv_usec = 0;
//brk
//burada hata durumunda 1 donuyordu, -1 yapýldý.
//tp_connect'in çaðrýldýðý yerlerde buna dikkat edilmeli...

	TCP_Init();

	if((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		debug_print("TCP Error: Could not create socket");
		return -1;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(rec_TERM.PORT_SERVER); //default'u 4444

	//if(inet_pton(AF_INET, "10.10.10.1", &serv_addr.sin_addr)<=0)
	if(inet_pton(AF_INET, rec_TERM.IP_SERVER, &serv_addr.sin_addr)<=0)
	{
		debug_print("inet_pton error occured");
	    return -1;
	}

	//Set socket to non-blocking
	//if ((flags = fcntl(s, F_GETFL, 0)) < 0)
	//{
	//	debug_print("TCP Error: Could not get socket flags");
	//	return -1;
	//}

	//if (fcntl(s, F_SETFL, flags | O_NONBLOCK) < 0)
	//{
	//	debug_print("TCP Error: Could not set socket to non-blocking mode");
	//	return -1;
	//}

	//lcd_printf(ALG_CENTER, "TCP CONNECT");
	//lcd_flip();
	if( connect(s, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
	   debug_print("TCP-Error: Connect Failed ");
	   return -1;
	}
	// Set socket to blocking

	//if ((flags = fcntl(s, F_GETFL, 0)) < 0)
	//{
	//	debug_print("TCP Error: Could not get socket flags");
	//	return -1;
	//}


	//if (fcntl(s, F_SETFL, flags & (~O_NONBLOCK)) < 0)
	//{
	//	debug_print("TCP Error: Could not set socket to blocking mode");
	//	return -1;
	//}
	//fd_set Write, Err;
	//FD_ZERO(&Write);
	//FD_ZERO(&Err);
	//FD_SET(s, &Write);
	//FD_SET(s, &Err);

	// check if the socket is ready
	//select(0,NULL,&Write,&Err,&Timeout);
	//if(FD_ISSET(s, &Write))
	//{
	//	return s;
	//}
	return s;
	//return -1;
}
*/

int TCP_Connect(const char *addr_to_connect, int port_to_connect)
{
    int res;
    struct sockaddr_in addr;
    long arg;
    fd_set myset;
    struct timeval tv;
    int valopt;
    socklen_t lon;
    int soc;

    TCP_Init();
    // Create socket
    soc = socket(AF_INET, SOCK_STREAM, 0);
    if (soc < 0)
    {
        //debug_print("Error creating socket (%d %s)", errno, strerror(errno));
        return -1;
    }

    addr.sin_family = AF_INET;
    //addr.sin_port = htons(rec_TERM.PORT_SERVER);
    addr.sin_port = htons(port_to_connect);
    //addr.sin_addr.s_addr = inet_addr(rec_TERM.IP_SERVER);
    addr.sin_addr.s_addr = inet_addr(addr_to_connect);

    // Set non-blocking
    if( (arg = fcntl(soc, F_GETFL, NULL)) < 0)
    {
        //debug_print("Error fcntl(..., F_GETFL) (%s)", strerror(errno));
        //2.5.3
        close(soc);
        return -1;
    }
    arg |= O_NONBLOCK;
    if( fcntl(soc, F_SETFL, arg) < 0)
    {
        //debug_print("Error fcntl(..., F_SETFL) (%s)", strerror(errno));
        //2.5.2
        close(soc);
        return -1;
    }
    // Trying to connect with timeout
    res = connect(soc, (struct sockaddr *)&addr, sizeof(addr));
    if (res < 0)
    {
        if (errno == EINPROGRESS)
        {
            //fprintf(stderr, "EINPROGRESS in connect() - selecting");
            do
            {
                //1.0.1
                //5 iken 1 sn yapılmıştı ama timeout'a düşüyordu, 2 sn yapıldı.
                //1.0.3
                //tekrar 5 sn yapıldı
                //1.0.3
                //tekrar 2 sn yapıldı
                tv.tv_sec = 2;//5 idi
                tv.tv_usec = 0;
                FD_ZERO(&myset);
                FD_SET(soc, &myset);
                res = select(soc+1, NULL, &myset, NULL, &tv);
                if (res < 0 && errno != EINTR)
                {
                    //debug_print("Error connecting %d - %s", errno, strerror(errno));
                    //2.5.2
                    close(soc);
                    return -1;
                }
                else if (res > 0)
                {
                    // Socket selected for write
                    lon = sizeof(int);
                    if (getsockopt(soc, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0)
                    {
                        //debug_print("Error in getsockopt() %d - %s", errno, strerror(errno));
                        //2.5.2
                        close(soc);
                        return -1;
                    }
                    // Check the value returned...
                    if (valopt)
                    {
                        //debug_print("Error in delayed connection() %d - %s", valopt, strerror(valopt));
                        //2.5.2
                        close(soc);
                        return -1;
                    }
                    break;
                }
                else
                {
                    //debug_print("Timeout in select() - Cancelling!");
                    //2.5.2
                    close(soc);
                    return -1;
                }
            }
            while (1);
        }
        else
        {
            //debug_print("Error connecting %d - %s", errno, strerror(errno));
            //2.5.2
            close(soc);
            return -1;
        }
    }
    // Set to blocking mode again...
    if( (arg = fcntl(soc, F_GETFL, NULL)) < 0)
    {
        //debug_print("Error fcntl(..., F_GETFL) (%s)", strerror(errno));
        //2.5.2
        close(soc);
        return -1;
    }
    arg &= (~O_NONBLOCK);
    if( fcntl(soc, F_SETFL, arg) < 0)
    {
        //debug_print("Error fcntl(..., F_SETFL) (%s)", strerror(errno));
        //2.5.2
        close(soc);
        return -1;
    }
    return soc;
}

void   TCP_Close(int isocket)
{
    if (isocket >=0)
        close(isocket);
}

//m8110 version
void TCP_SendData(struct_TCP_Packet * sndPacket)
{
    uint16_t   i,pck_Len;
    unsigned char *strByte;//m8110 unsigned idi
    unsigned char *strHex;//m8110 unsigned idi
    char bufSend[1024];

    pck_Len = sizeof(struct_TCP_Packet);
    strByte = (unsigned char *) calloc (pck_Len, 1);//m8110 unsigned char idi
    strHex = (unsigned char *) calloc ((pck_Len*2)+1, 1);//m8110 unsigned char idi

    sndPacket->CheckSum=0;
    for (i=0; i<sndPacket->Len; i++)
        sndPacket->CheckSum ^= sndPacket->Data[i];
    memset(bufSend, 0x0, 1024);
    memcpy(strByte,sndPacket,pck_Len);
    aStringToHex(strByte,sndPacket->Len+5,strHex);

    /*printf("send_pack.command: %x\n", sndPacket->Command);
    printf("send_pack.length: %x\n", sndPacket->Len);
    printf("send_pack.checksum: %x\n", sndPacket->CheckSum);*/
    sprintf(bufSend, "%c%s%c", packet_Start, strHex, packet_End);

    write(tcp_Socket, bufSend, strlen((char *)bufSend));

    free(strByte);
    free(strHex);
}

//m8110 version
char TCP_RecieveData(struct_TCP_Packet * rcvPacket,long toMiliSec)
{
    char ret = 0x00;
    uint16_t   i,L;
    uint16_t pck_Len = sizeof(struct_TCP_Packet);
    unsigned char  ChkSum;
    unsigned char *strByte;
    unsigned char *strHex;
    unsigned char *strRcv;
    //unsigned char rcvBuf[512];
    //int n = 0;

    fd_set readset;
    int result, iof = -1;
    struct timeval tv;
    strByte = (unsigned char *) calloc (pck_Len, 1);
    strHex = (unsigned char *) calloc ((pck_Len*2)+1, 1);
    strRcv = (unsigned char *) calloc (2049, 1);

    // Initialize the set
    FD_ZERO(&readset);
    FD_SET(tcp_Socket, &readset);

    // Initialize time out struct
    tv.tv_sec = 0;
    tv.tv_usec = toMiliSec * 1000;
    // select()
    result = select(tcp_Socket+1, &readset, NULL, NULL, &tv);

    // Check status
    if (result < 0)
    {
        //return -1;
        ret = 0;
        //debug_print("tcp_error");
    }
    else if (result > 0 && FD_ISSET(tcp_Socket, &readset))
    {
        // Set non-blocking mode
        if ((iof = fcntl(tcp_Socket, F_GETFL, 0)) != -1)
            fcntl(tcp_Socket, F_SETFL, iof | O_NONBLOCK);
        // receive
        result = recv(tcp_Socket, strRcv, 2048, 0);
        // set as before
        if (iof != -1)
            fcntl(tcp_Socket, F_SETFL, iof);
        //return result;
        //debug_print("tcp read: %d bytes", result);
        ret = 0x1;
    }//else
    // debug_print("tcp_timeout");
    //ret = 0;
    //printf("pck_LEN: %d\n", sizeof(rcvPacket));
//	read(tcp_Socket, rcvPacket, sizeof(rcvPacket)-1);
    /*i=0;
    while ( (n = read(tcp_Socket, strRcv+i, sizeof(strRcv)-1)) > 0)
    {
    	i+=n;
    }

    if(n < 0)
    {
    	printf("\n Read error \n");
    	ret = 0x0;
    }else
    	ret = 0x1;
    */
    /*timeOut=AccTimer_SetTime(toMiliSec);
    while ((!ret)&&(!AccTimer_IsTimeOut(timeOut)))
    {
    	if (SocketDataReady(tcp_Socket)>0)
    	{
    		ret=0x01;
    		memset(strRcv,0,2049);
    		SocketGetString(tcp_Socket, strRcv, 2048);
    		//"TCP << %s\r\n"

    	}
    }*/
    //ret=0x01;
    if (ret==0x01)
    {
        ret=0x00;
        //L = strlen((char *)strRcv);
        L = result;
        //debug_print("L: %d, start: %d, end: %d", L, strRcv[0], strRcv[L-1]);
        //debug_print("Packet Start: %d, Packet_End: %d", packet_Start, packet_End);
        //printf("Packet: ");
        //for(i=0;i<L;i++){
        //	printf("%c-", (char) strRcv[i]);
        //}
        //printf("\nPacket-Bitti");

        if (((L%2)==0)&&(strRcv[0]==packet_Start)&&(strRcv[L-1]==packet_End))
        {
            //printf("***************data received\n");

            memset(strHex,0,(pck_Len*2)+1);
            memcpy(strHex,strRcv+1,L-2);
            L = aHexToString((char *)strHex, (char *)strByte);
            memset(rcvPacket,0,pck_Len);
            memcpy(rcvPacket,strByte,L);
            if ((rcvPacket->Len==(L-5))&&(rcvPacket->Command==tcp_err_None))
            {
                //printf("*******hersey iyi****************\n");
                ChkSum=0;
                for (i=0; i<rcvPacket->Len; i++)
                    ChkSum ^= rcvPacket->Data[i];
                if (ChkSum==rcvPacket->CheckSum)
                {
                    ret=0x01;
                    //debug_print("tcp rcv OKOKOK");
                }
                //else
                //debug_print("tcp rcv NONONO");
            }
            //else{
            //debug_print("****hersey not iyi****");
            //printf("L: %d\n", L);
            //printf("rcvPacket->Len: %d\n", rcvPacket->Len);
            //printf("rcvPacket->Command: %d\n", rcvPacket->Command);
            //}

        }//else
        //debug_print("*****packet error****");
    }
    free(strRcv);
    free(strByte);
    free(strHex);
    return ret;
}

//m8110 version
char TCP_SendRecieve(struct_TCP_Packet * sndPacket,struct_TCP_Packet * rcvPacket)
{
    char ret = 0x00;
    struct_TCP_Packet closePacket;

//brk
//tcp_socket hata varsa 1 dönüyor, düzeltilmeli.
//tcp_connect hata dönerse ne yapacaðýz?
//bu sistem sadece online çalýþacaksa baðlantý saðlanamýyor diyerek çýkmalý...

    //debug_print("net_ONLINE: %d", net_ONLINE);
    if (net_ONLINE==0)
    {
        net_ONLINE=rec_TERM.ONLINE_TEKRAR_DK;
        //odroid
        if(READER_TYPE == READER_MIFARE)//Mifare
            tcp_Socket = TCP_Connect(rec_TERM.IP_SERVER, rec_TERM.PORT_SERVER);
        else//Barcode
            tcp_Socket = TCP_Connect(rec_TERM.IP_SERVER, rec_TERM.PORT_SERVER +20);//1.0.6 +10 iken ziyaretci fişi basan poslarla çakışıyordu

        //tcp_Socket = TCP_Connect("192.168.2.38", rec_TERM.PORT_SERVER);
        if (tcp_Socket>0)
        {
            TCP_SendData(sndPacket);
            //usleep(100000);
            if (TCP_RecieveData(rcvPacket,2000)==0x1)//0.0.6 1000 ms idi
            {
                //debug_print("tcp receive data ok, net_online:0");
                ret=0x01;
                net_ONLINE=0;
                flag_LOG_PARA=1;//0.0.7
            }//else
            //debug_print("tcp receive data error");

            memset(&closePacket,0x00,sizeof(closePacket));
            closePacket.Command = tcp_cmd_CloseTcp;
            TCP_SendData(&closePacket);
            net_ONLINE_PARA = ret;
            TCP_Close(tcp_Socket);
            tcp_Socket = -1;
        }//else{
        //debug_print("TCP CANT CONNECT"); //o zaman isleme devam edilmeyip, bakiye duzeltilmeli...
        //}
    }
    return ret;
}

//m8110 version
char TCP_GetDateTime(struct_date *d,struct_time *t)
{
    char ret = 0x00;
    //int i;
    struct_TCP_Packet sndPacket;
    struct_TCP_Packet rcvPacket;

    memset(&sndPacket,0x00,sizeof(sndPacket));
    memset(&rcvPacket,0x00,sizeof(rcvPacket));

    sndPacket.Command = tcp_cmd_TarihSaat;
    ret=TCP_SendRecieve(&sndPacket,&rcvPacket);
    if (ret==0x01)
    {
        memcpy(d,rcvPacket.Data,4);
        memcpy(t,rcvPacket.Data+4,4);

    }
    //printf("Date:%0d/%0d/%0d\n", d->da_day, d->da_mon, d->da_year);
    //printf("Time:%02d:%02d:%02d.%02d\n", t->ti_hour, t->ti_min, t->ti_sec, t->ti_hund);
    return ret;
}

char TCP_PersonelIzinAl(struct_PERSONEL * recPersonel,unsigned char *Response)
{
    char ret = 0x00;
    struct_TCP_Packet sndPacket;
    struct_TCP_Packet rcvPacket;

    memset(&sndPacket,0x00,sizeof(sndPacket));
    memset(&rcvPacket,0x00,sizeof(rcvPacket));

    sndPacket.Command = tcp_cmd_Izin;
    sndPacket.Len = sizeof(struct_PERSONEL);
    memcpy(sndPacket.Data,recPersonel,sndPacket.Len);
    ret=TCP_SendRecieve(&sndPacket,&rcvPacket);
    if (ret==0x01)
    {
        memcpy(Response,rcvPacket.Data,rcvPacket.Len);
    }
    return ret;
}

char TCP_PersonelHareketIsle(struct_PERSONEL * recPersonel,unsigned char *Response)
{
    char ret=0x00;
    struct_TCP_Packet 	sndPacket;
    struct_TCP_Packet 	rcvPacket;

    memset(&sndPacket,0x00,sizeof(sndPacket));
    memset(&rcvPacket,0x00,sizeof(rcvPacket));

    sndPacket.Command = tcp_cmd_Hareket;
    sndPacket.Len = sizeof(struct_PERSONEL);
    memcpy(sndPacket.Data,recPersonel,sndPacket.Len);
    ret=TCP_SendRecieve(&sndPacket,&rcvPacket);
    if (ret==0x01)
    {
        memcpy(Response,rcvPacket.Data,rcvPacket.Len);
    }
    return ret;
}

//m8110 version
char TCP_LogPara(struct_LOG_PARA *recLOG)
{
    char ret=0x00;
    struct_TCP_Packet 	sndPacket;
    struct_TCP_Packet 	rcvPacket;

    memset(&sndPacket,0x00,sizeof(sndPacket));
    memset(&rcvPacket,0x00,sizeof(rcvPacket));
    sndPacket.Command = tcp_cmd_Log_Para;
    sndPacket.Len = sizeof(struct_LOG_PARA);
    memcpy(sndPacket.Data,recLOG,sndPacket.Len);
    ret=TCP_SendRecieve(&sndPacket,&rcvPacket);
    if (ret==0x01)
    {
        if (rcvPacket.Command!=tcp_err_None)
        {
            ret=0x00;
        }
    }
    return ret;
}

char TCP_LogKart(struct_LOG_KART *recLOG)
{
    char ret=0x00;
    struct_TCP_Packet 	sndPacket;
    struct_TCP_Packet 	rcvPacket;

    memset(&sndPacket,0x00,sizeof(sndPacket));
    memset(&rcvPacket,0x00,sizeof(rcvPacket));
    sndPacket.Command = tcp_cmd_Log_Kart;
    sndPacket.Len = sizeof(struct_LOG_KART);
    memcpy(sndPacket.Data,recLOG,sndPacket.Len);
    ret=TCP_SendRecieve(&sndPacket,&rcvPacket);
    if (ret==0x01)
    {
        if (rcvPacket.Command!=tcp_err_None)
        {
            //debug_print("tcp rcvPacket error");
            ret=0x00;
        }
    }
    return ret;
}

//m8110 version
char TCP_PersonelParaIsle(struct_TCP_Para *recPara, unsigned char *Response)
{
    char ret=0x00;
    struct_TCP_Packet 	sndPacket;
    struct_TCP_Packet 	rcvPacket;
    //int i;

    memset(&sndPacket,0x00,sizeof(sndPacket));
    memset(&rcvPacket,0x00,sizeof(rcvPacket));

    sndPacket.Command = tcp_cmd_Para;
    sndPacket.Len = sizeof(struct_TCP_Para);
    //sndPacket.Len = 21;
    //printf("*********PARA DETAILS***********\n");
    //printf("KARTNO: %02X-%02X-%02X-%02X\n", recPara->KARTNO[0],recPara->KARTNO[1],recPara->KARTNO[2],recPara->KARTNO[3]);

    //printf("TARIH: %02d/%02d/%04d\n", recPara->OKUMA_TARIH.da_day, recPara->OKUMA_TARIH.da_mon, recPara->OKUMA_TARIH.da_year);
    //printf("SAAT: %02d:%02d:%02d:%02d\n", recPara->OKUMA_ZAMAN.ti_hour, recPara->OKUMA_ZAMAN.ti_min, recPara->OKUMA_ZAMAN.ti_sec, recPara->OKUMA_ZAMAN.ti_hund);

    //printf("ISLEM: %02d\n", recPara->ISLEM);


    memcpy(sndPacket.Data,recPara,sndPacket.Len);

    //printf("********SND_PAXET.LEN: %d********************\n", sndPacket.Len);
    //for(i=0;i<sndPacket.Len;i++)
    //	printf("sndPaket[%02d]: %02x\n", i, sndPacket.Data[i]);

    //i = sizeof(recPara->BAKIYE) +  sizeof(recPara->ISLEM) + sizeof(recPara->KARTNO) + sizeof(recPara->OKUMA_TARIH) + sizeof(recPara->OKUMA_ZAMAN) + sizeof(recPara->TUTAR);
    //printf("size fo recPara: %d\n", i);
    memcpy(sndPacket.Data, recPara->KARTNO, 4);
    memcpy(sndPacket.Data+4, &recPara->OKUMA_TARIH.da_year, 1);
    memcpy(sndPacket.Data+6, &recPara->OKUMA_TARIH.da_day, 1);
    memcpy(sndPacket.Data+7, &recPara->OKUMA_TARIH.da_mon, 1);
    memcpy(sndPacket.Data+8, &recPara->OKUMA_ZAMAN.ti_min, 1);
    memcpy(sndPacket.Data+9, &recPara->OKUMA_ZAMAN.ti_hour, 1);
    memcpy(sndPacket.Data+10, &recPara->OKUMA_ZAMAN.ti_hund, 1);
    memcpy(sndPacket.Data+11, &recPara->OKUMA_ZAMAN.ti_sec, 1);
    memcpy(sndPacket.Data+12, &recPara->ISLEM, 1);
    memcpy(sndPacket.Data+13, &recPara->BAKIYE, 4);
    memcpy(sndPacket.Data+17, &recPara->TUTAR, 4);


    ret=TCP_SendRecieve(&sndPacket,&rcvPacket);
    if (ret==0x01)
    {
        memcpy(Response,rcvPacket.Data,rcvPacket.Len);
    }
    return ret;
}

