#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "serport.h"
#include "tool.h"
#include "com.h"

#include "lcd.h"
//1.0.0
#include "config.h"

//#include "acctimer.h"

//artik 115.200 baudda aciliyor
/*
burada com portlar initialize edilerek,
ilgili portun file descriptor'ı oluşturulur.
seri portlar üzerinde tüm işlemler bu file descriptor ile yapılacak.
ifd[com_port]
*/
unsigned char COM_init(int com_port, int *srl)
{

	switch(com_port){
		//odroid
		//aşağısını ttyS0 olarak değiştirmeyi unutma.
		case COM1: srl[0] = serialOpen("/dev/ttyS0", Okuyucu_HIZ[0]);
                    if(srl[0]<0) //CON5
                        printf("error opening com1\n");
                    //else
                      //  printf("com1 opened");
                    //1.8.0
                    else
                        serialFlush(srl[0]);
                break;
		case COM2: srl[1] =serialOpen("/dev/ttyS2", Okuyucu_HIZ[1]);//s2 olacak
                    if(srl[1]<0)
                        printf("error opening com2\n");
                    //1.8.0
                    else
                        serialFlush(srl[1]);
            break;//40-pin, #8 & #10
		default:
            break;
	}
	return 1;
}

void COM_uninit(int fd)
{
	serialClose(fd);
}

/*
milli second cinsinden max kadar bekleyecek
*/
unsigned int COM_avail(int fd,unsigned int max)
{
	//long ZA;
    int r=0;
	//time_t then;
    struct timeval tNow, tLong, tEnd;
	//time(&ftp_end_time);
	//if(difftime(ftp_end_time, ftp_start_time) > FTP_TIME_OUT)

    //en fazla 200 msec bekle
	if(max > 500) max = 500;//6.2.0 200 idi
/*
	ZA = AccTimer_SetTime(max);

	while ((!r)&&(!AccTimer_IsTimeOut(ZA)))
	{
		r=SerPort_Avail(com_port);
//		printf("serport avail: %d\n", r);
//		if(SerPort_IsParityError(COM1) !=0 )
//			printf("parity error\n");

	}
*/
//	printf("r1: %d\n", r);
//	r=0;
//	r=SerPort_Avail(com_port);
//	printf("r2: %d\n", r);

    //odroid
    //buraya bakmayı unutma
    //then = time (NULL) + max ;
    tLong.tv_sec = 0;
    tLong.tv_usec = max * 1000L;
    gettimeofday(&tNow, NULL);
    timeradd(&tNow, &tLong, &tEnd);
    while (timercmp(&tNow, &tEnd, <))
    {
        r = serialDataAvail(fd);
        //1.9.1
        //if(r){//1.6.0
        //2.5.1
        //if(r>5){//1.6.0
        //2.5.1
        /*
        if(r>3){
            napms(5);
            break;
        }*/
        if(r) break;
        gettimeofday(&tNow, NULL);
    }/*
    then = time (NULL) + max ;
    while (time (NULL) < then){
        r = serialDataAvail(fd);
        if(r)
            break;
    }*/

    //lcd_printf(ALG_CENTER, "r:%d", r);

	return r;
}
/*
unsigned char COM_first_handshake(int fd)
{
	//unsigned char c;
	int c;
	//SerPort_Clear(com_port);
	serialFlush(fd);
	//SerPort_SendByte(com_port, 0x23);
	serialPutchar(fd, 0x23);
	if(!COM_avail(fd, 10))
		return 0x00;
	//SerPort_RecvByte(com_port, &c);
	c = serialGetchar(fd);
	if(c != 0x45)
		return 0x00;
	return 0x01;
}

unsigned char COM_second_handshake(int fd)
{
	//unsigned char c;
	int c;
	if(!COM_avail(fd,100)) return 0x00;
	//SerPort_RecvByte(com_port, &c);
	c = serialGetchar(fd);
	if(c != 0x67)	return 0x00;
	//SerPort_SendByte(com_port, 0x89);
	serialPutchar(fd, 0x89);
	return 0x01;
}
*/
//2.5.1
/*
unsigned int COM_ReadBuffer(int fd, unsigned char * Buffer)
{
	unsigned int i,j,max;
	char c;
	max = COM_avail(fd,200); //with 200 msec timeout
	if (max==0) return 0;
	j=0;
	while (max>0)
	{
		for(i=0;i<max;i++)
		{
			c = serialGetchar(fd);
			Buffer[j]=c;
			j++;
		}
		max=COM_avail(fd,max*3);
	}

    //1.8.0
	//serialFlush(fd);
	return j;
}
*/
char COM_SendBuffer(int fd,const char * Buffer,unsigned int Len)
{
	//if (SerPort_SendBuf(com_port,Buffer,Len))
	//if (serialPuts(fd,Buffer))
	//odroid
	//serialPuts void ama değiştirmekte fayda var.
	//write ne kadar yazdığını return etmiyor mu?
	//ona göre kontrol ederek cevap dönsün
	serialPuts(fd,Buffer);
	//{
	//	return 0x01;
	//} else {
		return 0x00;
	//}
}

unsigned int COM_ReadBuff(int fd, char * Buffer, int adet)
{
unsigned int i, max;
int c;

	max = COM_avail(fd, 500);
	if (max<adet) return 0;

    for(i=0;i<adet;i++)
	{
		c = serialGetchar(fd);
		Buffer[i]=(char)c;
    }
	return i;
}
