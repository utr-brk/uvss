#include <ncurses.h>
#include <menu.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include "lcd.h"
#include "keyb.h"
#include "config.h"
#include "mesaj.h"
#include "personel.h"

//1.4.1
//#include "resource.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD   4

char sifre[5] = {0x00,0x00,0x00,0x00,0x00};
char new_sifre[5] = {0x00,0x00,0x00,0x00,0x00};
int aPos;
char aPass[5] = {0x00,0x00,0x00,0x00,0x00};

char *admin_menu[] = {
		"NETWORK AYARLARI",
		"TARİH/SAAT",
		"ŞİFRE DEĞİŞTİR",
		"ÇIKIŞ",
};

char *tcpip_menu[] = {
		"LOCAL IP",
		"NETMASK",
		"GATEWAY",
		"SERVER IP",
		"SERVER PORT",
		"ÇIKIŞ",
};
int ADMIN_ShowMenu(char* Cap,...)
{
	int i,key;
	va_list ap;
	char *arg;
	arg=(char*) malloc(25);
	lcd_clean();
	//0.53
	gtk_clean();
	//gtk_layout_move(GTK_LAYOUT(main_layout), logo, 50, 5);
	gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);

	va_start(ap, Cap);
	lcd_printf_ex(ALG_CENTER, 0,Cap);
	//0.61
	gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
	//0.53
	gtk_printf(label1, "%s", Cap);
	for (i=0;i<6;i++)
	{
		memcpy(arg,va_arg(ap,char*),25);
		lcd_printf_ex(ALG_CENTER, i+1,arg);
		lcd_flip();
		//0.53
		if(strlen(arg))
            gtk_printf(label1, "%s\n%s", gtk_label_get_label(GTK_LABEL(label1)), arg);
	}
	va_end(ap);
	//cd_printf_ex(ALG_CENTER, 7, RESOURCE_ReadLine(1999));

	//0.53
	gtk_printf(label1, "%s\nÇIKMAK İÇİN [BKSP]", gtk_label_get_label(GTK_LABEL(label1)));
    while(gtk_events_pending())
        gtk_main_iteration();
    napms(10);
    lcd_printf_ex(ALG_CENTER, 7, "CIKMAK ICIN [BKSP]");
	lcd_flip();
	key=KEYB_getch(0);
	free(arg);
	return key;
}

/*
//0.43
void ADMIN_TcpIP(void)
{
	char *IP_TEMP;
	int key, row_, col_;
	//char *m[6];
	IP_TEMP = (char*) malloc(20);
	//for (i=0;i<6;i++)
	//{
	//		m[i]=(char*) malloc(256);
	//		sprintf(m[i],"%s",RESOURCE_ReadLine(1200+i));
	//}
	getmaxyx(stdscr, row_, col_); //get screen size
	while(key!=KEY_BKSP)
	{
		key = ADMIN_ShowMenu("TCP/IP AYARLARI",//m[0],//"TCP/IP AYARLARI",
                             "1-LOCAL IP",//m[1],//"1-LOCAL IP",
                             "2-NETMASK",//m[2],//"2-NETMASK",
                             "3-GATEWAY",//m[3],//"3-GATEWAY",
                             "4-SERVER IP",//m[4],//"4-SERVER IP",
                             "5-SERVER PORT",//m[5],//"5-SERVER PORT",
                             "");//"");
		switch (key)
		{
			case 1:
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 0,"LOCAL IP");//RESOURCE_ReadLine(1221));//"LOCAL IP");
				lcd_printf_ex(ALG_CENTER, 2,rec_TERM.IP_TERM);
				lcd_printf_ex(ALG_CENTER, 4,"YENI DEGER");//RESOURCE_ReadLine(1210));//"YENI DEGER:");
				lcd_flip();
				memset(IP_TEMP,0x00,sizeof(IP_TEMP));
				KEYB_inputip(IP_TEMP,5);
				sprintf(rec_TERM.IP_TERM,"%s",IP_TEMP);
				lcd_printf_ex(ALG_CENTER, 7,"DEGER KAYIT EDILDI");//RESOURCE_ReadLine(1211));//"DEGER KAYIT EDILDI");
				lcd_flip();
				napms(1000);
				break;
			case 2:
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 0,"NETMASK");//RESOURCE_ReadLine(1222));//"NETMASK");
				lcd_printf_ex(ALG_CENTER, 2,rec_TERM.IP_SUBNET);
				lcd_printf_ex(ALG_CENTER, 4,"YENI DEGER");//RESOURCE_ReadLine(1210));//"YENI DEGER:");
				lcd_flip();
				memset(IP_TEMP,0x00,sizeof(IP_TEMP));
				KEYB_inputip(IP_TEMP,5);
				sprintf(rec_TERM.IP_SUBNET,"%s",IP_TEMP);
				lcd_printf_ex(ALG_CENTER, 7,"DEGER KAYIT EDILDI");//RESOURCE_ReadLine(1211));//"DEGER KAYIT EDILDI");
				lcd_flip();
				napms(1000);
				break;
			case 3:
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 0,"GATEWAY");//RESOURCE_ReadLine(1223));//"GATEWAY");
				lcd_printf_ex(ALG_CENTER, 2,rec_TERM.IP_GATEWAY);
				lcd_printf_ex(ALG_CENTER, 4,"YENI DEGER");//RESOURCE_ReadLine(1210));//"YENI DEGER:");
				lcd_flip();
				memset(IP_TEMP,0x00,sizeof(IP_TEMP));
				KEYB_inputip(IP_TEMP,5);
				sprintf(rec_TERM.IP_GATEWAY,"%s",IP_TEMP);
				lcd_printf_ex(ALG_CENTER, 7,"DEGER KAYIT EDILDI");//RESOURCE_ReadLine(1211));//"DEGER KAYIT EDILDI");
				lcd_flip();
				napms(1000);
				break;
			case 4:
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 0,"SERVER IP");//RESOURCE_ReadLine(1224));//"SERVER IP");
				lcd_printf_ex(ALG_CENTER, 2,rec_TERM.IP_SERVER);
				lcd_printf_ex(ALG_CENTER, 4,"YENI DEGER");//RESOURCE_ReadLine(1210));//"YENI DEGER:");
				lcd_flip();
				memset(IP_TEMP,0x00,sizeof(IP_TEMP));
				KEYB_inputip(IP_TEMP,5);
				sprintf(rec_TERM.IP_SERVER,"%s",IP_TEMP);
				lcd_printf_ex(ALG_CENTER, 7,"DEGER KAYIT EDILDI");//RESOURCE_ReadLine(1211));//"DEGER KAYIT EDILDI");
				lcd_flip();
				napms(1000);
				break;
			case 5:
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 0,"SERVER PORT");//RESOURCE_ReadLine(1225));//"SERVER PORT");
				sprintf(IP_TEMP,"%d",rec_TERM.PORT_SERVER);
				lcd_printf_ex(ALG_CENTER, 2,IP_TEMP);
				lcd_printf_ex(ALG_CENTER, 4,"YENI DEGER");//RESOURCE_ReadLine(1210));//"YENI DEGER:");
				lcd_flip();
				KEYB_inputnumeric(IP_TEMP,5,(row_ /2 -6),4);//odroid 5,0 idi
				rec_TERM.PORT_SERVER=atoi(IP_TEMP);
				lcd_printf_ex(ALG_CENTER, 7,"DEGER KAYIT EDILDI");//RESOURCE_ReadLine(1211));//"DEGER KAYIT EDILDI");
				lcd_flip();
				napms(1000);
				break;
		}
	}
	free(IP_TEMP);
	//for (i=0;i<6;i++)
    //   free(m[i]);
}
*/
/*
//0.43
void ADMIN_TarihSaat(void)
{
	//struct_date d;
//	struct_time t;
	//struct_date d1;
//	struct_time t1;
	unsigned char *cDateTime;
	unsigned char *sz;
	char Gunler[12]={31,28,31,30,31,30,31,31,30,31,30,31};
struct tm * d;
//struct tm * d1;
time_t rawtime;//0.1.0
int row_, col_;

	cDateTime = (unsigned char*) malloc(20);
	sz = (unsigned char*) malloc(5);

	lcd_clean();
	lcd_printf_ex(ALG_CENTER, 0,"TARIH SAAT AYARI");//RESOURCE_ReadLine(1100)); //TARIH SAAT AYARI
	lcd_flip();
	getmaxyx(stdscr, row_, col_); //get screen size

	while(1)
	{
	    time(&rawtime);
        d = localtime(&rawtime);

		sprintf(cDateTime,"%02d/%02d/%04d %02d:%02d"//RESOURCE_ReadLine(1101) // "%04d/%02d/%02d %02d:%02d"
										 ,d->tm_mday,d->tm_mon+1,d->tm_year+1900,d->tm_hour,d->tm_min);
		lcd_printf_ex(ALG_CENTER, 2,cDateTime);
		lcd_flip();

        sprintf(sz,"%02d",d->tm_mday);
		KEYB_inputnumeric(sz,4,0,2);
		d->tm_mday=atoi(sz);
		mvprintw(4, (col_ - 15)/2+2, "/");
		lcd_flip();

		sprintf(sz,"%02d",d->tm_mon+1);
		KEYB_inputnumeric(sz,4,3,2);
		d->tm_mon=atoi(sz)-1;
		mvprintw(4, (col_ - 15)/2+5, "/");
		lcd_flip();

        sprintf(sz,"%04d",d->tm_year+1900);
		KEYB_inputnumeric(sz,4,6,4);
		d->tm_year=atoi(sz)-1900;


		sprintf(sz,"%02d",d->tm_hour);
		KEYB_inputnumeric(sz,4,11,2);
		d->tm_hour=atoi(sz);
		mvprintw(4, (col_ - 15)/2+13, ":");
		lcd_flip();

		sprintf(sz,"%02d",d->tm_min);
		KEYB_inputnumeric(sz,4,14,2);
		d->tm_min=atoi(sz);
		if (((d->tm_year+1900)%4)==0) Gunler[1] = 29; //Leap year
		if (((d->tm_year+1900)>3000)||((d->tm_year+1900)<2000)) continue;
		if ((d->tm_mon+1<1)||(d->tm_mon>11)) continue;
		if ((d->tm_mday<1)||(d->tm_mday>Gunler[d->tm_mon])) continue;


		rawtime = mktime ( d );
        if(!stime(&rawtime)){
            lcd_clean();
            lcd_printf_ex(ALG_CENTER, 7,"SAAT KAYDEDİLDİ");
            lcd_flip();
            MSJ_Ses("131313");
            break;
        }else{
            lcd_clean();
            lcd_printf_ex(ALG_CENTER, 7,"SAAT KAYDEDİLEMEDİ");
            lcd_flip();
            MSJ_Ses("1212");
        }
        //time(&rawtime);
        //d1 = localtime(&rawtime);
		//getdate(&d1);
		//gettime(&t1);
		//if  ((d->tm_hour==d1->tm_hour)&&
		//		 (d->tm_min==d1->tm_min)&&
		//		 (d->tm_year==d1->tm_year)&&
		//		 (d->tm_mon==d1->tm_mon)&&
		//		 (d->tm_mday==d1->tm_mday)) break;

	}
	free(cDateTime);
	free(sz);
}
*/
/*
//0.43
void ADMIN_SifreDegis(void)
{
	unsigned char *sz;
	int row_, col_;
	sz=(unsigned char*) malloc(10);

    getmaxyx(stdscr, row_, col_); //get screen size

	lcd_clean();
	lcd_printf_ex(ALG_CENTER, 0,"SIFRE DEGISTIR");//RESOURCE_ReadLine(1300)); //SIFRE DEGISTIR
	lcd_printf_ex(ALG_CENTER, 3,"YENI SIFRE");//RESOURCE_ReadLine(1210));//"YENI DEGER:");
	lcd_flip();
	memset(sz,0x00,10);
	KEYB_inputnumeric(sz,4,(row_)/2-6,4);
	if (strlen(sz)==4)
	{
		memcpy(rec_TERM.ADMIN_PASS,sz,4);
		lcd_printf_ex(ALG_CENTER, 7,"SIFRE KAYIT EDILDI");//RESOURCE_ReadLine(1211));//"DEGER KAYIT EDILDI");
		lcd_flip();
		napms(1000);
	} else {
		lcd_printf_ex(ALG_CENTER, 7,"SIFRE HATALI");//RESOURCE_ReadLine(1212));//"DEGER HATALI");
		lcd_flip();
		napms(1000);
	}
	free(sz);
}
*/

//0.44
void ADMIN_SifreDegis(void)
{
	int key, i;
	lcd_clean();
	lcd_printf_ex(ALG_CENTER, 0,"SİFRE DEGİSTİR");
	lcd_printf_ex(ALG_CENTER, 1, "YENİ SİFRE");
	lcd_flip();
    //0.53
    gtk_clean();
    //0.61
    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
    gtk_printf(label1, "ŞİFRE DEĞİŞTİR\nYENİ ŞİFRE\n%s", new_sifre);
    while(gtk_events_pending())
        gtk_main_iteration();
	i = 0;
	key = -1;
	while((key != 0xF8) && (key != KEY_ENTR)){
		//if(kb_hit()){

			key = KEYB_getch(0);
			if(key == KEY_BKSP){
				if(i>0) i--;
				new_sifre[i] = 0;
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 0,"SİFRE DEGİSTİR");
				lcd_printf_ex(ALG_CENTER, 1, "YENİ SİFRE");
				lcd_printf_ex(ALG_CENTER, 3,  new_sifre);
				lcd_flip();
				//0.53
                gtk_clean();
                //0.61
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
				gtk_printf(label1, "ŞİFRE DEĞİŞTİR\nYENİ ŞİFRE\n%s", new_sifre);
				while(gtk_events_pending())
                    gtk_main_iteration();
			}
			else
				if(key <= 9 && key >= 0 && i<4){
					new_sifre[i++] = (char) key+48;
					lcd_clean();
					lcd_printf_ex(ALG_CENTER, 0,"SİFRE DEGİSTİR");
					lcd_printf_ex(ALG_CENTER, 1, "YENİ SİFRE");
					lcd_printf_ex(ALG_CENTER, 3,  new_sifre);
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "ŞİFRE DEĞİŞTİR\nYENİ ŞİFRE\n%s", new_sifre);
                    while(gtk_events_pending())
                        gtk_main_iteration();
				}
		//}

	}
	if(key == 0xF8){
		lcd_clean();
		lcd_printf_ex(ALG_CENTER, 7, "İPTAL EDİLDİ");
		lcd_flip();
		//0.53
        gtk_clean();
        //0.61
        gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
        gtk_printf(label1, "\n\nİPTAL EDİLDİ");
        while(gtk_events_pending())
            gtk_main_iteration();
		return;
	}

	if (strlen(new_sifre)!=4)
	{
		lcd_printf_ex(ALG_CENTER, 7, "DEĞER HATALI");
		lcd_flip();
		//0.53
        gtk_clean();
        //0.61
        gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
        gtk_printf(label1, "\n\nDEĞER HATALI");
        while(gtk_events_pending())
            gtk_main_iteration();
		napms(1000);
		//0.54
		return;
	}

	lcd_clean();
	lcd_printf_ex(ALG_CENTER, 0, "YENİ SİFRE");
	lcd_printf_ex(ALG_CENTER, 3, new_sifre);
	lcd_printf_ex(ALG_CENTER, 5, "OLARAK KAYDEDİLECEK");
	lcd_printf_ex(ALG_CENTER, 6, "ONAYLIYOR MUSUNUZ?");
	lcd_printf_ex(ALG_CENTER, 7, "BKSP: İPTAL       ENT: KAYDET");
	lcd_flip();
    //0.53
    gtk_clean();
    //0.61
    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
    gtk_printf(label1, "YENİ ŞİFRE\n\n%s\n\nOLARAK KAYDEDİLECEK\nONAYLIYOR MUSUNUZ?\n\nBKSP: İPTAL       ENT: KAYDET", new_sifre);
    while(gtk_events_pending())
        gtk_main_iteration();
	key = -1;
	while((key != KEY_BKSP) && (key != KEY_ENTR)){
		//if(kb_hit()){

			key = KEYB_getch(0);
			if(key == KEY_BKSP){
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 7,"SİFRE KAYDEDİLMEDİ");
				lcd_flip();
				//0.53
                gtk_clean();
                //0.61
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                gtk_printf(label1, "\n\nŞİFRE KAYDEDİLMEDİ");
                while(gtk_events_pending())
                    gtk_main_iteration();
				MSJ_Ses("1212");
				napms(1000);
			}else
				if(key == KEY_ENTR){
					memcpy(rec_TERM.ADMIN_PASS,new_sifre,strlen(new_sifre));
					lcd_clean();
					lcd_printf_ex(ALG_CENTER, 7,"SİFRE KAYDEDİLDİ");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "\n\nŞİFRE KAYDEDİLDİ");
                    while(gtk_events_pending())
                        gtk_main_iteration();
					MSJ_Ses("1313131");
					napms(1000);
				}
		//}
	}
}

