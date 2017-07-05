/*
 * resource.c
 *
 *  Created on: 9 Haz 2014
 *      Author: Burak
 */


//#include <dos.h>
//#include <alloc.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "resource.h"
#include "lcd.h"
//#include <posapi.h>

//1.2.0
//#define RES_NAME "/home/odroid/Solidus/MRC300.RES"

#pragma pack(push,1)
typedef struct {
	unsigned short int	NO;
	char        LINE[257];
} struct_RESOURCE;

struct_RESOURCE		RES_REC;

#pragma pack(pop)

FILE*   			RES_FILE;

long 				RES_REC_LEN;
long				RES_FILE_LEN;
char*				RES_FILE_NAME;


char RESOURCE_Init(char * RES_NAME)
{

	RES_FILE_NAME=(char*) calloc(255, 1);//0.0.6 malloc'du calloc oldu
	sprintf(RES_FILE_NAME,"%s",RES_NAME);

	RES_FILE=fopen(RES_FILE_NAME,"rb");

	if (RES_FILE==NULL) return 0;

	RES_REC_LEN = sizeof(RES_REC);
	//RES_REC_LEN = 259;
	fseek(RES_FILE,0L,SEEK_END);
	RES_FILE_LEN = ftell(RES_FILE)/RES_REC_LEN;
	//RES_FILE_LEN = 76;

	fclose(RES_FILE);
	return 1;
}


void RESOURCE_Uninit(void)
{
	free(RES_FILE_NAME);
}


long RESOURCE_GetPos(unsigned int NO)
{
 long pos=0;
 long lastpos=0;
 long l,r;


	 if (RES_FILE_LEN==0)
	 {
		return -1;
	 }

	 l=1;
	 r=RES_FILE_LEN+1;

	 while(1)
	 {
		pos=(l+r)/2;
		if(lastpos==pos) break; //Yoksa
		fseek(RES_FILE,(pos-1)*RES_REC_LEN,SEEK_SET);

		memset(&RES_REC,0x00,RES_REC_LEN);
		fread(&RES_REC,RES_REC_LEN,1,RES_FILE);


		//printf("%d\n",pos);
		if (RES_REC.NO==NO) break;
		if (RES_REC.NO<NO) //ILERLE
		{
			l=pos;
		}
		if (RES_REC.NO>NO) //GERILE
		{
			r=pos;
		}
		lastpos=pos;
	 }

	 if (RES_REC.NO==NO)
	 {
			return pos;
	 }
	 return -1;
}


void RESOURCE_OpenFile(void)
{
	RES_FILE=fopen(RES_FILE_NAME,"rb");
}

void RESOURCE_CloseFile(void)
{
	fclose(RES_FILE);
}

char *RESOURCE_ReadLineOpen(unsigned int NO)
{
	long pos;
	pos=RESOURCE_GetPos(NO);
	if (pos>-1)
	{
		return RES_REC.LINE;
	}
	return NULL;
}

char *RESOURCE_ReadLine(unsigned int NO)
{
	long pos;
	RES_FILE=fopen(RES_FILE_NAME,"rb");

	pos=RESOURCE_GetPos(NO);
	fclose(RES_FILE);
	if (pos>-1)
	{
		return RES_REC.LINE;
	}
	return NULL;
}
