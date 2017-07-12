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

#include <sql.h>
#include <sqlext.h>

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

char REF_IMAGE_FILE[32];
char LP_FILE_NAME[32];
char UVSS_FILE_NAME[32];

/*
 * see Retrieving ODBC Diagnostics
 * for a definition of extract_error().
 */
static void extract_error(char *fn, SQLHANDLE handle, SQLSMALLINT type);

void extract_error(char *fn, SQLHANDLE handle, SQLSMALLINT type)
{
SQLINTEGER  i = 0;
SQLINTEGER  native;
SQLCHAR	    state[7];
SQLCHAR	    text[256];
SQLSMALLINT len;
SQLRETURN   ret;

    fprintf(stderr,"\nThe driver reported the following diagnostics whilst running %s\n\n", fn);

    do{
        ret = SQLGetDiagRec(type, handle, ++i, state, &native, text, sizeof(text), &len );
        if(SQL_SUCCEEDED(ret)) printf("%s:%ld:%ld:%s\n", state, i, native, text);
    }while(ret == SQL_SUCCESS);
}

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
	//return retval;
	return 1;
}

int UVSS_Send(int msg_type)
{
	int ref_file_len;
	int uvss_socket;
	//fd_set readset;
	//int result, iof = -1;
	//struct timeval tv;
	char *szBuf;

	int retval = 1;
	szBuf = (char *) calloc(64, 1);

	switch(msg_type)
	{
	case CAR_DETECTED:
	case CAR_FINISHED:
		sprintf(szBuf, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", 0x1, msg_type,
		        0xff, 0xff, 0xff, 0xff, 0x0, 0x0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
		uvss_socket = TCP_Connect(rec_TERM.IP_SERVER, rec_TERM.PORT_SERVER);
		if (uvss_socket>0)
		{
			write(uvss_socket, szBuf, 16);

			close(uvss_socket);
			uvss_socket = -1;
		}
		else
			retval = -1;//sunucu yok

		break;
	/*case CAR_FINISHED:
		sprintf(szBuf, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", 0x1, CAR_FINISHED,
		        0xff, 0xff, 0xff, 0xff, 0x0, 0x0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
		uvss_socket = TCP_Connect(rec_TERM.IP_SERVER, rec_TERM.PORT_SERVER);
		if (uvss_socket>0)
		{
			write(uvss_socket, szBuf, strlen((char *)szBuf));

			close(uvss_socket);
			uvss_socket = -1;
		}
		else
			retval = -1;//sunucu yok

		break;*/
	case REF_IMAGE:
		sprintf(REF_IMAGE_FILE, "%s", "PTSG_06fd2486_20171107140523");
		ref_file_len = strlen(REF_IMAGE_FILE);
		sprintf(szBuf, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%d%s", 0x1, REF_IMAGE,
		        0xff, 0xff, 0xff, 0xff, 0x0, 0x0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, ref_file_len, REF_IMAGE_FILE);
		uvss_socket = TCP_Connect(rec_TERM.IP_SERVER, rec_TERM.PORT_SERVER);
		if (uvss_socket>0)
		{
			write(uvss_socket, szBuf, 20+strlen((char *)REF_IMAGE_FILE));

			close(uvss_socket);
			uvss_socket = -1;
		}
		else
			retval = -1;//sunucu yok

		break;
	}

	free(szBuf);
	//return retval;
	return 1;

}

int UVSS_Read(int msg_type)
{
	int rcv_file_len = 0;
	int uvss_socket;
	fd_set readset;
	int result, iof = -1;
	struct timeval tv;
	char *strRcv;
	//char *szBuf;

	char rcv_file[32];

	int retval = 1;
	//szBuf = (char *) calloc(16, 1);
	strRcv = ( char *) calloc (2049, 1);

	uvss_socket = TCP_Connect(rec_TERM.IP_SERVER, rec_TERM.PORT_SERVER);
	if (uvss_socket>0)
	{
		// Initialize the set
		FD_ZERO(&readset);
		FD_SET(uvss_socket, &readset);

		// Initialize time out struct

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
			retval = 1;
		}

		if (retval==1)
		{
			retval=1;
			//Length is in result
			//L = result;
			memcpy(&rcv_file_len, strRcv+16, 4);
			if (rcv_file_len > 0)
			{
				memcpy(rcv_file, strRcv+20, rcv_file_len);
				retval=1;//izin var

				//MSJ_Ses(Ses);
			}
			else
				retval = -1;
		}
		close(uvss_socket);
		uvss_socket = -1;
	}
	else
		retval = -1;

	if(retval == 1)
	{
		switch(msg_type)
		{
		case LP_IMAGE:
			memcpy(LP_FILE_NAME, rcv_file, rcv_file_len);
			break;
		case UVSS_IMAGE:
			memcpy(UVSS_FILE_NAME, rcv_file, rcv_file_len);
			break;
		}
	}
	//free(szBuf);
	free(strRcv);
	//return retval;
	return 1;
}
/*
int SP_UVSS(int sp_type)
{
    int retval = -1;
	SQLHENV h_env;
	SQLHDBC h_dbc;
	SQLHSTMT h_sp_stmt;
	SQLRETURN ret; //ODBC API return status
	SQLCHAR outstr[1024];
	SQLSMALLINT outstrlen;
//SQLSMALLINT columns;
//int row = 0;
	//SQLCHAR sqlstmt[] = "SELECT SICILNO, ADISOYADI, BOLUMADI FROM PRO01_SBT.DBO.SICIL WHERE KARTNO ='3d4f0435'";
//SQLCHAR sp_stmt[] = "EXEC PRO01_SBT.DBO.SP_ODROID '12345678','01', '0','2017-04-10 16:50:00', ?, ''";
	//SQLCHAR sp_stmt[] = "EXEC PRO01_SBT.DBO.SPO ?, ?";

	SQLCHAR scl[50];
	SQLCHAR ad[50];
	SQLCHAR blm[50];
//SQLCHAR line1[50];
//SQLCHAR line2[50];
//SQLCHAR ads[20];
	SQLINTEGER qq, sp_ret, sp_rtn, qw;
	SQLINTEGER cbRtn;
	SQLCHAR kart_no[20], sonuc[50];
	SQLCHAR kodu[3];
	SQLCHAR gc[1];
	SQLDATE zmn;
	SQLCHAR line1[200];
	SQLCHAR line2[200];
	SQLINTEGER l1, l2, l3;
	SQLSMALLINT columns;

	// Allocate an environment handle
	SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &h_env);
	// We want ODBC 3 support
	SQLSetEnvAttr(h_env, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);
	// Allocate a connection handle
	SQLAllocHandle(SQL_HANDLE_DBC, h_env, &h_dbc);
	// Connect to the DSN mydsn
	ret = SQLDriverConnect(h_dbc, NULL, (SQLCHAR *)"DSN=MSSQL;UID=UTARIT;PWD=4747;",
	                       SQL_NTS, outstr, sizeof(outstr), &outstrlen, SQL_DRIVER_COMPLETE);
	if(SQL_SUCCEEDED(ret))
	{
		printf("Connected\n");
		printf("Returned connection string was:\n\t%s\n", outstr);
		if (ret == SQL_SUCCESS_WITH_INFO)
		{
			printf("Driver reported the following diagnostics\n");
			extract_error("SQLDriverConnect", h_dbc, SQL_HANDLE_DBC);
		}



		SQLAllocHandle(SQL_HANDLE_STMT, h_dbc, &h_sp_stmt);

		sp_ret = SQLBindParameter(h_sp_stmt, 1, SQL_PARAM_OUTPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &sp_rtn, 0, &cbRtn);
		sp_ret = SQLBindParameter(h_sp_stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 11, 0,kart_no, 11, NULL);
		sp_ret = SQLBindParameter(h_sp_stmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 30, 0,ad, 30, NULL);
		sp_ret = SQLBindParameter(h_sp_stmt, 4, SQL_PARAM_OUTPUT, SQL_C_SLONG, SQL_INTEGER, 4, 0, &qw, 0, 0);




		strcpy(kart_no, "474747474747");
		strcpy(ad, "DORUK DEFNE");

		sp_ret = SQLExecDirect(h_sp_stmt, (SQLCHAR*)"{?=call PRO01_SBT.DBO.SPO_1(?,?,'')}", SQL_NTS);
		ret = SQLBindCol(h_sp_stmt, 1, SQL_C_CHAR, (SQLPOINTER)line1, 50, &l1);
		ret = SQLBindCol(h_sp_stmt, 2, SQL_C_CHAR, (SQLPOINTER)line2, 50, &l2);
		ret = SQLBindCol(h_sp_stmt, 3, SQL_C_CHAR, (SQLPOINTER)sonuc, 50, &l3);
		ret = SQLFetch(h_sp_stmt);
		sp_ret = SQLNumResultCols(h_sp_stmt, &columns);

		printf("L1: %s\nL2: %s\nRS: %s\n", line1, line2, sonuc);

    	if(sp_ret == SQL_ERROR)
			retval = -1;
        else
            retval = 1;


		// disconnect from driver
		SQLDisconnect(h_dbc);
	}
	else
	{
		fprintf(stderr, "Failed to connect\n");
		extract_error("SQLDriverConnect", h_dbc, SQL_HANDLE_DBC);
	}
	// free up allocated handles
	SQLFreeHandle(SQL_HANDLE_ENV, h_sp_stmt);
	// disconnect from driver
    SQLDisconnect(h_dbc);
	SQLFreeHandle(SQL_HANDLE_DBC, h_dbc);
	SQLFreeHandle(SQL_HANDLE_ENV, h_env);


	return 1;
}*/

