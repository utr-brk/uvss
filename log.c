/*
 * log.c
 *
 *  Created on: 10 Haz 2014
 *      Author: Burak
 */


#include <stdio.h>
#include <string.h>
//#include <dos.h>
//#include "resource.h"
#include "lcd.h"
#include "log.h"
#include "tcp.h"
#include "config.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include <malloc.h>

//1.1.2
///home/odroid/Solidus/ path'i yoktu, eklendi
#define	FN_LOG_KART 	"/home/odroid/Solidus/LOG001.LOG"
#define	FN_LOG_TERM 	"/home/odroid/Solidus/LOG002.LOG"
#define	FN_LOG_ALARM 	"/home/odroid/Solidus/LOG003.LOG"
#define	FN_INF_KART 	"/home/odroid/Solidus/LOG001.INF"
#define	FN_INF_TERM 	"/home/odroid/Solidus/LOG002.INF"
#define	FN_INF_ALARM 	"/home/odroid/Solidus/LOG003.INF"

//2.5.0
#define	FN_LOG_BACKUP 	"/home/odroid/Solidus/LOG004.LOG"
#define	FN_INF_BACKUP 	"/home/odroid/Solidus/LOG004.INF"

//0.0.5 Para terminali için gerekli
//1.1.2
///home/odroid/Solidus/ path'i yoktu, eklendi
#define	FN_LOG_PARA 	"/home/odroid/Solidus/LOG001.LOG"
#define	FN_INF_PARA 	"/home/odroid/Solidus/LOG001.INF"


#define FMODE_READWRITE	 "r+b"
#define FMODE_WRITECRT	 "w+b"

//1.0.0
#define cmd_LOG_AL		42
#define cmd_OK  		10
#define cAyrac_1		0x01
#define cAyrac_2		2
#define cAyrac_Son		13

FILE * fLogKART;
FILE * fLogTERM;
FILE * fLogALARM;
FILE * fInfKART;
FILE * fInfTERM;
FILE * fInfALARM;

//0.0.5 Para terminali için gerekli
FILE * fLogPARA;
FILE * fInfPARA;

//2.5.0
FILE * fLogBACKUP;
FILE * fInfBACKUP;

struct_LOG_PARA logPARA;
struct_LOG_INFO	infPARA;

//2.5.0
struct_LOG_INFO	infBACKUP;

struct_LOG_KART logKART;
struct_LOG_INFO	infKART;
struct_LOG_TERM logTERM;
struct_LOG_INFO	infTERM;
struct_LOG_ALARM logALARM;
struct_LOG_INFO	infALARM;

char flag_LOG_KART;

char flag_LOG_PARA;//0.0.5

void LOG_InitKART(void)
{
    unsigned long i;

    memset(&logKART,0x00,sizeof(logKART));
    memset(&infKART,0x00,sizeof(infKART));

    fLogKART=fopen(FN_LOG_KART,FMODE_READWRITE);
    if (fLogKART==NULL)
    {
        fLogKART=fopen(FN_LOG_KART,FMODE_WRITECRT);
        for (i=0; i<LOG_KART_MAX; i++)
            fwrite(&logKART,sizeof(logKART),1,fLogKART);
        fclose(fLogKART);
        fLogKART=fopen(FN_LOG_KART,FMODE_READWRITE);

        fInfKART=fopen(FN_INF_KART,FMODE_WRITECRT);
        fwrite(&infKART,sizeof(infKART),1,fInfKART);
        fclose(fInfKART);
    }
    fInfKART=fopen(FN_INF_KART,FMODE_READWRITE);
    fseek(fInfKART,0,SEEK_SET);
    (void)fread(&infKART,sizeof(infKART),1,fInfKART);
    fclose(fInfKART);
    fclose(fLogKART);

    //2.5.0
    //100.000 kayıtlık backup tutmak için
    memset(&logKART,0x00,sizeof(logKART));
    memset(&infBACKUP,0x00,sizeof(infBACKUP));

    fLogBACKUP=fopen(FN_LOG_BACKUP,FMODE_READWRITE);
    if (fLogBACKUP==NULL)
    {
        fLogBACKUP=fopen(FN_LOG_BACKUP,FMODE_WRITECRT);
        for (i=0; i<LOG_KART_MAX; i++)
            fwrite(&logKART,sizeof(logKART),1,fLogBACKUP);
        fclose(fLogBACKUP);
        fLogBACKUP=fopen(FN_LOG_BACKUP,FMODE_READWRITE);

        fInfBACKUP=fopen(FN_INF_BACKUP,FMODE_WRITECRT);
        fwrite(&infBACKUP,sizeof(infBACKUP),1,fInfBACKUP);
        fclose(fInfBACKUP);
    }
    fInfBACKUP=fopen(FN_INF_BACKUP,FMODE_READWRITE);
    fseek(fInfBACKUP,0,SEEK_SET);
    (void)fread(&infBACKUP,sizeof(infBACKUP),1,fInfBACKUP);
    fclose(fInfBACKUP);
    fclose(fLogBACKUP);
}

