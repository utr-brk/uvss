/*
 * gunisigi.c
 *
 *  Created on: 10 Haz 2014
 *      Author: Burak
 */


//#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
//#include "posapi.h"
#include "gunisigi.h"

#define FILE_DST			"GUNISIGI.DST"
#define DST_REC_LEN 	18

//char GUNISIGI_Kontrol(struct dosdate_t *d,struct dostime_t *t)
char GUNISIGI_Kontrol(struct tm *timeinfo)
{
    char ret = 0x00;
    unsigned char f_pos;
    unsigned char f_len;
    unsigned int  cYear;
    unsigned char cMonth;
    unsigned char cDay;
    unsigned char cHour;
    unsigned char cNewHour;

    char *dstLine;
    char *dstPart;
    FILE *dst;
    time_t rawtime;

    dstLine=(char *) calloc (DST_REC_LEN+1, 1);//0.0.6 malloc'lar calloc oldu
    dstPart=(char *) calloc (5, 1);

    dst = fopen(FILE_DST,"r+b");

    if (dst!=NULL)
    {
        fseek(dst,0L,SEEK_END);
        f_len = ftell(dst) / DST_REC_LEN;
        for (f_pos=0; f_pos<f_len; f_pos++)
        {
            fseek(dst,f_pos*DST_REC_LEN,SEEK_SET);
            memset(dstLine,0x00,DST_REC_LEN+1);
            fread(dstLine,DST_REC_LEN,1,dst);
            if (dstLine[15]=='0') //Islenmemis
            {
                memset(dstPart,0x00,5);
                memcpy(dstPart,dstLine,4);
                cYear = atoi(dstPart);
                //if (cYear==d->year)
                if (cYear==timeinfo->tm_year)
                {
                    memset(dstPart,0x00,5);
                    memcpy(dstPart,dstLine+4,2);
                    cMonth = atoi(dstPart);
                    //if (cMonth==d->month)
                    if (cMonth==timeinfo->tm_mon)
                    {
                        memset(dstPart,0x00,5);
                        memcpy(dstPart,dstLine+6,2);
                        cDay = atoi(dstPart);
                        //if (cDay==d->day)
                        if (cDay==timeinfo->tm_mday)
                        {
                            memset(dstPart,0x00,5);
                            memcpy(dstPart,dstLine+9,2);
                            cHour = atoi(dstPart);
                            //if (cHour==t->hour)
                            if (cHour==timeinfo->tm_hour)
                            {
                                memset(dstPart,0x00,5);
                                memcpy(dstPart,dstLine+12,2);
                                cNewHour = atoi(dstPart);
                                dstLine[15]='1'; //Islendi Olarak Isaretle
                                fseek(dst,f_pos*DST_REC_LEN,SEEK_SET);
                                fwrite(dstLine,DST_REC_LEN,1,dst);

                                //_dos_gettime(t);//Son Saniye/Milisaniyeyi Al
                                //t->hour=cNewHour;
                                //_dos_settime(t);

                                time(&rawtime);
                                timeinfo = localtime(&rawtime);
                                timeinfo->tm_hour = cNewHour;
                                //odroid
                                //sys_set_time(timeinfo);
                                //stime(timeinfo);
                                printf("burada zaman set edilecek");
                                ret=0x01;
                                break;
                            }
                        }
                    }
                    //} else if (cYear>d->year) //Yil Buyudu ise cik
                }
                else if (cYear>timeinfo->tm_year)   //Yil Buyudu ise cik
                {
                    break;
                }
            }
        }
        fclose(dst);
    }
    free(dstLine);
    free(dstPart);
    return ret;
}