//0.44
void ADMIN_TcpIP(void)
{
int key, i, j;

char ipp[4], newport[6], oldport[6];
const char delimiters[] = ".";
char *token, *cp;
char new_ip[16], ip_control[16];
int octet = -1;
unsigned long new_port;

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    while(key!=KEY_BKSP)
    {
        key = ADMIN_ShowMenu("TCP/IP AYARLARI",//m[0],//"TCP/IP AYARLARI",
                             "1-LOCAL IP",//m[1],//"1-LOCAL IP",
                             "2-NETMASK",//m[2],//"2-NETMASK",
                             "3-GATEWAY",//m[3],//"3-GATEWAY",
                             "4-SERVER IP",//m[4],//"4-SERVER IP",
                             "5-SERVER PORT",//m[5],//"5-SERVER PORT",
                             "");//"");
        switch (key){
			case 1: //Local IP

				cp = strdup(rec_TERM.IP_TERM);   //Make writable copy.
				token = strtok (cp, delimiters);   //ilk oktet
				sprintf(ipp, "%03d", (int) strtol(token, NULL, 10));
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 0, "LOCAL IP");
				lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_TERM);
				lcd_printf_ex(ALG_CENTER, 2, "YENİ IP");
				lcd_printf_ex(ALG_CENTER, 3, ipp);
				lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
				lcd_flip();
				//0.53
                gtk_clean();
                //0.61
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                gtk_printf(label1, "LOCAL IP\n%s\nYENİ IP\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_TERM, ipp);
                while(gtk_events_pending())
                    gtk_main_iteration();
				i = 3;
				key = -1;
				while((key != 0xF8) && ((key != KEY_ENTR) ||(i==0))){
					//if(kbhit()){

						//key = kb_getkey();
						key =KEYB_getch(0);
						if(key == KEY_BKSP){
							if(i>0) i--;
							ipp[i] = 0;
							lcd_clean();
							lcd_printf_ex(ALG_CENTER, 0, "LOCAL IP");
							lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_TERM);
							lcd_printf_ex(ALG_CENTER, 2, "YENİ IP");
							lcd_printf_ex(ALG_CENTER, 3, ipp);
							lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
							lcd_flip();
							//0.53
                            gtk_clean();
                            //0.61
                            gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                            gtk_printf(label1, "LOCAL IP\n%s\nYENİ IP\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_TERM, ipp);
                            while(gtk_events_pending())
                                gtk_main_iteration();
						}
						else
							if(key <= 9 && key >= 0 && i<3){
								ipp[i++] = (char) key+48;
								lcd_clean();
								lcd_clean();
								lcd_printf_ex(ALG_CENTER, 0, "LOCAL IP");
								lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_TERM);
								lcd_printf_ex(ALG_CENTER, 2, "YENİ IP");
								lcd_printf_ex(ALG_CENTER, 3, ipp);
								lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
								lcd_flip();
								//0.53
                                gtk_clean();
                                //0.61
                                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                                gtk_printf(label1, "LOCAL IP\n%s\nYENİ IP\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_TERM, ipp);
                                while(gtk_events_pending())
                                    gtk_main_iteration();
							}
					//}
				}
				if(key == 0xF8){//0x1B){
					lcd_clean();
					lcd_printf_ex(ALG_CENTER, 5, "İPTAL EDİLDİ");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "\n\nİPTAL EDİLDİ");
                    while(gtk_events_pending())
                        gtk_main_iteration();
					//usleep(500000);
					napms(500);
					free(cp);
					break;
				}
				strcpy(new_ip, ipp);
				strcat(new_ip, ".");

				token = strtok (NULL, delimiters); //ikinci oktet
				sprintf(ipp, "%03d", (int) strtol(token, NULL, 10));
				strcat(new_ip, ipp);
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 0, "LOCAL IP");
				lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_TERM);
				lcd_printf_ex(ALG_CENTER, 2, "YENİ IP");
				lcd_printf_ex(ALG_CENTER, 3, new_ip);
				lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
				lcd_flip();
                //0.53
                gtk_clean();
                //0.61
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                gtk_printf(label1, "LOCAL IP\n%s\nYENİ IP\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_TERM, new_ip);
                while(gtk_events_pending())
                    gtk_main_iteration();

				i=strlen(new_ip);
				j = i-3;
				key = -1;
				while((key != 0xF8) && ((key != KEY_ENTR)||(i==j))){
					//if(kb_hit()){

						//key = kb_getkey();
						key = KEYB_getch(0);
						if(key == KEY_BKSP){
							if(i>j) i--;
							new_ip[i] = 0;
							lcd_clean();
							lcd_printf_ex(ALG_CENTER, 0, "LOCAL IP");
							lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_TERM);
							lcd_printf_ex(ALG_CENTER, 2, "YENİ IP");
							lcd_printf_ex(ALG_CENTER, 3, new_ip);
							lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
							lcd_flip();
							//0.53
                            gtk_clean();
                            //0.61
                            gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                            gtk_printf(label1, "LOCAL IP\n%s\nYENİ IP\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_TERM, new_ip);
                            while(gtk_events_pending())
                                gtk_main_iteration();
						}
						else
							if(key <= 9 && key >= 0 && i<(j+3)){
								new_ip[i++] = (char) key+48;
								lcd_clean();
								lcd_clean();
								lcd_printf_ex(ALG_CENTER, 0, "LOCAL IP");
								lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_TERM);
								lcd_printf_ex(ALG_CENTER, 2, "YENİ IP");
								lcd_printf_ex(ALG_CENTER, 3, new_ip);
								lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
								lcd_flip();
								//0.53
                                gtk_clean();
                                //0.61
                                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                                gtk_printf(label1, "LOCAL IP\n%s\nYENİ IP\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_TERM, new_ip);
                                while(gtk_events_pending())
                                    gtk_main_iteration();
							}
					//}
				}
				if(key == 0xF8){
					lcd_clean();
					lcd_printf_ex(ALG_CENTER, 5, "İPTAL EDİLDİ");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "\n\nİPTAL EDİLDİ");
                    while(gtk_events_pending())
                        gtk_main_iteration();
					//usleep(500000);
					napms(500);
					free(cp);
					break;
				}
				strcat(new_ip, ".");


				token = strtok (NULL, delimiters); //ikinci oktet
				sprintf(ipp, "%03d", (int) strtol(token, NULL, 10));
				strcat(new_ip, ipp);
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 0, "LOCAL IP");
				lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_TERM);
				lcd_printf_ex(ALG_CENTER, 2, "YENİ IP");
				lcd_printf_ex(ALG_CENTER, 3, new_ip);
				lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
				lcd_flip();
				//0.53
                gtk_clean();
                //0.61
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                gtk_printf(label1, "LOCAL IP\n%s\nYENİ IP\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_TERM, new_ip);
                while(gtk_events_pending())
                    gtk_main_iteration();
				i = strlen(new_ip);
				j = i-3;
				key = -1;
				while((key != 0xF8) && ((key != KEY_ENTR)||(i==j))){
					//if(kb_hit()){

						//key = kb_getkey();
						key = KEYB_getch(0);
						if(key == KEY_BKSP){
							if(i>j) i--;
							new_ip[i] = 0;
							lcd_clean();
							lcd_printf_ex(ALG_CENTER, 0, "LOCAL IP");
							lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_TERM);
							lcd_printf_ex(ALG_CENTER, 2, "YENİ IP");
							lcd_printf_ex(ALG_CENTER, 3, new_ip);
							lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
							lcd_flip();
							//0.53
                            gtk_clean();
                            //0.61
                            gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                            gtk_printf(label1, "LOCAL IP\n%s\nYENİ IP\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_TERM, new_ip);
                            while(gtk_events_pending())
                                gtk_main_iteration();
						}
						else
							if(key <= 9 && key >= 0 && i<(j+3)){
								new_ip[i++] = (char) key+48;
								lcd_clean();
								lcd_clean();
								lcd_printf_ex(ALG_CENTER, 0, "LOCAL IP");
								lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_TERM);
								lcd_printf_ex(ALG_CENTER, 2, "YENİ IP");
								lcd_printf_ex(ALG_CENTER, 3, new_ip);
								lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
								lcd_flip();
								//0.53
                                gtk_clean();
                                //0.61
                                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                                gtk_printf(label1, "LOCAL IP\n%s\nYENİ IP\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_TERM, new_ip);
                                while(gtk_events_pending())
                                    gtk_main_iteration();
							}
					//}
				}
				if(key == 0xF8){
					lcd_clean();
					lcd_flip();
					//0.53
                    gtk_clean();
                    while(gtk_events_pending())
                        gtk_main_iteration();
					//usleep(500000);
					napms(500);
					free(cp);
					break;
				}
				strcat(new_ip, ".");


				token = strtok (NULL, delimiters); //ikinci oktet
				sprintf(ipp, "%03d", (int) strtol(token, NULL, 10));
				strcat(new_ip, ipp);
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 0, "LOCAL IP");
				lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_TERM);
				lcd_printf_ex(ALG_CENTER, 2, "YENİ IP");
				lcd_printf_ex(ALG_CENTER, 3, new_ip);
				lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
				lcd_flip();
				//0.53
                gtk_clean();
                //0.61
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                gtk_printf(label1, "LOCAL IP\n%s\nYENİ IP\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_TERM, new_ip);
                while(gtk_events_pending())
                    gtk_main_iteration();
				i = strlen(new_ip);
				j = i-3;
				key = -1;
				while((key != 0xF8) && ((key != KEY_ENTR)||(i==j))){
					//if(kb_hit()){

						//key = kb_getkey();
						key = KEYB_getch(0);
						if(key == KEY_BKSP){
							if(i>j) i--;
							new_ip[i] = 0;
							lcd_clean();
							lcd_printf_ex(ALG_CENTER, 0, "LOCAL IP");
							lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_TERM);
							lcd_printf_ex(ALG_CENTER, 2, "YENİ IP");
							lcd_printf_ex(ALG_CENTER, 3, new_ip);
							lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
							lcd_flip();
							//0.53
                            gtk_clean();
                            //0.61
                            gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                            gtk_printf(label1, "LOCAL IP\n%s\nYENİ IP\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_TERM, new_ip);
                            while(gtk_events_pending())
                                gtk_main_iteration();
						}
						else
							if(key <= 9 && key >= 0 && i<(j+3)){
								new_ip[i++] = (char) key+48;
								lcd_clean();
								lcd_printf_ex(ALG_CENTER, 0, "LOCAL IP");
								lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_TERM);
								lcd_printf_ex(ALG_CENTER, 2, "YENİ IP");
								lcd_printf_ex(ALG_CENTER, 3, new_ip);
								lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
								lcd_flip();
								//0.53
                                gtk_clean();
                                //0.61
                                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                                gtk_printf(label1, "LOCAL IP\n%s\nYENİ IP\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_TERM, new_ip);
                                while(gtk_events_pending())
                                    gtk_main_iteration();
							}
					//}
				}
				if(key == 0xF8){
					lcd_clean();
					lcd_printf_ex(ALG_CENTER, 5, "İPTAL EDİLDİ");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "\n\nİPTAL EDİLDİ");
                    while(gtk_events_pending())
                        gtk_main_iteration();
					//usleep(500000);
					napms(500);
					free(cp);
					break;
				}

				cp = strdup(new_ip);   //Make writable copy.
				token = strtok (cp, delimiters); //first octet
				octet = (int) strtol(token, NULL, 10);
				if((octet >0) && (octet < 256))
					sprintf(ipp, "%d", octet);
				else
				{
					//lcd_set_bk_color(COLOR_RED);
					//lcd_set_font_color(COLOR_YELLOW);
					lcd_clean();
					lcd_flip();
					lcd_printf_ex(ALG_CENTER, 3, "HATALI IP DEĞERİ");
					lcd_printf_ex(ALG_CENTER, 5, "MEVCUT IP KORUNACAK");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "\nHATALI IP DEĞERİ\nMEVCUT IP KORUNACAK");
                    while(gtk_events_pending())
                        gtk_main_iteration();
					MSJ_Ses("5343332313");
					//sleep(1);
					napms(1000);
					//lcd_set_bk_color(COLOR_BLUE);
					free(cp);
					break;
				}
				strcpy(ip_control, ipp);
				strcat(ip_control, ".");

				token = strtok (NULL, delimiters);
				octet = (int) strtol(token, NULL, 10);
				if(octet < 256)
					sprintf(ipp, "%d", octet);
				else
				{
					//lcd_set_bk_color(COLOR_RED);
					//lcd_set_font_color(COLOR_YELLOW);
					lcd_clean();
					lcd_flip();
					lcd_printf_ex(ALG_CENTER, 3, "HATALI IP DEĞERİ");
					lcd_printf_ex(ALG_CENTER, 5, "MEVCUT IP KORUNACAK");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "\nHATALI IP DEĞERİ\nMEVCUT IP KORUNACAK");
                    while(gtk_events_pending())
                        gtk_main_iteration();
					MSJ_Ses("5343332313");
					//sleep(1);
					napms(1000);
					//lcd_set_bk_color(COLOR_BLUE);
					free(cp);
					break;
				}
				strcat(ip_control, ipp);
				strcat(ip_control, ".");

				token = strtok (NULL, delimiters);
				octet = (int) strtol(token, NULL, 10);
				if(octet < 256)
					sprintf(ipp, "%d", octet);
				else
				{
					//lcd_set_bk_color(COLOR_RED);
					//lcd_set_font_color(COLOR_YELLOW);
					lcd_clean();
					lcd_flip();
					lcd_printf_ex(ALG_CENTER, 3, "HATALI IP DEĞERİ");
					lcd_printf_ex(ALG_CENTER, 5, "MEVCUT IP KORUNACAK");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "\nHATALI IP DEĞERİ\nMEVCUT IP KORUNACAK");
                    while(gtk_events_pending())
                        gtk_main_iteration();
					MSJ_Ses("5343332313");
					//sleep(1);
					napms(1000);
					//lcd_set_bk_color(COLOR_BLUE);
					free(cp);
					break;
				}
				strcat(ip_control, ipp);
				strcat(ip_control, ".");

				token = strtok (NULL, delimiters);
				octet = (int) strtol(token, NULL, 10);
				if(octet < 256)
					sprintf(ipp, "%d", octet);
				else
				{
					//lcd_set_bk_color(COLOR_RED);
					//lcd_set_font_color(COLOR_YELLOW);
					lcd_clean();
					lcd_flip();
					lcd_printf_ex(ALG_CENTER, 3, "HATALI IP DEĞERİ");
					lcd_printf_ex(ALG_CENTER, 5, "MEVCUT IP KORUNACAK");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "\nHATALI IP DEĞERİ\nMEVCUT IP KORUNACAK");
                    while(gtk_events_pending())
                        gtk_main_iteration();
					MSJ_Ses("5343332313");
					//sleep(1);
					napms(1000);
					//lcd_set_bk_color(COLOR_BLUE);
					free(cp);
					break;
				}
				strcat(ip_control, ipp);
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 0, "LOCAL IP");
				lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_TERM);
				lcd_printf_ex(ALG_CENTER, 2, "YENİ IP");
				lcd_printf_ex(ALG_CENTER, 3, ip_control);
				lcd_printf_ex(ALG_CENTER, 4, "OLARAK KAYDEDİLECEK");
				lcd_printf_ex(ALG_CENTER, 5, "ONAYLIYOR MUSUNUZ?");
				lcd_printf_ex(ALG_CENTER, 6, "BKSP: İPTAL       ENT: KAYDET");
				lcd_flip();
                //0.53
                gtk_clean();
                //0.61
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                gtk_printf(label1, "LOCAL IP\n%s\nYENİ IP\n%s\nOLARAK KAYDEDİLECEK\nONAYLIYOR MUSUNUZ?\nBKSP: İPTAL       ENT: KAYDET", rec_TERM.IP_TERM, ip_control);
                while(gtk_events_pending())
                    gtk_main_iteration();
				key = -1;
				while((key != KEY_BKSP) && (key != KEY_ENTR)){
					//if(kb_hit()){

						//key = kb_getkey();
						key = KEYB_getch(0);
						if(key == KEY_BKSP){
							lcd_clean();
							lcd_printf_ex(ALG_CENTER, 5,"LOCAL IP KAYDEDİLMEDİ");
							lcd_flip();
							//0.53
                            gtk_clean();
                            //0.61
                            gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                            gtk_printf(label1, "\n\nLOCAL IP KAYDEDİLMEDİ");
                            while(gtk_events_pending())
                                gtk_main_iteration();
							MSJ_Ses("1212");
							//sleep(1);
                            napms(1000);
						}else
							if(key == KEY_ENTR){
								strcpy(rec_TERM.IP_TERM, ip_control);
								lcd_clean();
								lcd_printf_ex(ALG_CENTER, 5,"LOCAL IP KAYDEDİLDİ");
								lcd_flip();
								//0.53
                                gtk_clean();
                                //0.61
                                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                                gtk_printf(label1, "\n\nLOCAL IP KAYDEDİLDİ");
                                while(gtk_events_pending())
                                    gtk_main_iteration();
                                MSJ_Ses("1313131");
                                //sleep(1);
                                napms(1000);
                            }
					//}
				}

				free(cp);
				break;
			case 2: //SubnetMask
				cp = strdup(rec_TERM.IP_SUBNET);   //Make writable copy.
				token = strtok (cp, delimiters);   //ilk oktet
				sprintf(ipp, "%03d", (int) strtol(token, NULL, 10));
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 0, "SUBNETMASK");
				lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_SUBNET);
				lcd_printf_ex(ALG_CENTER, 2, "YENİ MASKE");
				lcd_printf_ex(ALG_CENTER, 3, ipp);
				lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
				lcd_flip();
				//0.53
                gtk_clean();
                //0.61
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                gtk_printf(label1, "SUBNETMASK\n%s\nYENİ MASKE\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_SUBNET, ipp);
                while(gtk_events_pending())
                    gtk_main_iteration();
				i = 3;
				key = -1;
				while((key != 0xF8) && ((key != KEY_ENTR) ||(i==0))){
					//if(kb_hit()){

						//key = kb_getkey();
						key = KEYB_getch(0);
						if(key == KEY_BKSP){
							if(i>0) i--;
							ipp[i] = 0;
							lcd_clean();
							lcd_printf_ex(ALG_CENTER, 0, "SUBNETMASK");
							lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_SUBNET);
							lcd_printf_ex(ALG_CENTER, 2, "YENİ MASKE");
							lcd_printf_ex(ALG_CENTER, 3, ipp);
							lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
							lcd_flip();
							//0.53
                            gtk_clean();
                            //0.61
                            gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                            gtk_printf(label1, "SUBNETMASK\n%s\nYENİ MASKE\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_SUBNET, ipp);
                            while(gtk_events_pending())
                                gtk_main_iteration();
						}
						else
							if(key <= 9 && key >= 0 && i<3){
								ipp[i++] = (char) key+48;
								lcd_clean();
								lcd_clean();
								lcd_printf_ex(ALG_CENTER, 0, "SUBNETMASK");
								lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_SUBNET);
								lcd_printf_ex(ALG_CENTER, 2, "YENİ MASKE");
								lcd_printf_ex(ALG_CENTER, 3, ipp);
								lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
								lcd_flip();
								//0.53
                                gtk_clean();
                                //0.61
                                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                                gtk_printf(label1, "SUBNETMASK\n%s\nYENİ MASKE\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_SUBNET, ipp);
                                while(gtk_events_pending())
                                    gtk_main_iteration();
							}
					//}
				}
				if(key == 0xF8){
					lcd_clean();
					lcd_printf_ex(ALG_CENTER, 5, "İPTAL EDİLDİ");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "\n\nİPTAL EDİLDİ");
                    while(gtk_events_pending())
                        gtk_main_iteration();
					//usleep(500000);
					napms(500);
					free(cp);
					break;
				}
				strcpy(new_ip, ipp);
				strcat(new_ip, ".");

				token = strtok (NULL, delimiters); //ikinci oktet
				sprintf(ipp, "%03d", (int) strtol(token, NULL, 10));
				strcat(new_ip, ipp);
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 0, "SUBNETMASK");
				lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_SUBNET);
				lcd_printf_ex(ALG_CENTER, 2, "YENİ MASKE");
				lcd_printf_ex(ALG_CENTER, 3, new_ip);
				lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
				lcd_flip();
				//0.53
                gtk_clean();
                //0.61
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                gtk_printf(label1, "SUBNETMASK\n%s\nYENİ MASKE\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_SUBNET, new_ip);
                while(gtk_events_pending())
                    gtk_main_iteration();


				i=strlen(new_ip);
				j = i-3;
				key = -1;
				while((key != 0xF8) && ((key != KEY_ENTR)||(i==j))){
					//if(kb_hit()){

						//key = kb_getkey();
						key = KEYB_getch(0);
						if(key == KEY_BKSP){
							if(i>j) i--;
							new_ip[i] = 0;
							lcd_clean();
							lcd_printf_ex(ALG_CENTER, 0, "SUBNETMASK");
							lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_SUBNET);
							lcd_printf_ex(ALG_CENTER, 2, "YENİ MASKE");
							lcd_printf_ex(ALG_CENTER, 3, new_ip);
							lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
							lcd_flip();
							//0.53
                            gtk_clean();
                            //0.61
                            gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                            gtk_printf(label1, "SUBNETMASK\n%s\nYENİ MASKE\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_SUBNET, new_ip);
                            while(gtk_events_pending())
                                gtk_main_iteration();
						}
						else
							if(key <= 9 && key >= 0 && i<(j+3)){
								new_ip[i++] = (char) key+48;
								lcd_clean();
								lcd_printf_ex(ALG_CENTER, 0, "SUBNETMASK");
								lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_SUBNET);
								lcd_printf_ex(ALG_CENTER, 2, "YENİ MASKE");
								lcd_printf_ex(ALG_CENTER, 3, new_ip);
								lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
								lcd_flip();
								//0.53
                                gtk_clean();
                                //0.61
                                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                                gtk_printf(label1, "SUBNETMASK\n%s\nYENİ MASKE\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_SUBNET, new_ip);
                                while(gtk_events_pending())
                                    gtk_main_iteration();
							}
					//}
				}
				if(key == 0xF8){
					lcd_clean();
					lcd_printf_ex(ALG_CENTER, 5, "İPTAL EDİLDİ");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "\n\nİPTAL EDİLDİ");
                    while(gtk_events_pending())
                        gtk_main_iteration();
					//usleep(500000);
					napms(500);
					free(cp);
					break;
				}
				strcat(new_ip, ".");


				token = strtok (NULL, delimiters); //ikinci oktet
				sprintf(ipp, "%03d", (int) strtol(token, NULL, 10));
				strcat(new_ip, ipp);
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 0, "SUBNETMASK");
				lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_SUBNET);
				lcd_printf_ex(ALG_CENTER, 2, "YENİ MASKE");
				lcd_printf_ex(ALG_CENTER, 3, new_ip);
				lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
				lcd_flip();
				//0.53
                gtk_clean();
                //0.61
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                gtk_printf(label1, "SUBNETMASK\n%s\nYENİ MASKE\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_SUBNET, new_ip);
                while(gtk_events_pending())
                    gtk_main_iteration();
				i = strlen(new_ip);
				j = i-3;
				key = -1;
				while((key != 0xF8) && ((key != KEY_ENTR)||(i==j))){
					//if(kb_hit()){

						//key = kb_getkey();
						key = KEYB_getch(0);
						if(key == KEY_BKSP){
							if(i>j) i--;
							new_ip[i] = 0;
							lcd_clean();
							lcd_printf_ex(ALG_CENTER, 0, "SUBNETMASK");
							lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_SUBNET);
							lcd_printf_ex(ALG_CENTER, 2, "YENİ MASKE");
							lcd_printf_ex(ALG_CENTER, 3, new_ip);
							lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
							lcd_flip();
							//0.53
                            gtk_clean();
                            //0.61
                            gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                            gtk_printf(label1, "SUBNETMASK\n%s\nYENİ MASKE\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_SUBNET, new_ip);
                            while(gtk_events_pending())
                                gtk_main_iteration();
						}
						else
							if(key <= 9 && key >= 0 && i<(j+3)){
								new_ip[i++] = (char) key+48;
								lcd_clean();
								lcd_printf_ex(ALG_CENTER, 0, "SUBNETMASK");
								lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_SUBNET);
								lcd_printf_ex(ALG_CENTER, 2, "YENİ MASKE");
								lcd_printf_ex(ALG_CENTER, 3, new_ip);
								lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
								lcd_flip();
								//0.53
                                gtk_clean();
                                //0.61
                                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                                gtk_printf(label1, "SUBNETMASK\n%s\nYENİ MASKE\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_SUBNET, new_ip);
                                while(gtk_events_pending())
                                    gtk_main_iteration();
							}
					//}
				}
				if(key == 0xF8){
					lcd_clean();
					lcd_flip();
					//0.53
                    gtk_clean();
                    while(gtk_events_pending())
                        gtk_main_iteration();
					//usleep(500000);
					napms(500);
					free(cp);
					break;
				}
				strcat(new_ip, ".");


				token = strtok (NULL, delimiters); //ikinci oktet
				sprintf(ipp, "%03d", (int) strtol(token, NULL, 10));
				strcat(new_ip, ipp);
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 0, "SUBNETMASK");
				lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_SUBNET);
				lcd_printf_ex(ALG_CENTER, 2, "YENİ MASKE");
				lcd_printf_ex(ALG_CENTER, 3, new_ip);
				lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
				lcd_flip();
				//0.53
                gtk_clean();
                //0.61
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                gtk_printf(label1, "SUBNETMASK\n%s\nYENİ MASKE\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_SUBNET, new_ip);
                while(gtk_events_pending())
                    gtk_main_iteration();
				i = strlen(new_ip);
				j = i-3;
				key = -1;
				while((key != 0xF8) && ((key != KEY_ENTR)||(i==j))){
					//if(kb_hit()){

						//key = kb_getkey();
						key = KEYB_getch(0);
						if(key == KEY_BKSP){
							if(i>j) i--;
							new_ip[i] = 0;
							lcd_clean();
							lcd_printf_ex(ALG_CENTER, 0, "SUBNETMASK");
							lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_SUBNET);
							lcd_printf_ex(ALG_CENTER, 2, "YENİ MASKE");
							lcd_printf_ex(ALG_CENTER, 3, new_ip);
							lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
							lcd_flip();
							//0.53
                            gtk_clean();
                            //0.61
                            gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                            gtk_printf(label1, "SUBNETMASK\n%s\nYENİ MASKE\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_SUBNET, new_ip);
                            while(gtk_events_pending())
                                gtk_main_iteration();
						}
						else
							if(key <= 9 && key >= 0 && i<(j+3)){
								new_ip[i++] = (char) key+48;
								lcd_clean();
								lcd_clean();
								lcd_printf_ex(ALG_CENTER, 0, "SUBNETMASK");
								lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_SUBNET);
								lcd_printf_ex(ALG_CENTER, 2, "YENİ MASKE");
								lcd_printf_ex(ALG_CENTER, 3, new_ip);
								lcd_printf_ex(ALG_CENTER, 210, "BKSP: SIL               ENT: KAYDET");
								lcd_flip();
								//0.53
                                gtk_clean();
                                //0.61
                                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                                gtk_printf(label1, "SUBNETMASK\n%s\nYENİ MASKE\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_SUBNET, new_ip);
                                while(gtk_events_pending())
                                    gtk_main_iteration();
							}
					//}
				}
				if(key == 0xF8){
					lcd_clean();
					lcd_printf_ex(ALG_CENTER, 5, "İPTAL EDİLDİ");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "\n\nİPTAL EDİLDİ");
                    while(gtk_events_pending())
                        gtk_main_iteration();
					//usleep(500000);
					napms(500);
					free(cp);
					break;
				}

				cp = strdup(new_ip);   //Make writable copy.
				token = strtok (cp, delimiters); //first octet
				octet = (int) strtol(token, NULL, 10);
				if((octet >0) && (octet < 256))
					sprintf(ipp, "%d", octet);
				else
				{
					//lcd_set_bk_color(COLOR_RED);
					//lcd_set_font_color(COLOR_YELLOW);
					lcd_clean();
					lcd_flip();
					lcd_printf_ex(ALG_CENTER, 3, "HATALI MASKE DEĞERİ");
					lcd_printf_ex(ALG_CENTER, 5, "MEVCUT MASKE KORUNACAK");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "\nHATALI MASKE DEĞERİ\nMEVCUT MASKE KORUNACAK");
                    while(gtk_events_pending())
                        gtk_main_iteration();
					MSJ_Ses("5343332313");
					//sleep(1);
					napms(1000);
					//lcd_set_bk_color(COLOR_BLUE);
					free(cp);
					break;
				}
				strcpy(ip_control, ipp);
				strcat(ip_control, ".");

				token = strtok (NULL, delimiters);
				octet = (int) strtol(token, NULL, 10);
				if(octet < 256)
					sprintf(ipp, "%d", octet);
				else
				{
					//lcd_set_bk_color(COLOR_RED);
					//lcd_set_font_color(COLOR_YELLOW);
					lcd_clean();
					lcd_flip();
					lcd_printf_ex(ALG_CENTER, 3, "HATALI MASKE DEĞERİ");
					lcd_printf_ex(ALG_CENTER, 5, "MEVCUT MASKE KORUNACAK");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "\nHATALI MASKE DEĞERİ\nMEVCUT MASKE KORUNACAK");
                    while(gtk_events_pending())
                        gtk_main_iteration();
					MSJ_Ses("5343332313");
					//sleep(1);
					napms(1000);
					//lcd_set_bk_color(COLOR_BLUE);
					free(cp);
					break;
				}
				strcat(ip_control, ipp);
				strcat(ip_control, ".");

				token = strtok (NULL, delimiters);
				octet = (int) strtol(token, NULL, 10);
				if(octet < 256)
					sprintf(ipp, "%d", octet);
				else
				{
					//lcd_set_bk_color(COLOR_RED);
					//lcd_set_font_color(COLOR_YELLOW);
					lcd_clean();
					lcd_flip();
					lcd_printf_ex(ALG_CENTER, 3, "HATALI MASKE DEĞERİ");
					lcd_printf_ex(ALG_CENTER, 5, "MEVCUT MASKE KORUNACAK");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "\nHATALI MASKE DEĞERİ\nMEVCUT MASKE KORUNACAK");
                    while(gtk_events_pending())
                        gtk_main_iteration();
					MSJ_Ses("5343332313");
					//sleep(1);
					napms(1000);
					//lcd_set_bk_color(COLOR_BLUE);
					free(cp);
					break;
				}
				strcat(ip_control, ipp);
				strcat(ip_control, ".");

				token = strtok (NULL, delimiters);
				octet = (int) strtol(token, NULL, 10);
				if(octet < 256)
					sprintf(ipp, "%d", octet);
				else
				{
					//lcd_set_bk_color(COLOR_RED);
					//lcd_set_font_color(COLOR_YELLOW);
					lcd_clean();
					lcd_flip();
					lcd_printf_ex(ALG_CENTER, 3, "HATALI MASKE DEĞERİ");
					lcd_printf_ex(ALG_CENTER, 5, "MEVCUT MASKE KORUNACAK");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "\nHATALI MASKE DEĞERİ\nMEVCUT MASKE KORUNACAK");
                    while(gtk_events_pending())
                        gtk_main_iteration();
					MSJ_Ses("5343332313");
					//sleep(1);
					napms(1000);
					//lcd_set_bk_color(COLOR_BLUE);
					free(cp);
					break;
				}
				strcat(ip_control, ipp);
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 0, "SUBNETMASK");
				lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_SUBNET);
				lcd_printf_ex(ALG_CENTER, 2, "YENİ MASKE");
				lcd_printf_ex(ALG_CENTER, 3, ip_control);
				lcd_printf_ex(ALG_CENTER, 4, "OLARAK KAYDEDİLECEK");
				lcd_printf_ex(ALG_CENTER, 5, "ONAYLIYOR MUSUNUZ?");
				lcd_printf_ex(ALG_CENTER, 6, "BKSP: İPTAL       ENT: KAYDET");
				lcd_flip();
                //0.53
                gtk_clean();
                //0.61
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                gtk_printf(label1, "SUBNETMASK\n%s\nYENİ MASKE\n%s\nOLARAK KAYDEDİLECEK\nONAYLIYOR MUSUNUZ?\nBKSP: İPTAL       ENT: KAYDET", rec_TERM.IP_SUBNET, ip_control);
                while(gtk_events_pending())
                    gtk_main_iteration();
				key = -1;
				while((key != KEY_BKSP) && (key != KEY_ENTR)){
					//if(kb_hit()){

						//key = kb_getkey();
						key = KEYB_getch(0);
						if(key == KEY_BKSP){
							lcd_clean();
							lcd_printf_ex(ALG_CENTER, 5,"SUBNETMASK KAYDEDİLMEDİ");
							lcd_flip();
							//0.53
                            gtk_clean();
                            //0.61
                            gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                            gtk_printf(label1, "\n\nSUBNETMASK KAYDEDİLMEDİ");
                            while(gtk_events_pending())
                                gtk_main_iteration();
							MSJ_Ses("1212");
							//sleep(1);
                            napms(1000);
						}else
							if(key == KEY_ENTR){
								strcpy(rec_TERM.IP_SUBNET, ip_control);
								lcd_clean();
								lcd_printf_ex(ALG_CENTER, 5,"SUBNETMASK KAYDEDİLDİ");
								lcd_flip();
								//0.53
                                gtk_clean();
                                //0.61
                                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                                gtk_printf(label1, "\n\nSUBNETMASK KAYDEDİLDİ");
                                while(gtk_events_pending())
                                    gtk_main_iteration();
								MSJ_Ses("1313131");
								//sleep(1);
                                napms(1000);
							}
					//}
				}

				free(cp);
				//selected = -1;
				break;
			case 3://Gateway
				cp = strdup(rec_TERM.IP_GATEWAY);   //Make writable copy.
				token = strtok (cp, delimiters);   //ilk oktet
				sprintf(ipp, "%03d", (int) strtol(token, NULL, 10));
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 0, "GATEWAY");
				lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_GATEWAY);
				lcd_printf_ex(ALG_CENTER, 2, "YENİ GATEWAY");
				lcd_printf_ex(ALG_CENTER, 3, ipp);
				lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
				lcd_flip();
				//0.53
                gtk_clean();
                //0.61
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                gtk_printf(label1, "GATEWAY\n%s\nYENİ GATEWAY\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_GATEWAY, ipp);
                while(gtk_events_pending())
                    gtk_main_iteration();
				i = 3;
				key = -1;
				while((key != 0xF8) && ((key != KEY_ENTR) ||(i==0))){
					//if(kb_hit()){

						//key = kb_getkey();
						key = KEYB_getch(0);
						if(key == KEY_BKSP){
							if(i>0) i--;
							ipp[i] = 0;
							lcd_clean();
							lcd_printf_ex(ALG_CENTER, 0, "GATEWAY");
							lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_GATEWAY);
							lcd_printf_ex(ALG_CENTER, 2, "YENİ GATEWAY");
							lcd_printf_ex(ALG_CENTER, 3, ipp);
							lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
							lcd_flip();
							//0.53
                            gtk_clean();
                            //0.61
                            gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                            gtk_printf(label1, "GATEWAY\n%s\nYENİ GATEWAY\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_GATEWAY, ipp);
                            while(gtk_events_pending())
                                gtk_main_iteration();
						}
						else
							if(key <= 9 && key >= 0 && i<3){
								ipp[i++] = (char) key+48;
								lcd_clean();
								lcd_clean();
								lcd_printf_ex(ALG_CENTER, 0, "GATEWAY");
								lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_GATEWAY);
								lcd_printf_ex(ALG_CENTER, 2, "YENİ GATEWAY");
								lcd_printf_ex(ALG_CENTER, 3, ipp);
								lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
								lcd_flip();
								//0.53
                                gtk_clean();
                                //0.61
                                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                                gtk_printf(label1, "GATEWAY\n%s\nYENİ GATEWAY\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_GATEWAY, ipp);
                                while(gtk_events_pending())
                                    gtk_main_iteration();
							}
					//}
				}
				if(key == 0xF8){
					lcd_clean();
					lcd_printf_ex(ALG_CENTER, 5, "İPTAL EDİLDİ");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "\n\nİPTAL EDİLDİ");
                    while(gtk_events_pending())
                        gtk_main_iteration();
					//usleep(500000);
					napms(500);
					free(cp);
					break;
				}
				strcpy(new_ip, ipp);
				strcat(new_ip, ".");

				token = strtok (NULL, delimiters); //ikinci oktet
				sprintf(ipp, "%03d", (int) strtol(token, NULL, 10));
				strcat(new_ip, ipp);
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 0, "GATEWAY");
				lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_GATEWAY);
				lcd_printf_ex(ALG_CENTER, 2, "YENİ GATEWAY");
				lcd_printf_ex(ALG_CENTER, 3, new_ip);
				lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
				lcd_flip();
                //0.53
                gtk_clean();
                //0.61
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                gtk_printf(label1, "GATEWAY\n%s\nYENİ GATEWAY\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_GATEWAY, new_ip);
                while(gtk_events_pending())
                    gtk_main_iteration();

				i=strlen(new_ip);
				j = i-3;
				key = -1;
				while((key != 0xF8) && ((key != KEY_ENTR)||(i==j))){
					//if(kb_hit()){

						//key = kb_getkey();
						key = KEYB_getch(0);
						if(key == KEY_BKSP){
							if(i>j) i--;
							new_ip[i] = 0;
							lcd_clean();
							lcd_printf_ex(ALG_CENTER, 0, "GATEWAY");
							lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_GATEWAY);
							lcd_printf_ex(ALG_CENTER, 2, "YENİ GATEWAY");
							lcd_printf_ex(ALG_CENTER, 3, new_ip);
							lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
							lcd_flip();
							//0.53
                            gtk_clean();
                            //0.61
                            gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                            gtk_printf(label1, "GATEWAY\n%s\nYENİ GATEWAY\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_GATEWAY, new_ip);
                            while(gtk_events_pending())
                                gtk_main_iteration();
						}
						else
							if(key <= 9 && key >= 0 && i<(j+3)){
								new_ip[i++] = (char) key+48;
								lcd_clean();
								lcd_printf_ex(ALG_CENTER, 0, "GATEWAY");
								lcd_printf_ex(ALG_CENTER, 1,rec_TERM.IP_GATEWAY);
								lcd_printf_ex(ALG_CENTER, 2, "YENİ GATEWAY");
								lcd_printf_ex(ALG_CENTER, 3, new_ip);
								lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
								lcd_flip();
								//0.53
                                gtk_clean();
                                //0.61
                                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                                gtk_printf(label1, "GATEWAY\n%s\nYENİ GATEWAY\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_GATEWAY, new_ip);
                                while(gtk_events_pending())
                                    gtk_main_iteration();
							}
					//}
				}
				if(key == 0xF8){
					lcd_clean();
					lcd_printf_ex(ALG_CENTER, 5, "İPTAL EDİLDİ");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "\n\nİPTAL EDİLDİ");
                    while(gtk_events_pending())
                        gtk_main_iteration();
					//usleep(500000);
					napms(500);
					free(cp);
					break;
				}
				strcat(new_ip, ".");


				token = strtok (NULL, delimiters); //ikinci oktet
				sprintf(ipp, "%03d", (int) strtol(token, NULL, 10));
				strcat(new_ip, ipp);
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 0, "GATEWAY");
				lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_GATEWAY);
				lcd_printf_ex(ALG_CENTER, 2, "YENİ GATEWAY");
				lcd_printf_ex(ALG_CENTER, 3, new_ip);
				lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
				lcd_flip();
				//0.53
                gtk_clean();
                //0.61
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                gtk_printf(label1, "GATEWAY\n%s\nYENİ GATEWAY\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_GATEWAY, new_ip);
                while(gtk_events_pending())
                    gtk_main_iteration();
				i = strlen(new_ip);
				j = i-3;
				key = -1;
				while((key != 0xF8) && ((key != KEY_ENTR)||(i==j))){
					//if(kb_hit()){

						//key = kb_getkey();
						key = KEYB_getch(0);
						if(key == KEY_BKSP){
							if(i>j) i--;
							new_ip[i] = 0;
							lcd_clean();
							lcd_printf_ex(ALG_CENTER, 0, "GATEWAY");
							lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_GATEWAY);
							lcd_printf_ex(ALG_CENTER, 2, "YENİ GATEWAY");
							lcd_printf_ex(ALG_CENTER, 3, new_ip);
							lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
							lcd_flip();
							//0.53
                            gtk_clean();
                            //0.61
                            gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                            gtk_printf(label1, "GATEWAY\n%s\nYENİ GATEWAY\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_GATEWAY, new_ip);
                            while(gtk_events_pending())
                                gtk_main_iteration();
						}
						else
							if(key <= 9 && key >= 0 && i<(j+3)){
								new_ip[i++] = (char) key+48;
								lcd_clean();
								lcd_printf_ex(ALG_CENTER, 0, "GATEWAY");
								lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_GATEWAY);
								lcd_printf_ex(ALG_CENTER, 2, "YENİ GATEWAY");
								lcd_printf_ex(ALG_CENTER, 3, new_ip);
								lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
								lcd_flip();
								//0.53
                                gtk_clean();
                                //0.61
                                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                                gtk_printf(label1, "GATEWAY\n%s\nYENİ GATEWAY\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_GATEWAY, new_ip);
                                while(gtk_events_pending())
                                    gtk_main_iteration();
							}
					//}
				}
				if(key == 0xF8){
					lcd_clean();
					lcd_flip();
					//0.53
                    gtk_clean();
                    while(gtk_events_pending())
                        gtk_main_iteration();
					//usleep(500000);
					napms(500);
					free(cp);
					break;
				}
				strcat(new_ip, ".");


				token = strtok (NULL, delimiters); //ikinci oktet
				sprintf(ipp, "%03d", (int) strtol(token, NULL, 10));
				strcat(new_ip, ipp);
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 0, "GATEWAY");
				lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_GATEWAY);
				lcd_printf_ex(ALG_CENTER, 2, "YENİ GATEWAY");
				lcd_printf_ex(ALG_CENTER, 3, new_ip);
				lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
				lcd_flip();
				//0.53
                gtk_clean();
                //0.61
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                gtk_printf(label1, "GATEWAY\n%s\nYENİ GATEWAY\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_GATEWAY, new_ip);
                while(gtk_events_pending())
                    gtk_main_iteration();
				i = strlen(new_ip);
				j = i-3;
				key = -1;
				while((key != 0xF8) && ((key != KEY_ENTR)||(i==j))){
					//if(kb_hit()){

						//key = kb_getkey();
						key = KEYB_getch(0);
						if(key == KEY_BKSP){
							if(i>j) i--;
							new_ip[i] = 0;
							lcd_clean();
							lcd_printf_ex(ALG_CENTER, 0, "GATEWAY");
							lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_GATEWAY);
							lcd_printf_ex(ALG_CENTER, 2, "YENİ GATEWAY");
							lcd_printf_ex(ALG_CENTER, 3, new_ip);
							lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
							lcd_flip();
							//0.53
                            gtk_clean();
                            //0.61
                            gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                            gtk_printf(label1, "GATEWAY\n%s\nYENİ GATEWAY\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_GATEWAY, new_ip);
                            while(gtk_events_pending())
                                gtk_main_iteration();
						}
						else
							if(key <= 9 && key >= 0 && i<(j+3)){
								new_ip[i++] = (char) key+48;
								lcd_clean();
								lcd_clean();
								lcd_printf_ex(ALG_CENTER, 0, "GATEWAY");
								lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_GATEWAY);
								lcd_printf_ex(ALG_CENTER, 2, "YENİ GATEWAY");
								lcd_printf_ex(ALG_CENTER, 3, new_ip);
								lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
								lcd_flip();
								//0.53
                                gtk_clean();
                                //0.61
                                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                                gtk_printf(label1, "GATEWAY\n%s\nYENİ GATEWAY\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_GATEWAY, new_ip);
                                while(gtk_events_pending())
                                    gtk_main_iteration();
							}
					//}
				}
				if(key == 0xF8){
					lcd_clean();
					lcd_printf_ex(ALG_CENTER, 5, "İPTAL EDİLDİ");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "\n\nİPTAL EDİLDİ");
                    while(gtk_events_pending())
                        gtk_main_iteration();
					//usleep(500000);
					napms(500);
					free(cp);
					break;
				}

				cp = strdup(new_ip);   //Make writable copy.
				token = strtok (cp, delimiters); //first octet
				octet = (int) strtol(token, NULL, 10);
				if((octet >0) && (octet < 256))
					sprintf(ipp, "%d", octet);
				else
				{
					//lcd_set_bk_color(COLOR_RED);
					//lcd_set_font_color(COLOR_YELLOW);
					lcd_clean();
					lcd_flip();
					lcd_printf_ex(ALG_CENTER, 3, "HATALI GATEWAY DEĞERİ");
					lcd_printf_ex(ALG_CENTER, 5, "MEVCUT GATEWAY IP KORUNACAK");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "\nHATALI GATEWAY DEĞERİ\nMEVCUT GATEWAY IP KORUNACAK");
                    while(gtk_events_pending())
                        gtk_main_iteration();
					MSJ_Ses("5343332313");
					//sleep(1);
					napms(1000);
					//lcd_set_bk_color(COLOR_BLUE);
					free(cp);
					break;
				}
				strcpy(ip_control, ipp);
				strcat(ip_control, ".");

				token = strtok (NULL, delimiters);
				octet = (int) strtol(token, NULL, 10);
				if(octet < 256)
					sprintf(ipp, "%d", octet);
				else
				{
					//lcd_set_bk_color(COLOR_RED);
					//lcd_set_font_color(COLOR_YELLOW);
					lcd_clean();
					lcd_flip();
					lcd_printf_ex(ALG_CENTER, 3, "HATALI GATEWAY DEGERİ");
					lcd_printf_ex(ALG_CENTER, 5, "MEVCUT GATEWAY IP KORUNACAK");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "\nHATALI GATEWAY DEĞERİ\nMEVCUT GATEWAY IP KORUNACAK");
                    while(gtk_events_pending())
                        gtk_main_iteration();
					MSJ_Ses("5343332313");
					//sleep(1);
					napms(1000);
					//lcd_set_bk_color(COLOR_BLUE);
					free(cp);
					break;
				}
				strcat(ip_control, ipp);
				strcat(ip_control, ".");

				token = strtok (NULL, delimiters);
				octet = (int) strtol(token, NULL, 10);
				if(octet < 256)
					sprintf(ipp, "%d", octet);
				else
				{
					//lcd_set_bk_color(COLOR_RED);
					//lcd_set_font_color(COLOR_YELLOW);
					lcd_clean();
					lcd_flip();
					lcd_printf_ex(ALG_CENTER, 3, "HATALI GATEWAY DEGERİ");
					lcd_printf_ex(ALG_CENTER, 5, "MEVCUT GATEWAY IP KORUNACAK");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "\nHATALI GATEWAY DEĞERİ\nMEVCUT GATEWAY IP KORUNACAK");
                    while(gtk_events_pending())
                        gtk_main_iteration();
					MSJ_Ses("5343332313");
					//sleep(1);
					napms(1000);
					//lcd_set_bk_color(COLOR_BLUE);
					free(cp);
					break;
				}
				strcat(ip_control, ipp);
				strcat(ip_control, ".");

				token = strtok (NULL, delimiters);
				octet = (int) strtol(token, NULL, 10);
				if(octet < 256)
					sprintf(ipp, "%d", octet);
				else
				{
					//lcd_set_bk_color(COLOR_RED);
					//lcd_set_font_color(COLOR_YELLOW);
					lcd_clean();
					lcd_flip();
					lcd_printf_ex(ALG_CENTER, 3, "HATALI GATEWAY DEGERİ");
					lcd_printf_ex(ALG_CENTER, 5, "MEVCUT GATEWAY IP KORUNACAK");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "\nHATALI GATEWAY DEĞERİ\nMEVCUT GATEWAY IP KORUNACAK");
                    while(gtk_events_pending())
                        gtk_main_iteration();
					MSJ_Ses("5343332313");
					//sleep(1);
					napms(1000);
					//lcd_set_bk_color(COLOR_BLUE);
					free(cp);
					break;
				}
				strcat(ip_control, ipp);
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 0, "GATEWAY");
				lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_GATEWAY);
				lcd_printf_ex(ALG_CENTER, 2, "YENİ GATEWAY");
				lcd_printf_ex(ALG_CENTER, 3, ip_control);
				lcd_printf_ex(ALG_CENTER, 4, "OLARAK KAYDEDİLECEK");
				lcd_printf_ex(ALG_CENTER, 5, "ONAYLIYOR MUSUNUZ?");
				lcd_printf_ex(ALG_CENTER, 6, "BKSP: İPTAL       ENTER: KAYDET");
				lcd_flip();
                //0.53
                gtk_clean();
                //0.61
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                gtk_printf(label1, "GATEWAY\n%s\nYENİ GATEWAY\n%s\nOLARAK KAYDEDİLECEK\nONAYLIYOR MUSUNUZ?\nBKSP: İPTAL       ENTER: KAYDET", rec_TERM.IP_GATEWAY, ip_control);
                while(gtk_events_pending())
                    gtk_main_iteration();
				key = -1;
				while((key != KEY_BKSP) && (key != KEY_ENTR)){
					//if(kb_hit()){

						//key = kb_getkey();
						key = KEYB_getch(0);
						if(key == KEY_BKSP){
							lcd_clean();
							lcd_printf_ex(ALG_CENTER, 5,"GATEWAY IP KAYDEDİLMEDİ");
							lcd_flip();
							//0.53
                            gtk_clean();
                            //0.61
                            gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                            gtk_printf(label1, "\n\nGATEWAY IP KAYDEDİLMEDİ");
                            while(gtk_events_pending())
                                gtk_main_iteration();
							MSJ_Ses("1212");
							//sleep(1);
                            napms(1000);
						}else
							if(key == KEY_ENTR){
								strcpy(rec_TERM.IP_GATEWAY, ip_control);
								lcd_clean();
								lcd_printf_ex(ALG_CENTER, 5,"GATEWAY IP KAYDEDİLDİ");
								lcd_flip();
								//0.53
                                gtk_clean();
                                //0.61
                                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                                gtk_printf(label1, "\n\nGATEWAY IP KAYDEDİLDİ");
                                while(gtk_events_pending())
                                    gtk_main_iteration();
								MSJ_Ses("1313131");
								//sleep(1);
                                napms(1000);
							}
					//}
				}

				free(cp);
				//selected = -1;
				break;
			case 4://Server IP
				cp = strdup(rec_TERM.IP_SERVER);   //Make writable copy.
				token = strtok (cp, delimiters);   //ilk oktet
				sprintf(ipp, "%03d", (int) strtol(token, NULL, 10));
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 0, "SERVER IP");
				lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_SERVER);
				lcd_printf_ex(ALG_CENTER, 2, "YENİ IP");
				lcd_printf_ex(ALG_CENTER, 3, ipp);
				lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
				lcd_flip();
				//0.53
                gtk_clean();
                //0.61
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                gtk_printf(label1, "SERVER IP\n%s\nYENİ IP\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_SERVER, ipp);
                while(gtk_events_pending())
                    gtk_main_iteration();
				i = 3;
				key = -1;
				while((key != 0XF8) && ((key != KEY_ENTR) ||(i==0))){
					//if(kb_hit()){

						//key = kb_getkey();
						key = KEYB_getch(0);
						if(key == KEY_BKSP){
							if(i>0) i--;
							ipp[i] = 0;
							lcd_clean();
							lcd_printf_ex(ALG_CENTER, 0, "SERVER IP");
							lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_SERVER);
							lcd_printf_ex(ALG_CENTER, 2, "YENİ IP");
							lcd_printf_ex(ALG_CENTER, 3, ipp);
							lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
							lcd_flip();
							//0.53
                            gtk_clean();
                            //0.61
                            gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                            gtk_printf(label1, "SERVER IP\n%s\nYENİ IP\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_SERVER, ipp);
                            while(gtk_events_pending())
                                gtk_main_iteration();
						}
						else
							if(key <= 9 && key >= 0 && i<3){
								ipp[i++] = (char) key+48;
								lcd_clean();
								lcd_printf_ex(ALG_CENTER, 0, "SERVER IP");
								lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_SERVER);
								lcd_printf_ex(ALG_CENTER, 2, "YENİ IP");
								lcd_printf_ex(ALG_CENTER, 3, ipp);
								lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
								lcd_flip();
								//0.53
                                gtk_clean();
                                //0.61
                                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                                gtk_printf(label1, "SERVER IP\n%s\nYENİ IP\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_SERVER, ipp);
                                while(gtk_events_pending())
                                    gtk_main_iteration();
							}
					//}
				}
				if(key == 0XF8){
					lcd_clean();
					lcd_printf_ex(ALG_CENTER, 5, "İPTAL EDİLDİ");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "\n\nİPTAL EDİLDİ");
                    while(gtk_events_pending())
                        gtk_main_iteration();
					//usleep(500000);
					napms(500);
					free(cp);
					break;
				}
				strcpy(new_ip, ipp);
				strcat(new_ip, ".");

				token = strtok (NULL, delimiters); //ikinci oktet
				sprintf(ipp, "%03d", (int) strtol(token, NULL, 10));
				strcat(new_ip, ipp);
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 0, "SERVER IP");
				lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_SERVER);
				lcd_printf_ex(ALG_CENTER, 2, "YENİ IP");
				lcd_printf_ex(ALG_CENTER, 3, new_ip);
				lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
				lcd_flip();
                //0.53
                gtk_clean();
                //0.61
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                gtk_printf(label1, "SERVER IP\n%s\nYENİ IP\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_SERVER, new_ip);
                while(gtk_events_pending())
                    gtk_main_iteration();

				i=strlen(new_ip);
				j = i-3;
				key = -1;
				while((key != 0xF8) && ((key != KEY_ENTR)||(i==j))){
					//if(kb_hit()){

						//key = kb_getkey();
						key = KEYB_getch(0);
						if(key == KEY_BKSP){
							if(i>j) i--;
							new_ip[i] = 0;
							lcd_clean();
                            lcd_printf_ex(ALG_CENTER, 0, "SERVER IP");
                            lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_SERVER);
                            lcd_printf_ex(ALG_CENTER, 2, "YENİ IP");
                            lcd_printf_ex(ALG_CENTER, 3, new_ip);
                            lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
                            lcd_flip();
                            //0.53
                            gtk_clean();
                            //0.61
                            gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                            gtk_printf(label1, "SERVER IP\n%s\nYENİ IP\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_SERVER, new_ip);
                            while(gtk_events_pending())
                                gtk_main_iteration();
						}
						else
							if(key <= 9 && key >= 0 && i<(j+3)){
								new_ip[i++] = (char) key+48;
								lcd_clean();
                                lcd_printf_ex(ALG_CENTER, 0, "SERVER IP");
                                lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_SERVER);
                                lcd_printf_ex(ALG_CENTER, 2, "YENİ IP");
                                lcd_printf_ex(ALG_CENTER, 3, new_ip);
                                lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
                                lcd_flip();
                                //0.53
                                gtk_clean();
                                //0.61
                                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                                gtk_printf(label1, "SERVER IP\n%s\nYENİ IP\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_SERVER, new_ip);
                                while(gtk_events_pending())
                                    gtk_main_iteration();
							}
					//}
				}
				if(key == 0xF8){
					lcd_clean();
					lcd_printf_ex(ALG_CENTER, 5, "İPTAL EDİLDİ");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "\n\nİPTAL EDİLDİ");
                    while(gtk_events_pending())
                        gtk_main_iteration();
					//usleep(500000);
					napms(500);
					free(cp);
					break;
				}
				strcat(new_ip, ".");


				token = strtok (NULL, delimiters); //ikinci oktet
				sprintf(ipp, "%03d", (int) strtol(token, NULL, 10));
				strcat(new_ip, ipp);
				lcd_clean();
                lcd_printf_ex(ALG_CENTER, 0, "SERVER IP");
                lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_SERVER);
                lcd_printf_ex(ALG_CENTER, 2, "YENİ IP");
                lcd_printf_ex(ALG_CENTER, 3, new_ip);
                lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
                lcd_flip();
                //0.53
                gtk_clean();
                //0.61
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                gtk_printf(label1, "SERVER IP\n%s\nYENİ IP\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_SERVER, new_ip);
                while(gtk_events_pending())
                    gtk_main_iteration();
				i = strlen(new_ip);
				j = i-3;
				key = -1;
				while((key != 0xF8) && ((key != KEY_ENTR)||(i==j))){
					//if(kb_hit()){

						//key = kb_getkey();
						key = KEYB_getch(0);
						if(key == KEY_BKSP){
							if(i>j) i--;
							new_ip[i] = 0;
							lcd_clean();
                            lcd_printf_ex(ALG_CENTER, 0, "SERVER IP");
                            lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_SERVER);
                            lcd_printf_ex(ALG_CENTER, 2, "YENİ IP");
                            lcd_printf_ex(ALG_CENTER, 3, new_ip);
                            lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
                            lcd_flip();
                            //0.53
                            gtk_clean();
                            //0.61
                            gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                            gtk_printf(label1, "SERVER IP\n%s\nYENİ IP\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_SERVER, new_ip);
                            while(gtk_events_pending())
                                gtk_main_iteration();
						}
						else
							if(key <= 9 && key >= 0 && i<(j+3)){
								new_ip[i++] = (char) key+48;
								lcd_clean();
                                lcd_printf_ex(ALG_CENTER, 0, "SERVER IP");
                                lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_SERVER);
                                lcd_printf_ex(ALG_CENTER, 2, "YENİ IP");
                                lcd_printf_ex(ALG_CENTER, 3, new_ip);
                                lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
                                lcd_flip();
                                //0.53
                                gtk_clean();
                                //0.61
                                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                                gtk_printf(label1, "SERVER IP\n%s\nYENİ IP\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_SERVER, new_ip);
                                while(gtk_events_pending())
                                    gtk_main_iteration();
							}
					//}
				}
				if(key == 0xF8){
					lcd_clean();
					lcd_flip();
					//0.53
                    gtk_clean();
                    while(gtk_events_pending())
                        gtk_main_iteration();
					//usleep(500000);
					napms(500);
					free(cp);
					break;
				}
				strcat(new_ip, ".");


				token = strtok (NULL, delimiters); //ikinci oktet
				sprintf(ipp, "%03d", (int) strtol(token, NULL, 10));
				strcat(new_ip, ipp);
				lcd_clean();
                lcd_printf_ex(ALG_CENTER, 0, "SERVER IP");
                lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_SERVER);
                lcd_printf_ex(ALG_CENTER, 2, "YENİ IP");
                lcd_printf_ex(ALG_CENTER, 3, new_ip);
                lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
                lcd_flip();
                //0.53
                gtk_clean();
                //0.61
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                gtk_printf(label1, "SERVER IP\n%s\nYENİ IP\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_SERVER, new_ip);
                while(gtk_events_pending())
                    gtk_main_iteration();
				i = strlen(new_ip);
				j = i-3;
				key = -1;
				while((key != 0xF8) && ((key != KEY_ENTR)||(i==j))){
					//if(kb_hit()){

						//key = kb_getkey();
						key = KEYB_getch(0);
						if(key == KEY_BKSP){
							if(i>j) i--;
							new_ip[i] = 0;
							lcd_clean();
                            lcd_printf_ex(ALG_CENTER, 0, "SERVER IP");
                            lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_SERVER);
                            lcd_printf_ex(ALG_CENTER, 2, "YENİ IP");
                            lcd_printf_ex(ALG_CENTER, 3, new_ip);
                            lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
                            lcd_flip();
                            //0.53
                            gtk_clean();
                            //0.61
                            gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                            gtk_printf(label1, "SERVER IP\n%s\nYENİ IP\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_SERVER, new_ip);
                            while(gtk_events_pending())
                                gtk_main_iteration();
						}
						else
							if(key <= 9 && key >= 0 && i<(j+3)){
								new_ip[i++] = (char) key+48;
								lcd_clean();
                                lcd_printf_ex(ALG_CENTER, 0, "SERVER IP");
                                lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_SERVER);
                                lcd_printf_ex(ALG_CENTER, 2, "YENİ IP");
                                lcd_printf_ex(ALG_CENTER, 3, new_ip);
                                lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
                                lcd_flip();
                                //0.53
                                gtk_clean();
                                //0.61
                                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                                gtk_printf(label1, "SERVER IP\n%s\nYENİ IP\n%s\nBKSP: SIL       ENT: KAYDET", rec_TERM.IP_SERVER, new_ip);
                                while(gtk_events_pending())
                                    gtk_main_iteration();
							}
					//}
				}
				if(key == 0xF8){
					lcd_clean();
					lcd_printf_ex(ALG_CENTER, 5, "İPTAL EDİLDİ");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "\n\nİPTAL EDİLDİ");
                    while(gtk_events_pending())
                        gtk_main_iteration();
					//usleep(500000);
					napms(500);
					free(cp);
					break;
				}

				cp = strdup(new_ip);   //Make writable copy.
				token = strtok (cp, delimiters); //first octet
				octet = (int) strtol(token, NULL, 10);
				if((octet >0) && (octet < 256))
					sprintf(ipp, "%d", octet);
				else
				{
					//lcd_set_bk_color(COLOR_RED);
					//lcd_set_font_color(COLOR_YELLOW);
					lcd_clean();
					lcd_flip();
					lcd_printf_ex(ALG_CENTER, 3, "HATALI SERVER IP DEGERİ");
					lcd_printf_ex(ALG_CENTER, 5, "MEVCUT SERVER IP KORUNACAK");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "\nHATALI SERVER IP DEGERİ\nMEVCUT SERVER IP KORUNACAK");
                    while(gtk_events_pending())
                        gtk_main_iteration();
					MSJ_Ses("5343332313");
					//sleep(1);
					napms(1000);
					//lcd_set_bk_color(COLOR_BLUE);
					free(cp);
					break;
				}
				strcpy(ip_control, ipp);
				strcat(ip_control, ".");

				token = strtok (NULL, delimiters);
				octet = (int) strtol(token, NULL, 10);
				if(octet < 256)
					sprintf(ipp, "%d", octet);
				else
				{
					//lcd_set_bk_color(COLOR_RED);
					//lcd_set_font_color(COLOR_YELLOW);
					lcd_clean();
					lcd_flip();
					lcd_printf_ex(ALG_CENTER, 3, "HATALI SERVER IP DEGERİ");
					lcd_printf_ex(ALG_CENTER, 5, "MEVCUT SERVER IP KORUNACAK");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "\nHATALI SERVER IP DEGERİ\nMEVCUT SERVER IP KORUNACAK");
                    while(gtk_events_pending())
                        gtk_main_iteration();
					MSJ_Ses("5343332313");
					//sleep(1);
					napms(1000);
					//lcd_set_bk_color(COLOR_BLUE);
					free(cp);
					break;
				}
				strcat(ip_control, ipp);
				strcat(ip_control, ".");

				token = strtok (NULL, delimiters);
				octet = (int) strtol(token, NULL, 10);
				if(octet < 256)
					sprintf(ipp, "%d", octet);
				else
				{
					//lcd_set_bk_color(COLOR_RED);
					//lcd_set_font_color(COLOR_YELLOW);
					lcd_clean();
					lcd_flip();
					lcd_printf_ex(ALG_CENTER, 3, "HATALI SERVER IP DEGERİ");
					lcd_printf_ex(ALG_CENTER, 5, "MEVCUT SERVER IP KORUNACAK");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "\nHATALI SERVER IP DEGERİ\nMEVCUT SERVER IP KORUNACAK");
                    while(gtk_events_pending())
                        gtk_main_iteration();
					MSJ_Ses("5343332313");
					//sleep(1);
					napms(1000);
					//lcd_set_bk_color(COLOR_BLUE);
					free(cp);
					break;
				}
				strcat(ip_control, ipp);
				strcat(ip_control, ".");

				token = strtok (NULL, delimiters);
				octet = (int) strtol(token, NULL, 10);
				if(octet < 256)
					sprintf(ipp, "%d", octet);
				else
				{
					//lcd_set_bk_color(COLOR_RED);
					//lcd_set_font_color(COLOR_YELLOW);
					lcd_clean();
					lcd_flip();
					lcd_printf_ex(ALG_CENTER, 3, "HATALI SERVER IP DEGERİ");
					lcd_printf_ex(ALG_CENTER, 5, "MEVCUT SERVER IP KORUNACAK");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "\nHATALI SERVER IP DEGERİ\nMEVCUT SERVER IP KORUNACAK");
                    while(gtk_events_pending())
                        gtk_main_iteration();
					MSJ_Ses("5343332313");
					//sleep(1);
					napms(1000);
					//lcd_set_bk_color(COLOR_BLUE);
					free(cp);
					break;
				}
				strcat(ip_control, ipp);
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 0, "SERVER IP");
				lcd_printf_ex(ALG_CENTER, 1, rec_TERM.IP_SERVER);
				lcd_printf_ex(ALG_CENTER, 2, "YENİ SERVER IP");
				lcd_printf_ex(ALG_CENTER, 3, ip_control);
				lcd_printf_ex(ALG_CENTER, 4, "OLARAK KAYDEDİLECEK");
				lcd_printf_ex(ALG_CENTER, 5, "ONAYLIYOR MUSUNUZ?");
				lcd_printf_ex(ALG_CENTER, 6, "BKSP: İPTAL       ENT: KAYDET");
				lcd_flip();
                //0.53
                gtk_clean();
                //0.61
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                gtk_printf(label1, "SERVER IP\n%s\nYENİ SERVER IP\n%s\nOLARAK KAYDEDİLECEK\nONAYLIYOR MUSUNUZ?\nBKSP: İPTAL       ENT: KAYDET", rec_TERM.IP_SERVER, ip_control);
                while(gtk_events_pending())
                    gtk_main_iteration();
				key = -1;
				while((key != KEY_BKSP) && (key != KEY_ENTR)){
					//if(kb_hit()){

						//key = kb_getkey();
						key = KEYB_getch(0);
						if(key == KEY_BKSP){
							lcd_clean();
							lcd_printf_ex(ALG_CENTER, 5,"SERVER IP KAYDEDİLMEDİ");
							lcd_flip();
							//0.53
                            gtk_clean();
                            //0.61
                            gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                            gtk_printf(label1, "\n\nSERVER IP KAYDEDİLMEDİ");
                            while(gtk_events_pending())
                                gtk_main_iteration();
							MSJ_Ses("1212");
							//sleep(1);
                            napms(1000);
						}else
							if(key == KEY_ENTR){
								strcpy(rec_TERM.IP_SERVER, ip_control);
								lcd_clean();
								lcd_printf_ex(ALG_CENTER, 5,"SERVER IP KAYDEDİLDİ");
								lcd_flip();
								//0.53
                                gtk_clean();
                                //0.61
                                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                                gtk_printf(label1, "\n\nSERVER IP KAYDEDİLDİ");
                                while(gtk_events_pending())
                                    gtk_main_iteration();
								MSJ_Ses("1313131");
								//sleep(1);
                                napms(1000);
							}
					//}
				}

				free(cp);
				//selected = -1;
				break;
			case 5://Server Port
				sprintf(newport, "%05d", (int) rec_TERM.PORT_SERVER);
				sprintf(oldport, "%d", (int) rec_TERM.PORT_SERVER);
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 0, "SERVER PORT");
				lcd_printf_ex(ALG_CENTER, 1, oldport);
				lcd_printf_ex(ALG_CENTER, 2, "YENİ PORT");
				lcd_printf_ex(ALG_CENTER, 3, newport);
				lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
				lcd_flip();
                //0.53
                gtk_clean();
                //0.61
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                gtk_printf(label1, "SERVER PORT\n%s\nYENİ PORT\n%s\nBKSP: SIL       ENT: KAYDET", oldport, newport);
                while(gtk_events_pending())
                    gtk_main_iteration();
				i = strlen(newport);
				key = -1;
				while((key != 0xF8) && ((key != KEY_ENTR) ||(i==0))){
					//if(kb_hit()){

						//key = kb_getkey();
						key = KEYB_getch(0);
						if(key == KEY_BKSP){
							if(i>0) i--;
							newport[i] = 0;
							lcd_clean();
                            lcd_printf_ex(ALG_CENTER, 0, "SERVER PORT");
                            lcd_printf_ex(ALG_CENTER, 1, oldport);
                            lcd_printf_ex(ALG_CENTER, 2, "YENİ PORT");
                            lcd_printf_ex(ALG_CENTER, 3, newport);
                            lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
                            lcd_flip();
                             //0.53
                            gtk_clean();
                            //0.61
                            gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                            gtk_printf(label1, "SERVER PORT\n%s\nYENİ PORT\n%s\nBKSP: SIL       ENT: KAYDET", oldport, newport);
                            while(gtk_events_pending())
                                gtk_main_iteration();
						}
						else
							if(key <= 9 && key >= 0 && i<5){
								newport[i++] = (char) key+48;
								lcd_clean();
                                lcd_printf_ex(ALG_CENTER, 0, "SERVER PORT");
                                lcd_printf_ex(ALG_CENTER, 1, oldport);
                                lcd_printf_ex(ALG_CENTER, 2, "YENİ PORT");
                                lcd_printf_ex(ALG_CENTER, 3, newport);
                                lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
                                lcd_flip();
                                 //0.53
                                gtk_clean();
                                //0.61
                                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                                gtk_printf(label1, "SERVER PORT\n%s\nYENİ PORT\n%s\nBKSP: SIL       ENT: KAYDET", oldport, newport);
                                while(gtk_events_pending())
                                    gtk_main_iteration();
							}
					//}

				}
				if(key == 0xF8){
					lcd_clean();
					lcd_printf_ex(ALG_CENTER, 5, "İPTAL EDİLDİ");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "\n\nİPTAL EDİLDİ");
                    while(gtk_events_pending())
                        gtk_main_iteration();
					break;
				}
				new_port = (unsigned long) strtol(newport, NULL,10);
				if ((new_port == 0) || (new_port > 65535)){
					lcd_clean();
					//lcd_set_bk_color(COLOR_RED);
					//lcd_set_font_color(COLOR_YELLOW);
					lcd_flip();
					lcd_printf_ex(ALG_CENTER, 3, "HATALI PORT DEĞERİ");
					lcd_printf_ex(ALG_CENTER, 5, "MEVCUT PORT KORUNACAK");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "\nHATALI PORT DEĞERİ\nMEVCUT PORT KORUNACAK ");
                    while(gtk_events_pending())
                        gtk_main_iteration();
					MSJ_Ses("5343332313");
					//sleep(1);
					napms(1000);
					//lcd_set_bk_color(COLOR_BLUE);
					break;
				}
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 0, "SERVER PORT");
				lcd_printf_ex(ALG_CENTER, 1, oldport);
				lcd_printf_ex(ALG_CENTER, 2, "YENİ PORT");
				sprintf(newport, "%d", (int) new_port);
				lcd_printf_ex(ALG_CENTER, 3, newport);
				lcd_printf_ex(ALG_CENTER, 4, "OLARAK KAYDEDİLECEK");
				lcd_printf_ex(ALG_CENTER, 5, "ONAYLIYOR MUSUNUZ?");
				lcd_printf_ex(ALG_CENTER, 6, "BKSP: İPTAL       ENT: KAYDET");
				lcd_flip();
                //0.53
                gtk_clean();
                //0.61
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                gtk_printf(label1, "SERVER PORT\n%s\nYENİ PORT\n%s\nOLARAK KAYDEDİLECEK\nONAYLIYOR MUSUNUZ?\nBKSP: İPTAL       ENT: KAYDET", oldport, newport);
                while(gtk_events_pending())
                    gtk_main_iteration();
				key = -1;
				while((key != KEY_BKSP) && (key != KEY_ENTR)){
					//if(kb_hit()){

						//key = kb_getkey();
						key = KEYB_getch(0);
						if(key == KEY_BKSP){
							lcd_clean();
							lcd_printf_ex(ALG_CENTER, 5,"SERVER PORT KAYDEDİLMEDİ");
							lcd_flip();
							//0.53
                            gtk_clean();
                            //0.61
                            gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                            gtk_printf(label1, "\n\nSERVER PORT KAYDEDİLMEDİ");
                            while(gtk_events_pending())
                                gtk_main_iteration();
							MSJ_Ses("1212");
							//sleep(1);
                            napms(1000);
						}else
							if(key == KEY_ENTR){
								rec_TERM.PORT_SERVER = (unsigned long) new_port;
								lcd_clean();
								lcd_printf_ex(ALG_CENTER, 5,"SERVER PORT KAYDEDİLDİ");
								lcd_flip();
								//0.53
                                gtk_clean();
                                //0.61
                                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                                gtk_printf(label1, "\n\nSERVER PORT KAYDEDİLDİ");
                                while(gtk_events_pending())
                                    gtk_main_iteration();
								MSJ_Ses("1313131");
								//sleep(1);
                                napms(1000);
							}
					//}
				}
				//selected = -1;
				break;
			//case 5:
			//	key = -1;
			//	break;
			default:
				break;
		}
	}

}