void LOG_InitPARA(void)
{
    unsigned long i;

    memset(&logPARA,0x00,sizeof(logPARA));
    memset(&infPARA,0x00,sizeof(infPARA));

    fLogPARA=fopen(FN_LOG_PARA,FMODE_READWRITE);
    if (fLogPARA==NULL)
    {
        fLogPARA=fopen(FN_LOG_PARA,FMODE_WRITECRT);
        for (i=0; i<LOG_PARA_MAX; i++)
            fwrite(&logPARA,sizeof(logPARA),1,fLogPARA);
        fclose(fLogPARA);
        fLogPARA=fopen(FN_LOG_PARA,FMODE_READWRITE);

        fInfPARA=fopen(FN_INF_PARA,FMODE_WRITECRT);
        fwrite(&infPARA,sizeof(infPARA),1,fInfPARA);
        fclose(fInfPARA);
    }
    fInfPARA=fopen(FN_INF_PARA,FMODE_READWRITE);
    fseek(fInfPARA,0,SEEK_SET);
    fread(&infPARA,sizeof(infPARA),1,fInfPARA);
    fclose(fInfPARA);
    fclose(fLogPARA);
}

void LOG_InitTERM(void)
{
    unsigned long i;

    memset(&logTERM,0x00,sizeof(logTERM));
    memset(&infTERM,0x00,sizeof(infTERM));

    fLogTERM=fopen(FN_LOG_TERM,FMODE_READWRITE);
    if (fLogTERM==NULL)
    {
        fLogTERM=fopen(FN_LOG_TERM,FMODE_WRITECRT);
        for (i=0; i<LOG_TERM_MAX; i++)
            fwrite(&logTERM,sizeof(logTERM),1,fLogTERM);
        fclose(fLogTERM);
        fLogTERM=fopen(FN_LOG_TERM,FMODE_READWRITE);

        fInfTERM=fopen(FN_INF_TERM,FMODE_WRITECRT);
        fwrite(&infTERM,sizeof(infTERM),1,fInfTERM);
        fclose(fInfTERM);
    }
    fInfTERM=fopen(FN_INF_TERM,FMODE_READWRITE);
    fseek(fInfTERM,0,SEEK_SET);
    fread(&infTERM,sizeof(infTERM),1,fInfTERM);
    fclose(fInfTERM);
    fclose(fLogTERM);
}


