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

#include "watchdt.h"
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



void UVSS_Karsila(void)
{

}