//0.44
void ADMIN_TarihSaat(void)
{
struct tm * timeinfo;
time_t rawtime;//0.1.0


int key, i;
char yil[5], ay[3], gun[3], saat[3], dakika[3];

	time(&rawtime);

	timeinfo = localtime(&rawtime);


	sprintf(yil, "%04d", (timeinfo->tm_year)+1900);
	sprintf(ay, "%02d", timeinfo->tm_mon+1);
	sprintf(gun, "%02d", timeinfo->tm_mday);
	lcd_clean();
	lcd_printf_ex(ALG_CENTER, 0, "TARİH/SAAT");
	lcd_printf_ex(ALG_CENTER, 3, "YIL:     %s", yil);
	lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
	lcd_flip();
    //0.53
    gtk_clean();
    //0.61
    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
    gtk_printf(label1, "TARİH/SAAT\nYIL:     %s\nBKSP: SIL               ENT: KAYDET", yil);
    while(gtk_events_pending())
        gtk_main_iteration();
	i = 4;
	key = -1;
	while((key != 0xF8) && (key != KEY_ENTR)){
		//if(kb_hit()){

			//key = kb_getkey();
			key = KEYB_getch(0);
			if(key == KEY_BKSP){
				if(i>0) i--;
				yil[i] = 0;
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 0, "TARİH/SAAT");
				lcd_printf_ex(ALG_CENTER, 3, "YIL:     %s", yil);
				lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
				lcd_flip();
				//0.53
                gtk_clean();
                //0.61
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                gtk_printf(label1, "TARİH/SAAT\nYIL:     %s\nBKSP: SIL               ENT: KAYDET", yil);
                while(gtk_events_pending())
                    gtk_main_iteration();
			}
			else
				if(key <= 9 && key >= 0 && i<4){
					yil[i++] = (char) key+48;
					lcd_clean();
					lcd_printf_ex(ALG_CENTER, 0, "TARİH/SAAT");
					lcd_printf_ex(ALG_CENTER, 3, "YIL:     %s", yil);
					lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "TARİH/SAAT\nYIL:     %s\nBKSP: SIL               ENT: KAYDET", yil);
                    while(gtk_events_pending())
                        gtk_main_iteration();
				}
		//}
	}
	if(key == 0xF8){
		lcd_clean();
		lcd_printf_ex(ALG_CENTER, 5, "İPTAL EDİLDİ");
		lcd_flip();
		//0.53
        gtk_clean();
        //0.61
        gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
        gtk_printf(label1, "\n\nİPTAL EDİLDİ");
        while(gtk_events_pending())
            gtk_main_iteration();

		return;
	}
	timeinfo->tm_year = (int) (strtol(yil, NULL, 10) - 1900);
	if((strlen(yil) == 4) && (timeinfo->tm_year < 200) && (timeinfo->tm_year > 100)){ //2000-2100 arasi
		lcd_printf_ex(ALG_CENTER, 4, "YENİ YIL: %s", yil);
		lcd_flip();
		//0.53
		gtk_clean();
		//0.61
        gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
        gtk_printf(label1, "TARİH/SAAT\nYENİ YIL:     %s", yil);
        while(gtk_events_pending())
            gtk_main_iteration();
		//usleep(500000);
		napms(500);
	}else
	{
		lcd_printf_ex(ALG_CENTER, 5, "HATALI DEĞER");
		lcd_flip();
		//0.53
        gtk_clean();
        //0.61
        gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
        gtk_printf(label1, "\n\nHATALI DEĞER");
        while(gtk_events_pending())
            gtk_main_iteration();
		//sleep(1);
        napms(1000);;
		return;
	}
	lcd_clean();
	lcd_printf_ex(ALG_CENTER, 0, "TARİH/SAAT");
	lcd_printf_ex(ALG_CENTER, 1, "YIL:     %s", yil);
	lcd_printf_ex(ALG_CENTER, 3, "AY:      %s", ay);
	lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
	lcd_flip();
	//0.53
    gtk_clean();
    //0.61
    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
    gtk_printf(label1, "TARİH/SAAT\nYIL:     %s\nAY:      %s\nBKSP: SIL               ENT: KAYDET", yil, ay);
    while(gtk_events_pending())
        gtk_main_iteration();
	i = 2;
	key = -1;
	while((key != 0xF8) && (key != KEY_ENTR)){
		//if(kb_hit()){

			//key = kb_getkey();
			key = KEYB_getch(0);
			if(key == KEY_BKSP){
				if(i>0) i--;
				ay[i] = 0;
				lcd_clean();
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 0, "TARİH/SAAT");
				lcd_printf_ex(ALG_CENTER, 1, "YIL:     %s", yil);
				lcd_printf_ex(ALG_CENTER, 3, "AY:      %s", ay);
				lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
				lcd_flip();
				//0.53
                gtk_clean();
                //0.61
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                gtk_printf(label1, "TARİH/SAAT\nYIL:     %s\nAY:      %s\nBKSP: SIL               ENT: KAYDET", yil, ay);
                while(gtk_events_pending())
                    gtk_main_iteration();
			}
			else
				if(key <= 9 && key >= 0 && i<2){
					ay[i++] = (char) key+48;
					lcd_clean();
					lcd_printf_ex(ALG_CENTER, 0, "TARİH/SAAT");
					lcd_printf_ex(ALG_CENTER, 1, "YIL:     %s", yil);
					lcd_printf_ex(ALG_CENTER, 3, "AY:      %s", ay);
					lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "TARİH/SAAT\nYIL:     %s\nAY:      %s\nBKSP: SIL               ENT: KAYDET", yil, ay);
                    while(gtk_events_pending())
                        gtk_main_iteration();
				}
		//}
	}
	if(key == 0xF8){
		lcd_clean();
		lcd_printf_ex(ALG_CENTER, 5, "İPTAL EDİLDİ");
		lcd_flip();
		//0.53
        gtk_clean();
        //0.61
        gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
        gtk_printf(label1, "\n\nİPTAL EDİLDİ");
        while(gtk_events_pending())
            gtk_main_iteration();
		return;
	}
	timeinfo->tm_mon = (int) (strtol(ay, NULL, 10) -1);
	sprintf(ay, "%02d", timeinfo->tm_mon+1);
	if((timeinfo->tm_mon < 12) && (timeinfo->tm_mon > -1)){//1-12 ay arası
		lcd_printf_ex(ALG_CENTER, 4, "YENİ AY: %s", ay);
		lcd_flip();
		//0.53
		gtk_clean();
		//0.61
        gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
        gtk_printf(label1, "TARİH/SAAT\nYENİ AY:     %s", ay);
        while(gtk_events_pending())
            gtk_main_iteration();
		//usleep(500000);
		napms(500);
	}else
	{
		lcd_printf_ex(ALG_CENTER, 5, "HATALI DEGER");
		lcd_flip();
		//0.53
        gtk_clean();
        //0.61
        gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
        gtk_printf(label1, "\n\nHATALI DEGER");
        while(gtk_events_pending())
            gtk_main_iteration();
		//sleep(1);
        napms(1000);
		return;
	}

	lcd_clean();
	lcd_printf_ex(ALG_CENTER, 0, "TARİH/SAAT");
	lcd_printf_ex(ALG_CENTER, 1, "YIL:     %s", yil);
	lcd_printf_ex(ALG_CENTER, 2, "AY:      %s", ay);
	lcd_printf_ex(ALG_CENTER, 3, "GUN:  %s", gun);
	lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
	lcd_flip();
	//0.53
    gtk_clean();
    //0.61
    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
    gtk_printf(label1, "TARİH/SAAT\nYIL:     %s\nAY:      %s\nGUN:  %s\nBKSP: SIL               ENT: KAYDET", yil, ay, gun);
    while(gtk_events_pending())
        gtk_main_iteration();
	i = 2;
	key = -1;
	while((key != 0xF8) && (key != KEY_ENTR)){
		//if(kb_hit()){

			//key = kb_getkey();
			key = KEYB_getch(0);
			if(key == KEY_BKSP){
				if(i>0) i--;
				gun[i] = 0;
				lcd_clean();
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 0, "TARİH/SAAT");
				lcd_printf_ex(ALG_CENTER, 1, "YIL:     %s", yil);
				lcd_printf_ex(ALG_CENTER, 2, "AY:      %s", ay);
				lcd_printf_ex(ALG_CENTER, 3, "GUN:  %s", gun);
				lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
				lcd_flip();
				//0.53
                gtk_clean();
                //0.61
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                gtk_printf(label1, "TARİH/SAAT\nYIL:     %s\nAY:      %s\nGUN:  %s\nBKSP: SIL               ENT: KAYDET", yil, ay, gun);
                while(gtk_events_pending())
                    gtk_main_iteration();
			}
			else
				if(key <= 9 && key >= 0 && i<2){
					gun[i++] = (char) key+48;
					lcd_clean();
					lcd_printf_ex(ALG_CENTER, 0, "TARİH/SAAT");
					lcd_printf_ex(ALG_CENTER, 1, "YIL:     %s", yil);
					lcd_printf_ex(ALG_CENTER, 2, "AY:      %s", ay);
					lcd_printf_ex(ALG_CENTER, 3, "GUN:  %s", gun);
					lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "TARİH/SAAT\nYIL:     %s\nAY:      %s\nGUN:  %s\nBKSP: SIL               ENT: KAYDET", yil, ay, gun);
                    while(gtk_events_pending())
                        gtk_main_iteration();
				}
		//}
	}
	if(key == 0xF8){
		lcd_clean();
		lcd_printf_ex(ALG_CENTER, 5, "İPTAL EDİLDİ");
		lcd_flip();
		//0.53
        gtk_clean();
        //0.61
        gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
        gtk_printf(label1, "\n\nİPTAL EDİLDİ");
        while(gtk_events_pending())
            gtk_main_iteration();
		return;
	}
	timeinfo->tm_mday = (int)(strtol(gun, NULL, 10));
	sprintf(gun, "%02d", timeinfo->tm_mday);
	if((timeinfo->tm_mday < 32) && (timeinfo->tm_mday > 0)){
		lcd_printf_ex(ALG_CENTER, 4, "YENİ GÜN: %s", gun);
		lcd_flip();
		//0.53
		gtk_clean();
		//0.61
        gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
        gtk_printf(label1, "TARİH/SAAT\nYENİ GÜN:     %s", gun);
        while(gtk_events_pending())
            gtk_main_iteration();
		//usleep(500000);
		napms(500);
	}else
	{
		lcd_printf_ex(ALG_CENTER, 5, "HATALI DEĞER");
		lcd_flip();
		//0.53
        gtk_clean();
        //0.61
        gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
        gtk_printf(label1, "\n\nHATALI DEĞER");
        while(gtk_events_pending())
            gtk_main_iteration();
		//sleep(1);
        napms(1000);
		return;
	}

	lcd_clean();
	lcd_printf_ex(ALG_CENTER, 0, "TARİH");
	lcd_printf_ex(ALG_CENTER, 1, "%s/%s/%s", gun, ay, yil);
	lcd_printf_ex(ALG_CENTER, 2, "OLARAK KAYDEDİLECEK");
	lcd_printf_ex(ALG_CENTER, 3, "ONAYLIYOR MUSUNUZ?");
	lcd_printf_ex(ALG_CENTER, 5, "BKSP: İPTAL       ENT: KAYDET");
	lcd_flip();
    //0.53
    gtk_clean();
    //0.61
    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
    gtk_printf(label1, "TARİH\n%s/%s/%s\nOLARAK KAYDEDİLECEK\nONAYLIYOR MUSUNUZ?\nBKSP: İPTAL               ENT: KAYDET", gun, ay, yil);
    while(gtk_events_pending())
        gtk_main_iteration();
	key = -1;
	while((key != KEY_BKSP) && (key != KEY_ENTR)){
		//if(kb_hit()){

			//key = kb_getkey();
			key = KEYB_getch(0);
			if(key == KEY_BKSP){
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 5,"TARİH KAYDEDİLMEDİ");
				lcd_flip();
				//0.53
                gtk_clean();
                //0.61
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                gtk_printf(label1, "\n\nTARİH KAYDEDİLMEDİ");
                while(gtk_events_pending())
                    gtk_main_iteration();
				MSJ_Ses("1212");
				//sleep(1);
                napms(1000);
			}else
				if(key == KEY_ENTR){
                    //odroid
					//if(!sys_set_time(timeinfo)){
					rawtime = mktime ( timeinfo );
                    if(!stime(&rawtime)){
					//if(napms(100)){
						lcd_clean();
						lcd_printf_ex(ALG_CENTER, 5,"TARİH KAYDEDİLDİ");
						lcd_flip();
						//0.53
                        gtk_clean();
                        //0.61
                        gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                        gtk_printf(label1, "\n\nTARİH KAYDEDİLDİ");
                        while(gtk_events_pending())
                            gtk_main_iteration();
						MSJ_Ses("1313131");
					}else{
						lcd_clean();
						lcd_printf_ex(ALG_CENTER, 5,"TARİH KAYDEDİLEMEDİ");
						lcd_flip();
						//0.53
                        gtk_clean();
                        //0.61
                        gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                        gtk_printf(label1, "\n\nTARİH KAYDEDİLEMEDİ");
                        while(gtk_events_pending())
                            gtk_main_iteration();
						MSJ_Ses("1212");
					}
					//sleep(1);
					napms(1000);
				}
		//}
	}
	if(key == 0xF8){
		lcd_clean();
		lcd_printf_ex(ALG_CENTER, 55, "İPTAL EDİLDİ");
		lcd_flip();
		//0.53
        gtk_clean();
        //0.61
        gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
        gtk_printf(label1, "\n\nİPTAL EDİLDİ");
        while(gtk_events_pending())
            gtk_main_iteration();

		return;
	}

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	sprintf(saat, "%02d", timeinfo->tm_hour);
	sprintf(dakika, "%02d", timeinfo->tm_min);
	lcd_clean();
	lcd_printf_ex(ALG_CENTER, 0, "TARİH/SAAT");
	lcd_printf_ex(ALG_CENTER, 1, "SAAT:         %s", saat);
	lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
	lcd_flip();
    //0.53
    gtk_clean();
    //0.61
    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
    gtk_printf(label1, "TARİH/SAAT\nSAAT:         %s\nBKSP: SIL               ENT: KAYDET", saat);
    while(gtk_events_pending())
        gtk_main_iteration();
	i = 2;
	key = -1;
	while((key != 0xF8) && (key != KEY_ENTR)){
		//if(kb_hit()){

			//key = kb_getkey();
			key = KEYB_getch(0);
			if(key == KEY_BKSP){
				if(i>0) i--;
				saat[i] = 0;
				lcd_clean();
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 0, "TARİH/SAAT");
				lcd_printf_ex(ALG_CENTER, 1, "SAAT:         %s", saat);
				lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
				lcd_flip();
				//0.53
                gtk_clean();
                //0.61
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                gtk_printf(label1, "TARİH/SAAT\nSAAT:         %s\nBKSP: SIL               ENT: KAYDET", saat);
                while(gtk_events_pending())
                    gtk_main_iteration();
			}
			else
				if(key <= 9 && key >= 0 && i<4){
					saat[i++] = (char) key+48;
					//lcd_clean(0, 50, 24, 20);
					lcd_clean();
					lcd_printf_ex(ALG_CENTER, 0, "TARİH/SAAT");
					lcd_printf_ex(ALG_CENTER, 1, "SAAT:         %s", saat);
					lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "TARİH/SAAT\nSAAT:         %s\nBKSP: SIL               ENT: KAYDET", saat);
                    while(gtk_events_pending())
                        gtk_main_iteration();
				}
		//}
	}
	if(key == 0xF8){
		lcd_clean();
		lcd_printf_ex(ALG_CENTER, 5, "İPTAL EDİLDİ");
		lcd_flip();
		//0.53
        gtk_clean();
        //0.61
        gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
        gtk_printf(label1, "\n\nİPTAL EDİLDİ");
        while(gtk_events_pending())
            gtk_main_iteration();
		return;
	}
	timeinfo->tm_hour = (int)(strtol(saat, NULL, 10));
	sprintf(saat, "%02d", timeinfo->tm_hour);
	if(timeinfo->tm_hour < 24){
		lcd_printf_ex(ALG_CENTER, 3, "YENİ SAAT: %s", saat);
		lcd_flip();
		//0.53
		gtk_clean();
		//0.61
        gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
        gtk_printf(label1, "TARİH/SAAT\nYENİ SAAT:     %s", saat);
        while(gtk_events_pending())
            gtk_main_iteration();
		//usleep(500000);
		napms(500);
	}else
	{
		lcd_printf_ex(ALG_CENTER, 5, "HATALI DEĞER");
		lcd_flip();
		//0.53
        gtk_clean();
        //0.61
        gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
        gtk_printf(label1, "\n\nHATALI DEĞER");
        while(gtk_events_pending())
            gtk_main_iteration();
		//sleep(1);
        napms(1000);
		return;
	}

	lcd_clean();
	lcd_printf_ex(ALG_CENTER, 0, "TARİH/SAAT");
	lcd_printf_ex(ALG_CENTER, 1, "SAAT:         %s", saat);
	lcd_printf_ex(ALG_CENTER, 2, "DAKİKA:     %s", dakika);
	lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
	lcd_flip();
	//0.53
    gtk_clean();
    //0.61
    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
    gtk_printf(label1, "TARİH/SAAT\nSAAT:         %s\nDAKİKA:     %s\nBKSP: SIL               ENT: KAYDET", saat, dakika);
    while(gtk_events_pending())
        gtk_main_iteration();
	i = 2;
	key = -1;
	while((key != 0xF8) && (key != KEY_ENTR)){
		//if(kb_hit()){

			//key = kb_getkey();
			key = KEYB_getch(0);
			if(key == KEY_BKSP){
				if(i>0) i--;
				dakika[i] = 0;
				lcd_clean();
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 0, "TARİH/SAAT");
				lcd_printf_ex(ALG_CENTER, 1, "SAAT:         %s", saat);
				lcd_printf_ex(ALG_CENTER, 2, "DAKİKA:     %s", dakika);
				lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
				lcd_flip();
				//0.53
                gtk_clean();
                //0.61
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                gtk_printf(label1, "TARİH/SAAT\nSAAT:         %s\nDAKİKA:     %s\nBKSP: SIL               ENT: KAYDET", saat, dakika);
                while(gtk_events_pending())
                    gtk_main_iteration();
			}
			else
				if(key <= 9 && key >= 0 && i<2){
					dakika[i++] = (char) key+48;
					//lcd_clean(0, 50, 24, 20);
					lcd_clean();
					lcd_printf_ex(ALG_CENTER, 0, "TARİH/SAAT");
					lcd_printf_ex(ALG_CENTER, 1, "SAAT:         %s", saat);
					lcd_printf_ex(ALG_CENTER, 2, "DAKİKA:     %s", dakika);
					lcd_printf_ex(ALG_CENTER, 5, "BKSP: SIL               ENT: KAYDET");
					lcd_flip();
					//0.53
                    gtk_clean();
                    //0.61
                    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                    gtk_printf(label1, "TARİH/SAAT\nSAAT:         %s\nDAKİKA:     %s\nBKSP: SIL               ENT: KAYDET", saat, dakika);
                    while(gtk_events_pending())
                        gtk_main_iteration();
				}
		//}
	}
	if(key == 0xF8){
		lcd_clean();
		lcd_printf_ex(ALG_CENTER, 5, "İPTAL EDİLDİ");
		lcd_flip();
		//0.53
        gtk_clean();
        //0.61
        gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
        gtk_printf(label1, "\n\nİPTAL EDİLDİ");
        while(gtk_events_pending())
            gtk_main_iteration();
		return;
	}
	timeinfo->tm_min = (int)(strtol(dakika, NULL, 10));
	sprintf(dakika, "%02d", timeinfo->tm_min);
	if(timeinfo->tm_min < 60){
		lcd_printf_ex(ALG_CENTER, 3, "YENİ DAKİKA: %s", dakika);
		lcd_flip();
		//0.53
        gtk_clean();
        //0.61
        gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
        gtk_printf(label1, "TARİH/SAAT\nYENİ DAKİKA:     %s", dakika);
        while(gtk_events_pending())
            gtk_main_iteration();
		//usleep(500000);
		napms(500);
	}else
	{
		lcd_printf_ex(ALG_CENTER, 5, "HATALI DEĞER");
		lcd_flip();
		//0.53
        gtk_clean();
        //0.61
        gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
        gtk_printf(label1, "\n\nHATALI DEĞER");
        while(gtk_events_pending())
            gtk_main_iteration();
		//sleep(1);
        napms(1000);
		return;
	}

	lcd_clean();
	lcd_printf_ex(ALG_CENTER, 0, "SAAT");
	lcd_printf_ex(ALG_CENTER, 1, "%s:%s", saat, dakika);
	lcd_printf_ex(ALG_CENTER, 2, "OLARAK KAYDEDİLECEK");
	lcd_printf_ex(ALG_CENTER, 3, "ONAYLIYOR MUSUNUZ?");
	lcd_printf_ex(ALG_CENTER, 5, "BKSP: İPTAL       ENTER: KAYDET");
	lcd_flip();
    //0.53
    gtk_clean();
    //0.61
    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
    gtk_printf(label1, "SAAT\n%s:%s\nOLARAK KAYDEDİLECEK\nONAYLIYOR MUSUNUZ?\nBKSP: İPTAL       ENTER: KAYDET", saat, dakika);
    while(gtk_events_pending())
        gtk_main_iteration();
	key = -1;
	while((key != KEY_BKSP) && (key != KEY_ENTR)){
		//if(kb_hit()){

			//key = kb_getkey();
			key = KEYB_getch(0);
			if(key == KEY_BKSP){
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 5,"SAAT KAYDEDİLMEDİ");
				lcd_flip();
				//0.53
                gtk_clean();
                //0.61
                gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                gtk_printf(label1, "\n\nSAAT KAYDEDİLMEDİ");
                while(gtk_events_pending())
                    gtk_main_iteration();
				MSJ_Ses("1212");
				//sleep(1);
                napms(1000);
			}else
				if(key == KEY_ENTR){
					timeinfo->tm_sec = 0;
					//odroid
					//if(!sys_set_time(timeinfo)){
					//odroid
					//if(!stime(timeinfo)){
					rawtime = mktime ( timeinfo );
					if(!stime(&rawtime)){
						lcd_clean();
						lcd_printf_ex(ALG_CENTER, 5,"SAAT KAYDEDİLDİ");
						lcd_flip();
						//0.53
                        gtk_clean();
                        //0.61
                        gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                        gtk_printf(label1, "\n\nSAAT KAYDEDİLDİ");
                        while(gtk_events_pending())
                            gtk_main_iteration();
						MSJ_Ses("1313131");
					}else{
						lcd_clean();
						lcd_printf_ex(ALG_CENTER, 5,"SAAT KAYDEDİLEMEDİ");
						lcd_flip();
						//0.53
                        gtk_clean();
                        //0.61
                        gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
                        gtk_printf(label1, "\n\nSAAT KAYDEDİLEMEDİ");
                        while(gtk_events_pending())
                            gtk_main_iteration();
						MSJ_Ses("1212");
					}
					//sleep(1);
					napms(1000);
				}
		//}
	}
	if(key == 0xF8){
		lcd_clean();
		lcd_printf_ex(ALG_CENTER, 5, "İPTAL EDİLDİ");
		lcd_flip();
		//0.53
        gtk_clean();
        //0.61
        gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
        gtk_printf(label1, "\n\nİPTAL EDİLDİ");
        while(gtk_events_pending())
            gtk_main_iteration();

		return;
	}
}
/*
void ADMIN_SifreDegis(void)
{
	int key, i;
	lcd_clean();
	lcd_printf_ex(ALG_CENTER, 20,"ŞİFRE DEĞİŞTİR");
	lcd_printf_ex(ALG_CENTER, 55, "YENİ ŞİFRE");
	lcd_flip();

	i = 0;
	key = -1;
	while((key != 0x1B) && (key != KEY_ENTR)){
		//if(kb_hit()){

			//key = kb_getkey();
			key = getch();
			if(key == KEY_BKSP){
				if(i>0) i--;
				new_sifre[i] = 0;
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 20,"ŞİFRE DEĞİŞTİR");
				lcd_printf_ex(ALG_CENTER, 55, "YENİ ŞİFRE");
				lcd_printf_ex(ALG_CENTER, 100,  new_sifre);
				lcd_flip();
			}
			else
				if(key <= 0x39 && key >= 0x30 && i<4){
					new_sifre[i++] = (char) key;
					lcd_clean();
					lcd_printf_ex(ALG_CENTER, 20,"ŞİFRE DEĞİŞTİR");
					lcd_printf_ex(ALG_CENTER, 55, "YENİ ŞİFRE");
					lcd_printf_ex(ALG_CENTER, 100,  new_sifre);
					lcd_flip();
				}
		//}

	}
	if(key == 0x1B){
		lcd_clean();
		lcd_printf_ex(ALG_CENTER, 45, "İPTAL EDİLDİ");
		lcd_flip();
		return;
	}

	if (strlen(new_sifre)!=4)
	{
		lcd_printf_ex(ALG_CENTER, 155, "DEĞER HATALI");
		lcd_flip();
		//sleep(1);
        napms(1000);
	}

	lcd_clean();
	lcd_printf_ex(ALG_CENTER, 55, "YENİ ŞİFRE");
	lcd_printf_ex(ALG_CENTER, 80, new_sifre);
	lcd_printf_ex(ALG_CENTER, 110, "OLARAK KAYDEDİLECEK");
	lcd_printf_ex(ALG_CENTER, 130, "ONAYLIYOR MUSUNUZ?");
	lcd_printf_ex(ALG_CENTER, 210, "CANCEL: İPTAL       ENTER: KAYDET");
	lcd_flip();

	key = -1;
	while((key != 0x1B) && (key != KEY_ENTR)){
		//if(kb_hit()){

			//key = kb_getkey();
			key = getch();
			if(key == 0x1B){
				lcd_clean();
				lcd_printf_ex(ALG_CENTER, 25,"ŞİFRE KAYDEDİLMEDİ");
				lcd_flip();
				MSJ_Ses("1212");
				//sleep(1);
                napms(1000);
			}else
				if(key == KEY_ENTR){
					memcpy(rec_TERM.ADMIN_PASS,new_sifre,strlen(new_sifre));
					lcd_clean();
					lcd_printf_ex(ALG_CENTER, 25,"ŞİFRE KAYDEDİLDİ");
					lcd_flip();
					MSJ_Ses("1313131");
					//sleep(1);
					napms(1000);
				}
		//}
	}
}
*/

