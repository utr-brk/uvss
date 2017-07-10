#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>

#include "tool.h"
#include "config.h"
#include "lcd.h"
#include "mesaj.h"
#include "log.h"
#include "tcp.h"

//0.0.1
//#include "watchdt.h"
#include "personel.h"
//#include "resource.h"
#include "reader.h"

#include "zamanis.h"
#include "sayac.h"
#include "reader.h"
#include "serport.h"

#include "smm.h"
#include "keyb.h"
#include "timer.h"

#include "string_convert.h"

//1.4.2
#include "yazici.h"

#include <wiringPi.h>

//2.0.0
#include "cn56b.h"

#include "uvss.h"

//2.1.0
#include "minini.h"
#define sizearray(a)	(sizeof(a) / sizeof((a)[0]))

//1.0.0
#define cmd_LOG_AL		42
#define cAyrac_1		0x01
#define cAyrac_2		2
#define cAyrac_Son		13
#define cmd_OK  		10
//2.1.0
#define cmd_CANCEL  	20
#define cmd_MSFR_ONAY  	25
#define cmd_HAREKET		50

#define cmd_IZIN		30

#define IN              1
#define OUT             !IN

#define CAR_DETECTED    1
#define CAR_FINISHED    2
#define LP_IMAGE        11
#define UVSS_IMAGE      12
#define REF_IMAGE       13

int CAR_Ready = 0;
int LP_Arrived = 0;
int UVSS_Ready = 0;
int VEHICLE_Ready = 0;

/*
IN_OUT parametresine göre giriş ya da çıkış loop'unda araç olup olmadığını söyler
*/
int UVSS_Vehicle_Check(int IN_OUT)
{
    int Relay[2] = {23, 13};
    int Sense[2] = {0, 2};
    int retval = -1;

    switch(IN_OUT)
    {
    case IN:
        if(digitalRead(Sense[rec_TERM.READERS[0].SENS-1])==rec_TERM.SENS_CEVAP)
        {
            retval = 1;
            digitalWrite(Relay[rec_TERM.READERS[0].ROLE -1], 1);//turn leds on
        }
        else
            retval = 0;
        break;
    case OUT:
        if(digitalRead(Sense[rec_TERM.READERS[1].SENS-1])==rec_TERM.SENS_CEVAP)
        {
            retval = 1;
            digitalWrite(Relay[rec_TERM.READERS[0].ROLE -1], 0);//turn leds off
        }
        else
            retval = 0;
        break;
    }
    return retval;
}

int UVSS_Send(int msg_type)
{
    int uvss_socket;
    fd_set readset;
    int result, iof = -1;
    struct timeval tv;
    char *strRcv;
    char *szBuf;

    int retval = 1;
    szBuf = (char *) calloc(16, 1);
    strRcv = ( char *) calloc (32, 1);

    switch(msg_type)
    {
    case CAR_DETECTED:
        sprintf(szBuf, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", 0x1, CAR_DETECTED,
                0xff, 0xff, 0xff, 0xff, 0x0, 0x0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
        uvss_socket = TCP_Connect(rec_TERM.IP_SERVER, rec_TERM.PORT_SERVER);
        if (uvss_socket>0)
        {
            write(uvss_socket, szBuf, strlen((char *)szBuf));
    /*
            // Initialize the set
            FD_ZERO(&readset);
            FD_SET(uvss_socket, &readset);

            // Initialize time out struct
            //2.4.0
            //tv.tv_sec = 0;
            //tv.tv_usec = 2000 * 1000;
            tv.tv_sec = KAPI_TCP_WAIT_TIME;
            tv.tv_usec = 0;
            // select()
            result = select(uvss_socket+1, &readset, NULL, NULL, &tv);

            // Check status
            //1.7.0
            //sunucu bağlantısı var ama timeout'a düşüp cevap dönmediyse offline izin verildi dön
            //if (result < 0){
            //  retval = 0x1;
            if (result < 1)
            {
                //debug_print("tcp_error");
                retval = -1;
            }
            else if (result > 0 && FD_ISSET(uvss_socket, &readset))
            {
                // Set non-blocking mode
                if ((iof = fcntl(uvss_socket, F_GETFL, 0)) != -1)
                    fcntl(uvss_socket, F_SETFL, iof | O_NONBLOCK);
                // receive
                result = recv(uvss_socket, strRcv, 2048, 0);
                // set as before
                if (iof != -1)
                    fcntl(uvss_socket, F_SETFL, iof);
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

                    //MSJ_Ses(Ses);
                }
                else if (strRcv[0]==cmd_MSFR_ONAY)
                {

                    while(gtk_events_pending())
                        gtk_main_iteration();

                    //MSJ_Ses(Ses);

                }
                else if (strRcv[0]==cmd_CANCEL)
                {

                    while(gtk_events_pending())
                        gtk_main_iteration();
                    //MSJ_Ses(Ses);
                }


                while(gtk_events_pending())
                    gtk_main_iteration();
            }
*/
            close(uvss_socket);
            uvss_socket = -1;
        }
        else
            retval = -1;//sunucu yok


        break;
    case CAR_FINISHED:

        break;
    case REF_IMAGE:

        break;
    }

    free(szBuf);
    free(strRcv);
    return retval;

}

int UVSS_Read(int msg_type)
{

    return 1;
}

int SP_UVSS(int sp_type)
{
    return 1;
}


void UVSS_Karsila(void)
{


    switch (CAR_Ready)
    {
    case 0://not arrived yet
        if(UVSS_Vehicle_Check(IN))//check the entry loop
        {
            UVSS_Send(CAR_DETECTED);//notify Bora that there starts a car
            CAR_Ready = 1;
        }
        break;
    case 1://car is on, wait for LP and check if it is over
        if(!LP_Arrived)//if lp not read yet
        {
            if(UVSS_Read(LP_IMAGE))//try to read the lp image file name
            {
                LP_Arrived = 1;
                if(SP_UVSS(1)) //call the stored procedure to get the ref image file name (if exists)
                {
                    UVSS_Send(REF_IMAGE);//send the corresponding reference image file name
                }
            }
        }
        else
        {
            if(!VEHICLE_Ready)
            {
                if(UVSS_Vehicle_Check(OUT))//check the exit loop
                {
                    UVSS_Send(CAR_FINISHED);//notify that car finished
                    VEHICLE_Ready = 1;//now car is ready, wait for the image files
                }
            }
            else
            {
                if(UVSS_Read(UVSS_IMAGE))//image files are ready
                {
                    SP_UVSS(2);//call the stored procedure to notify that the files are ready
                    VEHICLE_Ready = 0;
                    LP_Arrived = 0;
                    CAR_Ready = 0;//start over for the next car
                }
            }
        }
        break;
    }

}
