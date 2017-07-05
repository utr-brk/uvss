/*
 * tool.c
 *
 *  Created on: 10 Haz 2014
 *      Author: Burak
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
//#include "timer.h"
#include "tool.h"


//2.1.0
char TOOL_GetSplit(char* FullText,int Pos,char Ayrac, char* Ret)
{
int i,j,p,tLen;
 char* r;
char rtn;

    r=( char*) malloc(1024);

    memset(r,0x00,1024);
    tLen = strlen(FullText);
    p=0;j=0;
    rtn=0;
    for (i=0;i<tLen;i++)
    {
        if (FullText[i]==Ayrac)
        {
            p++;
        } else if (p==Pos) {
            rtn=1;
            r[j]=FullText[i];
            j++;
        }
        if (p>Pos) break;
    }

    memcpy(Ret,r,strlen(r)+1);
    free(r);
    return rtn;
}


void get_date(struct_date *datep)
{
struct tm * timeinfo;
time_t rawtime;

 	time(&rawtime);
	timeinfo = localtime(&rawtime);

	datep->da_day 	= (char) timeinfo->tm_mday;
	datep->da_mon 	= (char) timeinfo->tm_mon+1;
	datep->da_year 	= (uint16_t)timeinfo->tm_year + 1900;

}

void get_time(struct_time *timep)
{
struct tm * timeinfo;
time_t rawtime;

 	time(&rawtime);
	timeinfo = localtime(&rawtime);

	timep->ti_hour = (unsigned char) timeinfo->tm_hour;
	timep->ti_min = (unsigned char) timeinfo->tm_min;
	timep->ti_sec = (unsigned char) timeinfo->tm_sec;
	timep->ti_hund = 0;
}

/*
void aDelay(long MiliSec)
{
	//m8210 burası komple değişecek
	unsigned long long t;
	//t=Timer_Set(MiliSec);
	//while (!Timer_End(t));
	//nanosleep()
}
*/

unsigned char aBinToHex(char* Bin)
{
	unsigned char r=0;
	unsigned char BIT[8] ={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
	unsigned char i;
	for (i=0;i<8;i++)
		if(Bin[i]=='1') r|=BIT[i];
	return r;
}

void aHexToBin(unsigned char Hex,unsigned char* Bin)
{
	unsigned char BIT[8] ={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
	unsigned char * rBin;
	unsigned char i;
	rBin = (unsigned char*) calloc(10,1); //0.0.6 malloc'du calloc oldu
	//memset(rBin,'0',8); //calloc olunca gerek kalmadi
	for (i=0;i<8;i++)
		if ((Hex&BIT[i])==BIT[i]) rBin[i]='1';
	memcpy(Bin,rBin,8);
	free(rBin);
}

/*
unsigned long aPower(unsigned long X,unsigned long Y)
{
	if (Y==0)
	{
		return 1;
	} else {
		return X * aPower(X,Y-1);
	}
}
*/
/*
unsigned long aHexToLong(char *Hex)
{
	//unsigned char HEX_TABLE[17]="0123456789ABCDEF";
	char HEX_TABLE[17]="0123456789ABCDEF";
	unsigned long r=0;
	unsigned int i,l;
	l=strlen(Hex);
	i=l;
	while (i>0)
	{
		r = r + aPower(16,l-i) * (strchr(HEX_TABLE,Hex[i-1])-HEX_TABLE);
		i--;
	}
	return r;
}
*/
unsigned int aHexToInt(char *hexstr)
{
	unsigned char HEX_2_INT_TABLE[132] ={0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
						0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
						0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5,
						6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0, 10, 11, 12, 13, 14, 15, 0, 0,
						0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
						0, 0, 0, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
						0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	unsigned int length, i, value, shift;
	for (length = 0; length < 9; length++) if (!hexstr[length]) break;
	shift = (length - 1) * 4;
	for (i = value = 0; i < length; i++, shift -= 4)
		value += HEX_2_INT_TABLE[(unsigned int)hexstr[i] & 127] << shift;
	return value;
}

//unsigned int aHexToString(unsigned char *strHex,unsigned char *strByte)
/*
 int aHexToString(char *strHex, char *strByte)
{
	unsigned int L,i,j;
	unsigned char * Hex;

	Hex = (unsigned char*) malloc(3);
	L=strlen(strHex);
	i=0;
	j=0;
	memset(Hex,0,3);
	while (i<L)
	{
		memcpy(Hex,strHex+i,2);
		strByte[j] = (unsigned char) aHexToLong(Hex);
		i+=2;
		j++;
	}
	free(Hex);
	return j;
}
*/
/*
unsigned int aStringToHex(unsigned char *strByte,unsigned int Len,unsigned char *strHex)
{
	unsigned int i;
	strHex[0]=0x00;
	for (i=0;i<Len;i++)
		sprintf(strHex+(i*2),"%02X",strByte[i]);
	return i;
}
*/
char aDayOfWeek(void)
{
	time_t t;
	char *str;
	char ret;

	str=(char*) calloc(100, 1);//0.0.6 malloc'du calloc oldu
	time(&t);
	strcpy(str, ctime(&t));
	if (strncmp(str,"Mon",3)==0) ret=1;
	if (strncmp(str,"Tue",3)==0) ret=2;
	if (strncmp(str,"Wed",3)==0) ret=3;
	if (strncmp(str,"Thu",3)==0) ret=4;
	if (strncmp(str,"Fri",3)==0) ret=5;
	if (strncmp(str,"Sat",3)==0) ret=6;
	if (strncmp(str,"Sun",3)==0) ret=7;
//  printf("%s   %d\n",str,ret);
	free(str);
	return ret;
}


//unsigned char aFileexist(unsigned char *FileName)
 char aFileexist(char *FileName)
{
	FILE *f;
	unsigned char r=0x00;
	f=fopen(FileName,"rb");
	if (f!=NULL)
	{
		fclose(f);
		r=0x01;
	}
	return r;
}
 //m8210 bakılacak, gerekmeyebilir
 /*
void aWinToDos(unsigned char *Win)

{
    int L=strlen(Win);
    int i;
    unsigned char c;

    for (i=0;i<L;i++)
    {
        c=Win[i];
        switch (c)
        {
            case 'Ğ':
                     c='¦';
                     break;
            case 'Ü':
                     c='š';
                     break;
            case 'Ş':
                     c='';
                     break;
            case 'İ':
                     c='˜';
                     break;
            case 'Ö':
                     c='™';
                     break;
            case 'Ç':
                     c='€';
                     break;
        }
        Win[i]=c;
    }
}
*/