void LOG_InitALARM(void)
{
    unsigned long i;

    memset(&logALARM,0x00,sizeof(logALARM));
    memset(&infALARM,0x00,sizeof(infALARM));

    fLogALARM=fopen(FN_LOG_ALARM,FMODE_READWRITE);
    if (fLogALARM==NULL)
    {
        fLogALARM=fopen(FN_LOG_ALARM,FMODE_WRITECRT);
        for (i=0; i<LOG_ALARM_MAX; i++)
            fwrite(&logALARM,sizeof(logALARM),1,fLogALARM);
        fclose(fLogALARM);
        fLogALARM=fopen(FN_LOG_ALARM,FMODE_READWRITE);

        fInfALARM=fopen(FN_INF_ALARM,FMODE_WRITECRT);
        fwrite(&infALARM,sizeof(infALARM),1,fInfALARM);
        fclose(fInfALARM);
    }
    fInfALARM=fopen(FN_INF_ALARM,FMODE_READWRITE);
    fseek(fInfALARM,0,SEEK_SET);
    fread(&infALARM,sizeof(infALARM),1,fInfALARM);
    fclose(fInfALARM);
    fclose(fLogALARM);
}


void LOG_Init(void)
{
    lcd_clean();
    lcd_printf(ALG_CENTER,"LOGLAR");
    lcd_printf(ALG_CENTER,"YUKLENIYOR");
    lcd_flip();
    if(rec_TERM.TERM_TIP == TERM_PARA)
    {
        LOG_InitPARA();
        flag_LOG_PARA = 0;
    }
    else
    {
        LOG_InitKART();
        flag_LOG_KART = 0;
    }
    LOG_InitTERM();
    LOG_InitALARM();

}



char LOG_Add_KART(struct_LOG_KART *rlKART)
{
    /*
    if (LOG_KART_MAX==infKART.SAY) //Log Full
    {
    	return 0x00;
    }
    */
    fLogKART=fopen(FN_LOG_KART,FMODE_READWRITE);
    fInfKART=fopen(FN_INF_KART,FMODE_READWRITE);

    memcpy(&logKART,rlKART,sizeof(logKART));
    fseek(fLogKART,infKART.SON*sizeof(logKART),SEEK_SET); //Son Pozisyona Git
    fwrite(&logKART,sizeof(logKART),1,fLogKART);

    /*
    infKART.SAY++;
    infKART.SON++;
    if (infKART.SON>(LOG_KART_MAX-1)) //Basa Don
    {
    	infKART.SON=0;
    }
    */
    infKART.SAY++;
    if(infKART.SAY > LOG_KART_MAX)
    {
        infKART.SAY = LOG_KART_MAX;//sayıyı max'da sabitle
        infKART.ILK++;//ilk'i ilerlet
        if(infKART.ILK > (LOG_KART_MAX-1))//başa dön
        {
            infKART.ILK = 0;
        }
    }
    infKART.SON++;
    if(infKART.SON > (LOG_KART_MAX-1))//başa dön
    {
        infKART.SON = 0;
    }
    fseek(fInfKART,0,SEEK_SET);
    fwrite(&infKART,sizeof(infKART),1,fInfKART);
    fclose(fLogKART);
    fclose(fInfKART);

    //2.5.0
    fLogBACKUP=fopen(FN_LOG_BACKUP,FMODE_READWRITE);
    fInfBACKUP=fopen(FN_INF_BACKUP,FMODE_READWRITE);

    memcpy(&logKART,rlKART,sizeof(logKART));
    fseek(fLogBACKUP,infBACKUP.SON*sizeof(logKART),SEEK_SET); //Son Pozisyona Git
    fwrite(&logKART,sizeof(logKART),1,fLogBACKUP);

    infBACKUP.SAY++;
    if(infBACKUP.SAY > LOG_KART_MAX)
    {
        infBACKUP.SAY = LOG_KART_MAX;//sayıyı max'da sabitle
        infBACKUP.ILK++;//ilk'i ilerlet
        if(infBACKUP.ILK > (LOG_KART_MAX-1))//başa dön
        {
            infBACKUP.ILK = 0;
        }
    }
    infBACKUP.SON++;
    if(infBACKUP.SON > (LOG_KART_MAX-1))//başa dön
    {
        infBACKUP.SON = 0;
    }
    fseek(fInfBACKUP,0,SEEK_SET);
    fwrite(&infBACKUP,sizeof(infBACKUP),1,fInfBACKUP);
    fclose(fLogBACKUP);
    fclose(fInfBACKUP);
    return 0x01;
}