void ADMIN_Menu(void)
{
	int key;

	/*char *m[5];

	for (i=0;i<5;i++)
	{
		m[i]=(char*) malloc(256);
		sprintf(m[i],"%s",RESOURCE_ReadLine(1000+i));
	}
*/
	while(key!=0xFA)
	{
		key = ADMIN_ShowMenu("YONETICI MENUSU",//m[0],//"YONETICI MENUSU",
                             "1-BAGLANTI AYARI",//m[1],//"1-BAGLANTI AYARI",
                             "2-TARIH SAAT",//m[2],//"2-TARIH SAAT",
                             "3-SIFRE DEGISTIR",//m[3],//"3-SIFRE DEGISTIR",
                             "",//m[4],//"4-ISBANK MASTER KART",
                             "",//"",
                             "",//"",
                             "");//"");
		switch (key)
		{
			case 1:
				ADMIN_TcpIP();
				break;
			case 2:
				ADMIN_TarihSaat();
				break;
			case 3:
				ADMIN_SifreDegis();
				break;
			case 4:
				break;
		}
	}
	//for (i=0;i<5;i++)	free(m[i]);
}

/*
void ADMIN_Menu(void)
{
	int selected;
	selected = 0;

	while (selected>=0){
		selected = lcd_menu("YÖNETİCİ MENÜSÜ", admin_menu, sizeof(admin_menu)/100, selected);
		switch (selected){
			case 0:
				ADMIN_TcpIP();
				//selected = -1;
				break;
			case 1:
				ADMIN_TarihSaat();
				//selected = -1;
				break;
			case 2:
				ADMIN_SifreDegis();
				//selected = -1;
				break;
			case 3:
				selected = -1;
				break;
			default:
				break;
		}
	}
}
*/
/*
char ADMIN_Giris(void)
{
	int key, i;
	time_t rawtime;//0.1.0
	struct tm * timeinfo;
	char sfr[10];
//odroid
//buraya bakılmalı nasıl giriş yapılacak diye
//return 0;
	memset(sfr, 0, 10);
	//key=get_keyms(5000);
	//if(kb_hit()){
		//key = kb_getkey();
		//key = getch();
		//key = KEYB_pressup();
		key = KEYB_pressup();
		//if (key==DIKS_ALT)//menu key
		//if (key==DIKS_ALT)//menu key
		if(key>-1){
            lcd_clean();
            lcd_printf_ex(ALG_CENTER, 4, "key: %d", key);
            lcd_flip();
		}
		if (key==0xF5)//F5 key
		{
			lcd_clean();
			lcd_printf_ex(ALG_CENTER, 5, "SİFRE GİRİNİZ");
			lcd_flip();
			i = 0;
			key = -1;
			while((key != 0) && (key != 0x1B) && (key != KEY_ENTR) && (i<4)){
				//if(kb_hit()){

					//key = kb_getkey();
					key = get_keyms(3000);
					if(key == KEY_BKSP){
						if(i>0) i--;
						sifre[i] = 0;
						sfr[i] = 0;
						lcd_clean();
						lcd_printf_ex(ALG_CENTER, 5, "ŞİFRE GİRİNİZ");
						lcd_printf_ex(ALG_CENTER, 5,  sfr);
						lcd_flip();
					}
					else
						if(key <= 0x39 && key >= 0x30){
							sifre[i] = (char) key;
							sfr[i++] = '*';
							//lcd_clean(0, 50, 24, 20);
							lcd_clean();
							lcd_printf_ex(ALG_CENTER, 5, "ŞİFRE GİRİNİZ");
							lcd_printf_ex(ALG_CENTER, 5,  sfr);
							lcd_flip();
						}
				//}

			}
			if((key == 0x1B) || (key == 0)){
				memset(sifre,0x00,4);
				time(&rawtime);
				timeinfo = localtime(&rawtime);
				lcd_clean();
				MSJ_Goster(MSJ_TIP_LG);
				lcd_printf_ex(ALG_CENTER, 16," %02d/%02d/%04d, %02d:%02d", timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min); //Her Dakika Zaman Goster
				lcd_printf_ex(ALG_CENTER, 17, "www.utarit.com ©2014");
				lcd_flip();
				return 0;
			}

			if (strncmp(sifre, rec_TERM.ADMIN_PASS,4)==0){
				memset(sifre,0x00,4);
				ADMIN_Menu();
				CONFIG_Save();
				//free(sz);
				return 1;
			}else{
				lcd_printf(ALG_CENTER, "HATALI ŞİFRE");
				lcd_flip();
				MSJ_Ses(rec_TERM.SES_TANIMSIZ);
				//sleep(1);
                napms(1000);
			}
			memset(sifre,0x00,4);
			time(&rawtime);
			timeinfo = localtime(&rawtime);
			lcd_clean();
			MSJ_Goster(MSJ_TIP_LG);
			lcd_printf_ex(ALG_CENTER, 16," %02d/%02d/%04d, %02d:%02d", timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min); //Her Dakika Zaman Goster
			lcd_printf_ex(ALG_CENTER, 17, "www.utarit.com ©2014");
			lcd_flip();
		}
	//}
	return 0;
}
*/
char ADMIN_Giris(void)
{
	int key;
    char *sz;

	sz = (char *) malloc(50);

	key=KEYB_pressup();
    /*if(key > -1){
        lcd_printf_ex(ALG_CENTER, 7, "Key: %d", key);
        lcd_flip();
    }*/
	if (key==KEY_BKSP)
	{
		memset(aPass,0x00,4);
		aPos=0;
		free(sz);
		return 0;
	} else
	if ((key>=0)&&(key<=9))
	{
		aPass[aPos]=key+48;
		//printf("%s\r\n",aPass);
		aPos++;
		if (aPos>3) aPos=0;
		if (strncmp(aPass,rec_TERM.ADMIN_PASS,4)==0)
		{
			aPos=0;
			memset(aPass,0x00,4);
			MSJ_Ses("314231");
			//WDT_Disable();
			ADMIN_Menu();
			CONFIG_Save();
			free(sz);
			return 1;
		}
	} else
	if (key==0xF8)
	{
		lcd_clean();
		lcd_printf_ex(ALG_CENTER, 0, "TCP/IP AYARLARI");//RESOURCE_ReadLine(1200));
		lcd_printf_ex(ALG_CENTER, 2,rec_TERM.IP_TERM);
		lcd_printf_ex(ALG_CENTER, 3,rec_TERM.IP_SUBNET);
		lcd_printf_ex(ALG_CENTER, 4,rec_TERM.IP_GATEWAY);
		memset(sz,0x00,50);
		sprintf(sz,"%s:%d",rec_TERM.IP_SERVER,(int)rec_TERM.PORT_SERVER);
		lcd_printf_ex(ALG_CENTER, 5,sz);
		lcd_flip();
		//0.50
		gtk_clean();
        //0.60
        //gtk_layout_move(GTK_LAYOUT(main_layout), logo, 50, 5);
        gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);
        //0.51
        GdkRGBA font_color;
        font_color.alpha = 1;
        font_color.blue = 1;
        font_color.green = 0;
        font_color.red = 0;
        gtk_widget_override_color(label1, GTK_STATE_FLAG_NORMAL, &font_color);
        //0.60
        //gtk_widget_override_font(label1, pango_font_description_from_string("Droid Sans Bold 12"));//Ubuntu Medium Italic 12
        gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12

        gtk_printf(label1, "%s\n\nTCP/IP AYARLARI\n  LOCAL IP: %s\n    SUBNET: %s\n GATEWAY: %s\nSERVER IP:PORT: %s",PROG_VER, rec_TERM.IP_TERM,rec_TERM.IP_SUBNET,rec_TERM.IP_GATEWAY, sz);
        //gtk_printf(label2,"LOCAL IP: %s", rec_TERM.IP_TERM);
		//gtk_printf(label3,"SUBNET: %s", rec_TERM.IP_SUBNET);
		//gtk_printf(label4,"GATEWAY: %s",rec_TERM.IP_GATEWAY);
		//gtk_printf(label5,"SERVER IP:PORT: %s", sz);
		while(gtk_events_pending())
            gtk_main_iteration();

		MSJ_Ses("425221");
		napms(1000);
		MSJ_OK=0;
	}
	free(sz);
	return 0;
}