int SP_UVSS(int sp_type)
{
    int retval = -1;
	SQLHENV h_env;
	SQLHDBC h_dbc;
	SQLHSTMT h_sp_stmt;
	SQLRETURN ret; //ODBC API return status
	SQLCHAR outstr[1024];
	SQLSMALLINT outstrlen;

	SQLINTEGER sp_ret, sp_rtn;
	SQLINTEGER cbRtn, islem;

	SQLCHAR sonuc[50], org_file_name[500], hgs_tag[50];
    SQLCHAR plaka_no[50],dosya_ismi[500],terminal_kodu[3],terminal_ip[15];

	SQLINTEGER l1, l2, l3;//length of the returns
	SQLSMALLINT columns;//number of the columns

	// Allocate an environment handle
	SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &h_env);
	// We want ODBC 3 support
	SQLSetEnvAttr(h_env, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);
	// Allocate a connection handle
	SQLAllocHandle(SQL_HANDLE_DBC, h_env, &h_dbc);
	// Connect to the DSN mydsn
	ret = SQLDriverConnect(h_dbc, NULL, (SQLCHAR *)"DSN=MSSQL;UID=UTARIT;PWD=4747;",
	                       SQL_NTS, outstr, sizeof(outstr), &outstrlen, SQL_DRIVER_COMPLETE);
	if(SQL_SUCCEEDED(ret))
	{
		/*
		printf("Connected\n");
		printf("Returned connection string was:\n\t%s\n", outstr);
		if (ret == SQL_SUCCESS_WITH_INFO)
		{
			printf("Driver reported the following diagnostics\n");
			extract_error("SQLDriverConnect", h_dbc, SQL_HANDLE_DBC);
		}
        */

		SQLAllocHandle(SQL_HANDLE_STMT, h_dbc, &h_sp_stmt);

        sp_ret = SQLBindParameter(h_sp_stmt, 1, SQL_PARAM_OUTPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &sp_rtn, 0, &cbRtn);
		sp_ret = SQLBindParameter(h_sp_stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 50, 0,plaka_no, 50, NULL);
		sp_ret = SQLBindParameter(h_sp_stmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 500, 0,dosya_ismi, 500, NULL);
		sp_ret = SQLBindParameter(h_sp_stmt, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 3, 0,terminal_kodu, 3, NULL);
		sp_ret = SQLBindParameter(h_sp_stmt, 5, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 15, 0,terminal_ip, 15, NULL);
		sp_ret = SQLBindParameter(h_sp_stmt, 6, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 4, 0, &islem, 0, 0);

		strcpy(plaka_no, "06EY4300");//comes from bora

		if(sp_type == 1)
            strcpy(dosya_ismi, "06EY4300_20170712115633.JPEG");//comes from bora
        else
            memset(dosya_ismi, 0, 500);

		strcpy(terminal_kodu, rec_TERM.KAPI_KOD);
		strcpy(terminal_ip, rec_TERM.IP_TERM);
		islem = sp_type;

        memset(sonuc, 0, 50);
        memset(org_file_name, 0, 500);
        memset(hgs_tag, 0, 50);


		sp_ret = SQLExecDirect(h_sp_stmt, (SQLCHAR*)"{?=call PRO01_SBT.DBO.SP_ARAC_ALTI_SISTEMI(?,?,?,?,?)}", SQL_NTS);

		ret = SQLBindCol(h_sp_stmt, 1, SQL_C_CHAR, (SQLPOINTER)sonuc, 50, &l1);
		ret = SQLBindCol(h_sp_stmt, 2, SQL_C_CHAR, (SQLPOINTER)org_file_name, 500, &l2);
		ret = SQLBindCol(h_sp_stmt, 3, SQL_C_CHAR, (SQLPOINTER)hgs_tag, 50, &l3);
		ret = SQLFetch(h_sp_stmt);
		sp_ret = SQLNumResultCols(h_sp_stmt, &columns);

		printf("\nislem: %d\norg_file_name: %s\nhgs_tag: %s\nsonuc: %s\n", sp_type, org_file_name, hgs_tag, sonuc);

		if(sp_ret == SQL_ERROR)
			retval = -1;
        else
            retval = 1;


		// disconnect from driver
		SQLDisconnect(h_dbc);
	}
	else
	{
		fprintf(stderr, "Failed to connect\n");
		extract_error("SQLDriverConnect", h_dbc, SQL_HANDLE_DBC);
	}
	// free up allocated handles
	SQLFreeHandle(SQL_HANDLE_ENV, h_sp_stmt);
	// disconnect from driver
    SQLDisconnect(h_dbc);
	SQLFreeHandle(SQL_HANDLE_DBC, h_dbc);
	SQLFreeHandle(SQL_HANDLE_ENV, h_env);


	return retval;
}

void UVSS_Karsila(void)
{


	switch (CAR_Ready)
	{
	case 0://not arrived yet
		if(UVSS_Vehicle_Check(IN))//check the entry loop
		{
			if(UVSS_Send(CAR_DETECTED) == 1)//notify Bora that there starts a car, what happens if it fails
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
					UVSS_Send(REF_IMAGE);//send the corresponding reference image file name, what happens if it fails?
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