char LOG_Read_KART(struct_LOG_KART *rlKART)
{

    if (infKART.SAY==0) return 0x00;

    fLogKART=fopen(FN_LOG_KART,FMODE_READWRITE);
    fInfKART=fopen(FN_INF_KART,FMODE_READWRITE);

    memset(&logKART,0x00,sizeof(logKART));
    fseek(fLogKART,infKART.ILK*sizeof(logKART),SEEK_SET); //Ilk Pozisyona Git
    fread(&logKART,sizeof(logKART),1,fLogKART); //Oku
    memcpy(rlKART,&logKART,sizeof(logKART));

    fclose(fLogKART);
    fclose(fInfKART);

    return 0x01;
}

char LOG_Del_KART(void)
{

    if (infKART.SAY==0) return 0x00;

    fLogKART=fopen(FN_LOG_KART,FMODE_READWRITE);
    fInfKART=fopen(FN_INF_KART,FMODE_READWRITE);

    memset(&logKART,0x00,sizeof(logKART));
    fseek(fLogKART,infKART.ILK*sizeof(logKART),SEEK_SET); //Ilk Pozisyona Git
    fwrite(&logKART,sizeof(logKART),1,fLogKART); //Bosalt

    infKART.SAY--;
    infKART.ILK++;
    if (infKART.ILK>(LOG_KART_MAX-1)) //Basa Don
    {
        infKART.ILK=0;
    }
    fseek(fInfKART,0,SEEK_SET);
    fwrite(&infKART,sizeof(infKART),1,fInfKART);

    fclose(fLogKART);
    fclose(fInfKART);

    return 0x01;
}


char LOG_Add_TERM(struct_LOG_TERM *rlTERM)
{
    /*
    if (LOG_TERM_MAX==infTERM.SAY) //Log Full
    {
    	return 0x00;
    }
    */
    fLogTERM=fopen(FN_LOG_TERM,FMODE_READWRITE);
    fInfTERM=fopen(FN_INF_TERM,FMODE_READWRITE);

    memcpy(&logTERM,rlTERM,sizeof(logTERM));
    fseek(fLogTERM,infTERM.SON*sizeof(logTERM),SEEK_SET); //Son Pozisyona Git
    fwrite(&logTERM,sizeof(logTERM),1,fLogTERM);

    infTERM.SAY++;
    if(infTERM.SAY > LOG_TERM_MAX)
    {
        infTERM.SAY = LOG_TERM_MAX;//sayıyı max'da sabitle
        infTERM.ILK++;//ilk'i ilerlet
        if(infTERM.ILK > (LOG_TERM_MAX-1))//başa dön
        {
            infTERM.ILK = 0;
        }
    }
    infTERM.SON++;
    if(infTERM.SON > (LOG_TERM_MAX-1))//başa dön
    {
        infTERM.SON = 0;
    }
    /*
    infTERM.SAY++;
    infTERM.SON++;
    if (infTERM.SON>(LOG_TERM_MAX-1)) //Basa Don
    {
    	infTERM.SON=0;
    }
    */
    fseek(fInfTERM,0,SEEK_SET);
    fwrite(&infTERM,sizeof(infTERM),1,fInfTERM);

    fclose(fLogTERM);
    fclose(fInfTERM);

    return 0x01;
}

char LOG_Read_TERM(struct_LOG_TERM *rlTERM)
{

    if (infTERM.SAY==0) return 0x00;

    fLogTERM=fopen(FN_LOG_TERM,FMODE_READWRITE);
    fInfTERM=fopen(FN_INF_TERM,FMODE_READWRITE);

    memset(&logTERM,0x00,sizeof(logTERM));
    fseek(fLogTERM,infTERM.ILK*sizeof(logTERM),SEEK_SET); //Ilk Pozisyona Git
    fread(&logTERM,sizeof(logTERM),1,fLogTERM); //Oku
    memcpy(rlTERM,&logTERM,sizeof(logTERM));

    fclose(fLogTERM);
    fclose(fInfTERM);

    return 0x01;
}

