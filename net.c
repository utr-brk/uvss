#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "config.h"

void NET_Init(void)
{
    char buf[100];

    memset(buf, '\0', sizeof(buf));
    //odroid
	//wireless icin eth0 değil, wlan0 olacak
	system("ifconfig eth0 down");
	sleep(1);

	strcpy(buf, "ifconfig eth0 ");
	strcat(buf, rec_TERM.IP_TERM);
	strcat(buf, " netmask ");
	strcat(buf, rec_TERM.IP_SUBNET);
	strcat(buf, " up");

	system(buf);
    sleep(1);

    memset(buf, '\0', sizeof(buf));
	strcpy(buf, "route add default gw ");
	strcat(buf, rec_TERM.IP_GATEWAY);

	system(buf);
    sleep(1);
}
