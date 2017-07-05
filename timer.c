/*
 * timer.c
 *
 *  Created on: 1 Tem 2014
 *      Author: Burak
 */


//#include <stdio.h>
//#include <dos.h>
//#include <alloc.h>
//#include <conio.h>
//#include <string.h>
#include <stdlib.h>
//#include <dir.h>
#include <sys/time.h>
#include "tool.h"

/*
unsigned long long Timer_Set(unsigned long MS)
{
    struct_date d;
    struct_time t;
    unsigned long long rt;

		get_date(&d);
		get_time(&t);

    rt = (unsigned long long) (t.ti_sec);
    rt+= (unsigned long long) (t.ti_min*60L);
    rt+= (unsigned long long) (t.ti_hour*60L*60L);
    rt+= (unsigned long long) (d.da_day * 24L * 60L * 60L);
    rt+= (unsigned long long) (d.da_mon * 30L * 24L * 60L * 60L);
    rt+= (unsigned long long) ((d.da_year-2000) * 365L * 24L * 60L * 60L);
    //Saniye Olarak Bulundu
    rt = rt * 1000L; //Ms Oldu
    rt+= (unsigned long long) (t.ti_hund*10L); //Zamandaki ms eklendi
    rt+= (unsigned long long) MS; //Ýstenen süre eklendi

    return rt;
}

char Timer_End(unsigned long long Timer)
{
    char r = 0;
    if ( Timer_Set(0) > Timer)
    {
        r = 1;
    }
    return r;
}
*/

//2.2.5
//struct timeval Set_Timer(struct timeval tSon)
struct timeval Set_Timer(int sec, long usec)
{
    struct timeval tNow, tEnd, tSon;
    //2.2.5
    tSon.tv_sec = sec;
    tSon.tv_usec = usec;

    gettimeofday(&tNow, NULL);
    timeradd(&tNow, &tSon, &tEnd);
    return tEnd;
}

int Check_Timer(struct timeval tEnd)
{
    struct timeval tNow;
    gettimeofday(&tNow, NULL);

    if(timercmp(&tNow, &tEnd, >))
        return 1;
    else
        return 0;
}