char LOG_Del_TERM(void)
{

    if (infTERM.SAY==0) return 0x00;

    fLogTERM=fopen(FN_LOG_TERM,FMODE_READWRITE);
    fInfTERM=fopen(FN_INF_TERM,FMODE_READWRITE);

    memset(&logTERM,0x00,sizeof(logTERM));
    fseek(fLogTERM,infTERM.ILK*sizeof(logTERM),SEEK_SET); //Ilk Pozisyona Git
    fwrite(&logTERM,sizeof(logTERM),1,fLogTERM); //Bosalt

    infTERM.SAY--;
    infTERM.ILK++;
    if (infTERM.ILK>(LOG_TERM_MAX-1)) //Basa Don
    {
        infTERM.ILK=0;
    }
    fseek(fInfTERM,0,SEEK_SET);
    fwrite(&infTERM,sizeof(infTERM),1,fInfTERM);

    fclose(fLogTERM);
    fclose(fInfTERM);

    return 0x01;
}

char LOG_Add_ALARM(struct_LOG_ALARM *rlALARM)
{
    /*
    if (LOG_ALARM_MAX==infALARM.SAY) //Log Full
    {
    	return 0x00;
    }
    */
    fLogALARM=fopen(FN_LOG_ALARM,FMODE_READWRITE);
    fInfALARM=fopen(FN_INF_ALARM,FMODE_READWRITE);

    memcpy(&logALARM,rlALARM,sizeof(logALARM));
    fseek(fLogALARM,infALARM.SON*sizeof(logALARM),SEEK_SET); //Son Pozisyona Git
    fwrite(&logALARM,sizeof(logALARM),1,fLogALARM);

    /*
    infALARM.SAY++;
    infALARM.SON++;
    if (infALARM.SON>(LOG_ALARM_MAX-1)) //Basa Don
    {
    	infALARM.SON=0;
    }
    */
    infALARM.SAY++;
    if(infALARM.SAY > LOG_ALARM_MAX)
    {
        infALARM.SAY = LOG_ALARM_MAX;//sayıyı max'da sabitle
        infALARM.ILK++;//ilk'i ilerlet
        if(infALARM.ILK > (LOG_ALARM_MAX-1))//başa dön
        {
            infALARM.ILK = 0;
        }
    }
    infALARM.SON++;
    if(infALARM.SON > (LOG_ALARM_MAX-1))//başa dön
    {
        infALARM.SON = 0;
    }

    fseek(fInfALARM,0,SEEK_SET);
    fwrite(&infALARM,sizeof(infALARM),1,fInfALARM);

    fclose(fLogALARM);
    fclose(fInfALARM);

    return 0x01;
}

char LOG_Read_ALARM(struct_LOG_ALARM *rlALARM)
{

    if (infALARM.SAY==0) return 0x00;

    fLogALARM=fopen(FN_LOG_ALARM,FMODE_READWRITE);
    fInfALARM=fopen(FN_INF_ALARM,FMODE_READWRITE);

    memset(&logALARM,0x00,sizeof(logALARM));
    fseek(fLogALARM,infALARM.ILK*sizeof(logALARM),SEEK_SET); //Ilk Pozisyona Git
    fread(&logALARM,sizeof(logALARM),1,fLogALARM); //Oku
    memcpy(rlALARM,&logALARM,sizeof(logALARM));

    fclose(fLogALARM);
    fclose(fInfALARM);

    return 0x01;
}

char LOG_Del_ALARM(void)
{

    if (infALARM.SAY==0) return 0x00;

    fLogALARM=fopen(FN_LOG_ALARM,FMODE_READWRITE);
    fInfALARM=fopen(FN_INF_ALARM,FMODE_READWRITE);

    memset(&logALARM,0x00,sizeof(logALARM));
    fseek(fLogALARM,infALARM.ILK*sizeof(logALARM),SEEK_SET); //Ilk Pozisyona Git
    fwrite(&logALARM,sizeof(logALARM),1,fLogALARM); //Bosalt

    infALARM.SAY--;
    infALARM.ILK++;
    if (infALARM.ILK>(LOG_ALARM_MAX-1)) //Basa Don
    {
        infALARM.ILK=0;
    }
    fseek(fInfALARM,0,SEEK_SET);
    fwrite(&infALARM,sizeof(infALARM),1,fInfALARM);

    fclose(fLogALARM);
    fclose(fInfALARM);

    return 0x01;
}

char Log_Run_Kapi(void)
{
//2.4.2
//char *szLog;
    char *szBuf;
//4.8.800
//dosya buyukse fLen overflow oluyor,
//log.txt dosyasi silindigiyle kaliyordu.
//int fLen,i;
    long fLen,i;
//char ok=0;
    char rtn=0;
    FILE  *fp;
    FILE  *fx;
    fd_set readset;
    int result, iof = -1;
    struct timeval tv;
    unsigned char *strRcv;
    int log_socket;
    int retval = -1;
//2.4.2
    size_t n=0;
    char *szLog = NULL;
    ssize_t log_len = 0;

    fp = fopen("/home/odroid/Solidus/log.txt", "rb");
    if(fp == NULL) return rtn;
    strRcv = (unsigned char *) calloc (2049, 1);
    fseek(fp,0L,SEEK_END);
    fLen=ftell(fp);
    if (fLen==0)
    {
        fclose(fp);
        free(strRcv);
        return rtn;
    }
    //2.1.0
    //szLog = (char *) calloc(33,1);
    //2.4.1
    //iskur ise 38 byte (çünkü barkod uzunluğu 15 byte),
    //değil ise 33 byte (çünkü kart no uzunluğu 10 byte)
    //szLog = (char *) calloc(38,1);
    //2.4.2
    /*
    if(ISKUR)
    {
        szLog = (char *) calloc(38,1);
    }else
    {
        szLog = (char *) calloc(33,1);
    }
    */
    szBuf = (char *) calloc(64,1);

    fseek(fp,0L,SEEK_SET);
    //1.0.1
    //1 byte eksik gidiyordu data
    //2.1.0
    //barkod 15 char
    //fread(szLog,31,1,fp);
    //2.4.1
    //fread(szLog,36,1,fp);
    //2.4.2
    /*
    if(ISKUR)
    {
        fread(szLog,36,1,fp);
    }else
    {
        fread(szLog,31,1,fp);
    }
    */
    log_len = getline(&szLog, &n, fp);
    //2.1.0
    //if (szLog[29]=='1') rtn=1;
    //2.4.1
    //if (szLog[34]=='1') rtn=1;
    if(ISKUR)
    {
        if (szLog[34]=='1') rtn=1;
    }
    else
    {
        if (szLog[29]=='1') rtn=1;
    }

    sprintf(szBuf,"%c%c%s%c",
            cmd_LOG_AL,cAyrac_1,
            szLog,cAyrac_Son);

    log_socket = TCP_Connect(rec_TERM.IP_SERVER, rec_TERM.PORT_SERVER);
    if (log_socket>0)
    {
        write(log_socket, szBuf, strlen((char *)szBuf));
        // Initialize the set
        FD_ZERO(&readset);
        FD_SET(log_socket, &readset);

        // Initialize time out struct
        //2.4.0
        /*
        tv.tv_sec = 0;
        tv.tv_usec = 2000 * 1000;*/
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        // select()
        result = select(log_socket+1, &readset, NULL, NULL, &tv);

        // Check status
        if (result < 0)
        {
            //debug_print("tcp_error");
            retval = 0x1;
        }
        else if (result > 0 && FD_ISSET(log_socket, &readset))
        {
            // Set non-blocking mode
            if ((iof = fcntl(log_socket, F_GETFL, 0)) != -1)
                fcntl(log_socket, F_SETFL, iof | O_NONBLOCK);
            // receive
            result = recv(log_socket, strRcv, 2048, 0);
            // set as before
            if (iof != -1)
                fcntl(log_socket, F_SETFL, iof);
            //return result;
            //debug_print("tcp read: %d bytes", result);
            retval = 0;
        }

        if (retval==0)
        {
            retval=1;
            //Length is in result
            //L = result;
            if (strRcv[0]==cmd_OK)
            {
                retval=0;//izin var
            }
        }


        //2.5.2
        //zaten buyuk olmasa buraya giremez
        //if (log_socket >=0)
        close(log_socket);
        log_socket = -1;
    }
    else
        retval = -1;//sunucu yok

    if ((retval==0) || rtn)
    {
        remove("/home/odroid/Solidus/log.x");
        fx = fopen("/home/odroid/Solidus/log.x", "a+b");
        //1.1.3
        //32 olarak kalmış halbuki, 31 byte data + CR+LF toplam 33 byte her satır
        //2.1.0
        //barkod yuzunden 5 byte arttı
        //i=33;
        //2.4.1
        //i=38;
        //2.4.2
        /*
        if(ISKUR)
        {
          i=38;
        }
        else{
          i=33;
        }
        */
        i = log_len;
        while (i<fLen)
        {
            fseek(fp,i,SEEK_SET);
            //1.1.1
            //1 byte eksik kalıyordu cunku kapi kodu 2 degil 3 byte cikti
            //o yuzden 32'den 33'e çıkartıldı
            //2.1.0
            //barkod yuzunden 5 byte arttı
            //fread(szLog,33,1,fp);
            //fwrite(szLog,33,1,fx);
            //i+=33;
            //2.4.1
            //fread(szLog,38,1,fp);
            //fwrite(szLog,38,1,fx);
            //i+=38;

            //2.4.2
            /*
            if(ISKUR)
            {   //2.4.2
            //burası test edilip, düzgün bir biçimde yazılacak.
                size_t n=0;
                char *line = NULL;
                log_len = getline(&line, &n, fp);
                fwrite(szLog,log_len,1,fx);
                //fread(szLog,38,1,fp);
                //fwrite(szLog,38,1,fx);
                i+=38;
            }else
            {
                fread(szLog,33,1,fp);
                fwrite(szLog,33,1,fx);
                i+=33;
            }
            */
            log_len = getline(&szLog, &n, fp);
            fwrite(szLog,log_len,1,fx);
            i+=log_len;
        }
        fclose(fp);
        fclose(fx);
        remove("/home/odroid/Solidus/log.txt");
        rename("/home/odroid/Solidus/log.x","/home/odroid/Solidus/log.txt");
        rtn=1;
    }
    else
    {
        fclose(fp);
    }
    //2.4.2
    //free(szLog);
    if(szLog) free(szLog);
    free(szBuf);
    free(strRcv);
    return rtn;
}

void LOG_Run(void)
{
    struct_LOG_KART recLOGKART;
    int count = 3;

    flag_LOG_KART=0;
    while(count)
    {
        LOG_InitKART();
        if (infKART.SAY>0)
        {
            if (LOG_Read_KART(&recLOGKART))
            {
                if (!recLOGKART.GECTI_ONLINE)
                {
                    if (TCP_LogKart(&recLOGKART))
                    {
                        LOG_Del_KART();
                        flag_LOG_KART=1;
                        count--;
                    }
                    else
                    {
                        break;//0.0.8 gonderemezsen cik
                    }
                }
                else
                {
                    LOG_Del_KART();
                }
            }

        }
        else
        {
            break;
        }
    }
}

//0.0.5 Para Logları için yapıldı
char LOG_Add_PARA(struct_LOG_PARA *rlPARA)
{
    if (LOG_PARA_MAX==infPARA.SAY) //Log Full
    {
        return 0x00;
    }

    fLogPARA=fopen(FN_LOG_PARA,FMODE_READWRITE);
    fInfPARA=fopen(FN_INF_PARA,FMODE_READWRITE);

    memcpy(&logPARA,rlPARA,sizeof(logPARA));
    fseek(fLogPARA,infPARA.SON*sizeof(logPARA),SEEK_SET); //Son Pozisyona Git
    fwrite(&logPARA,sizeof(logPARA),1,fLogPARA);

    infPARA.SAY++;
    infPARA.SON++;
    if (infPARA.SON>(LOG_PARA_MAX-1)) //Basa Don
    {
        infPARA.SON=0;
    }
    fseek(fInfPARA,0,SEEK_SET);
    fwrite(&infPARA,sizeof(infPARA),1,fInfPARA);
    fclose(fLogPARA);
    fclose(fInfPARA);
    //flag_LOG_PARA = 1;
    return 0x01;
}

char LOG_Read_PARA(struct_LOG_PARA *rlPARA)
{

    if (infPARA.SAY==0) return 0x00;

    fLogPARA=fopen(FN_LOG_PARA,FMODE_READWRITE);
    fInfPARA=fopen(FN_INF_PARA,FMODE_READWRITE);

    memset(&logPARA,0x00,sizeof(logPARA));
    fseek(fLogPARA,infPARA.ILK*sizeof(logPARA),SEEK_SET); //Ilk Pozisyona Git
    fread(&logPARA,sizeof(logPARA),1,fLogPARA); //Oku
    memcpy(rlPARA,&logPARA,sizeof(logPARA));

    fclose(fLogPARA);
    fclose(fInfPARA);

    return 0x01;
}

char LOG_Del_PARA(void)
{

    if (infPARA.SAY==0) return 0x00;

    fLogPARA=fopen(FN_LOG_PARA,FMODE_READWRITE);
    fInfPARA=fopen(FN_INF_PARA,FMODE_READWRITE);

    memset(&logPARA,0x00,sizeof(logPARA));
    fseek(fLogPARA,infPARA.ILK*sizeof(logPARA),SEEK_SET); //Ilk Pozisyona Git
    fwrite(&logPARA,sizeof(logPARA),1,fLogPARA); //Bosalt

    infPARA.SAY--;
    infPARA.ILK++;
    if (infPARA.ILK>(LOG_PARA_MAX-1)) //Basa Don
    {
        infPARA.ILK=0;
    }
    fseek(fInfPARA,0,SEEK_SET);
    fwrite(&infPARA,sizeof(infPARA),1,fInfPARA);

    fclose(fLogPARA);
    fclose(fInfPARA);

    return 0x01;
}


void LOG_Run_Para(void)
{
    struct_LOG_PARA recLOGPARA;
    //debug_print("flag_LOG_PARA: %d", flag_LOG_PARA);
    if (flag_LOG_PARA) //Kart Loglar Gonderilecekse
    {
        while(1)
        {
            if (infPARA.SAY>0)
            {
                if (LOG_Read_PARA(&recLOGPARA))
                {
                    if(!recLOGPARA.CEVAP_ONLINE)
                    {
                        if (TCP_LogPara(&recLOGPARA))
                        {
                            LOG_Del_PARA();
                            flag_LOG_PARA = 1;
                        }
                        else
                        {
                            flag_LOG_PARA=0; //Sonraki Sefere
                            break;//0.0.8 gonderemediysen cik.
                        }
                    }
                    else
                    {
                        LOG_Del_PARA();
                    }
                }

            }
            else
            {
                flag_LOG_PARA=0;
                break;
            }
        }
    }
}



