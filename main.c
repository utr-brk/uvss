/*
 ============================================================================
 Name        : m8110.c
 Author      : Burak TUZUN <tuzun@utarit.com>
 Version     :
 Copyright   : Copyright UTARIT (c) 2014
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
 //1.4.1
 //asprintf uyarısını engellemek için
#define _GNU_SOURCE

#include <gtk/gtk.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>


#include <sys/ioctl.h>
#include <sys/types.h>
#include <net/if.h>
#include <netdb.h>
#include "lcd.h"
#include "tcp.h"
#include "config.h"
#include "zamanis.h"
#include "main.h"
#include "personel.h"
#include "log.h"
#include "mesaj.h"
#include "ftp.h"
#include "reader.h"
#include "sayac.h"
#include "timer.h"
#include "admin.h"
#include "com.h"
#include "net.h"
#include "keyb.h"
#include <wiringPi.h>
#include <softTone.h>
//#include <pthread.h>
#include <time.h>
#include <signal.h>
//2.0.0
#include "cn56b.h"

char HTTP_RESET;

volatile sig_atomic_t sig_flag = 0;

//2.2.5
int DATE_TIME_OK = 0;

GtkWidget *window;
GtkWidget *main_layout;
GtkWidget *label1;
GtkWidget *label_status;
GtkWidget *logo;
GtkWidget *infobarr;
GtkWidget *content_area;
GtkWidget *txtBarcode;


gint window_width, window_height;
gint logo_x, logo_y;
gint logo_x_top, logo_y_top;
gint label_x, label_y;
gint label_width, label_height;
gint infobar_x, infobar_y;

char *small_font_name;//[] = "Droid Sans Bold 11";
char *big_font_name;//[] = "Droid Sans Bold 12";


int outputs[8] = {26, 27, 7, 21, 22, 23, 24, 13};
int inputs[6] = {1, 4, 5, 6, 0, 2};

uint32_t aPower(unsigned long X,unsigned long Y)
{
	if (Y==0)
	{
		return 1;
	} else {
		return X * aPower(X,Y-1);
	}
}

uint32_t aHexToLong(char *Hex)
{
	char HEX_TABLE[17]="0123456789ABCDEF";
	uint32_t r=0;
	uint16_t i,l;
	l=strlen(Hex);
	i=l;
	while (i>0)
	{
		r = r + aPower(16,l-i) * (strchr(HEX_TABLE,Hex[i-1])-HEX_TABLE);
		i--;
	}
	return r;
}

uint16_t aHexToString(unsigned char *strHex, unsigned char *strByte)
{
	uint16_t L,i,j;
	char * Hex;

	Hex = (char*) calloc(3, 1);
	L=(int) strlen((const char *)strHex);
	i=0;
	j=0;

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

uint16_t aStringToHex(unsigned char *strByte,unsigned int Len, unsigned char *strHex)//m8110 unsigned char idiler
{
	uint16_t i;
	strHex[0]=0x00;
	for (i=0;i<Len;i++)
		sprintf((char *)strHex+(i*2),"%02X",strByte[i]);
	return i;
}


int main_TS_Guncelle(void)
{
	struct_date dd;
	struct_time tt;
	struct tm * timeinfo;
	time_t rawtime;
	int ret = 0x0;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	mktime ( timeinfo );

	if (TCP_GetDateTime(&dd,&tt)==0x01)
	{
		timeinfo->tm_year = (int) (dd.da_year - 1900);
		timeinfo->tm_mon = (int) (dd.da_mon - 1);
		timeinfo->tm_mday = (int) (dd.da_day);
		timeinfo->tm_hour = (int) (tt.ti_hour);
		timeinfo->tm_min = (int) (tt.ti_min);
		timeinfo->tm_sec = (int)(tt.ti_sec);
		rawtime = mktime ( timeinfo );

		if(stime(&rawtime))
			printf("error setting date-time: %s\n", strerror(errno));
		else{
			lcd_printf(ALG_CENTER, "Date-Time Set: %02d/%02d/%04d, %02d:%02d:%02d", dd.da_day, dd.da_mon, dd.da_year, tt.ti_hour, tt.ti_min, tt.ti_sec);
			lcd_flip();

            //2.3.1
            //tarih/saat değişti, zamanlirole'yi yeniden başlat
            system("sudo pkill -9 -f zamanlirole");
            system("/home/odroid/Solidus/zamanlirole&");

			ret = 0x1;
		}

	}
	return ret;
}

//to exit properly when CTRL+C is pressed.
void INThandler(int sig)
{
    signal(sig, SIG_IGN);
    sig_flag = 1;
}

int main_solidus(void) {

int i;
//2.2.5
//unsigned long long timer1dk;
struct timeval timer1dk;
unsigned long syc_dk=0;

//1.6.2
int FTP_SGNL;

//2.0.0
unsigned char SerNo[8];

    signal(SIGINT, INThandler);

    while(gtk_events_pending())
        gtk_main_iteration();

	lcd_init();
	lcd_clean();
	lcd_printf(ALG_CENTER, "mercure EMV POS");

	memset(&Hata,0,sizeof(Hata));
	memset (&TopUP_ERR, 0, sizeof(TopUP_ERR));
    lcd_printf(ALG_CENTER, "mercure supper POS");

    if(wiringPiSetup() == -1){
        lcd_printf_ex(ALG_CENTER, 6, "wiringPiSetup() failure!");
        lcd_uninit();
    }

    //keypad colums x5 + relay + buzzer x2
    for(i=0; i<8; i++)
    {
        pinMode(outputs[i], OUTPUT);
        digitalWrite(outputs[i], LOW);
    }

    //keypad rows x4 + sense x2
    for(i=0; i<6; i++)
    {
        pinMode(inputs[i], INPUT);
        pullUpDnControl(inputs[i], PUD_DOWN);
    }
    //odroid burayı test et
    //pullUpDnControl(inputs[4], PUD_UP);//Sense1
    //pullUpDnControl(inputs[5], PUD_UP);//Sense2

    //1.5.0
    //interrupt'dan eski yönteme dönüldü
    /*
    if(!KEYB_Init()){
        lcd_printf(ALG_CENTER, "Error Initialising KEYB Interrupts");
        lcd_flip();
        getch();
    }
    */


	CONFIG_Load();

    while(gtk_events_pending())
        gtk_main_iteration();

    NET_Init();

    //dogru porttan gondesin diye
    READER_TYPE = READER_MIFARE;
    //2.0.2
    //network ayarlarından sonra 500 ms daha bekle
    napms(500);
	if(!main_TS_Guncelle()){
		lcd_printf(ALG_CENTER, "BAGLANTI KURULAMIYOR");
		lcd_flip();
		//2.0.2
		//tarih saat ayarlanamazsa
		//500 ms bekle ve tekrar dene
		napms(500);
		if(main_TS_Guncelle())
            DATE_TIME_OK = 1;
	}else{
		lcd_printf(ALG_CENTER, "TARIH/SAAT AYARLANDI");
		lcd_flip();
		DATE_TIME_OK = 1;
	}


	ZAMANIS_Init();

	LOG_Init();

	READER_Init();

	lcd_clean();
    lcd_printf_ex(ALG_CENTER, 0, "MERCURE ODROID");

	lcd_printf_ex(ALG_CENTER, 2, PROG_VER);
	if(rec_HAVUZ.AKTIF){
		if(rec_HAVUZ.GIRIS){
			lcd_printf_ex(ALG_CENTER, 4, "HAVUZ::GIRIS");
		}
		else{
			lcd_printf_ex(ALG_CENTER, 4, "%s HAVUZ::CIKIS");
		}
	}

	lcd_printf_ex(ALG_CENTER, 8, rec_TERM.IP_TERM);

	lcd_flip();
	SAYAC_Load();

	MSJ_Goster(MSJ_TIP_LG);
	MSJ_Ses(rec_TERM.SES_IZIN_OK);


	if(rec_TERM.TERM_TIP == TERM_PARA)
		flag_LOG_PARA=1;
	else
		flag_LOG_KART=1;

	//2.2.5
	//timer1dk = Timer_Set(60L*1000L);
	timer1dk = Set_Timer(60, 0);


    GdkRGBA font_color;
    font_color.alpha = 1;
    font_color.blue = .7;
    font_color.green = .01;
    font_color.red = 0.1;
    GdkRGBA bg_color;
    bg_color.alpha = 1;
    bg_color.blue = 0;
    bg_color.green = 0;
    bg_color.red = 1;
    gtk_widget_override_color(label1, GTK_STATE_FLAG_NORMAL, &font_color);
    gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
    gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);
    gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
    if(rec_TERM.TERM_TIP == TERM_YEMEKHANE)
        gtk_printf(label1, "%s\nYEMEKHANE TERMİNALİ\n%s", PROG_VER, rec_TERM.IP_TERM);
    else if(rec_TERM.TERM_TIP == TERM_KAPI)
        gtk_printf(label1, "%s\nKAPI TERMİNALİ\n%s", PROG_VER, rec_TERM.IP_TERM);
    //0.0.1
    else if(rec_TERM.TERM_TIP == TERM_UVSS)
        gtk_printf(label1, "%s\nARAÇ ALTI TERMİNALİ\n%s", PROG_VER, rec_TERM.IP_TERM);
    else
        gtk_printf(label1, "%s\n%s", PROG_VER, rec_TERM.IP_TERM);
    //2.0.0
    //brk aşağıyı silmeyi unutma
    //Okuyucu_TIP[0] = 3;
    if((rec_TERM.READERS[0].AKTIF)&&((Okuyucu_TIP[0] ==2)||(Okuyucu_TIP[0] == 3))){
        CN56B_GetSerial(ifd[0], SerNo);
        gtk_printf(label1, "%s\nReader1: CN%s\n", gtk_label_get_label(GTK_LABEL(label1)),SerNo);
    }
    if((rec_TERM.READERS[1].AKTIF)&&((Okuyucu_TIP[1] ==2)||(Okuyucu_TIP[1] == 3))){
        CN56B_GetSerial(ifd[1], SerNo);
        gtk_printf(label1, "%s\nReader2: CN%s\n", gtk_label_get_label(GTK_LABEL(label1)),SerNo);
    }
    //CN56B_Select(ifd[0], SerNo);

    while(gtk_events_pending())
        gtk_main_iteration();
    sleep(2);

    HTTP_RESET = 0;
    //1.6.2
    FTP_RESET = 0;
    FTP_START = 0;
    FTP_SGNL = 1;

    //2.3.1
    //önce daha önce çalışan kopyayı kill et,
    //sonra yeniden başlat
    system("sudo pkill -9 -f zamanlirole");
    system("/home/odroid/Solidus/zamanlirole&");

LOOP:

	while(gtk_events_pending())
        gtk_main_iteration();

    if(sig_flag){

        READER_Uninit(COM1);
        READER_Uninit(COM2);
        lcd_clean();
        lcd_printf_ex(ALG_CENTER, 20, "CIKILIYOR...");
        lcd_flip();
        MSJ_Ses(rec_TERM.SES_IZIN_OK);
        lcd_uninit();
        //2.3.1
        //programdan çıkılıyor, zamanli role işine de son ver.
        system("sudo pkill -9 -f zamanlirole");

        raise(SIGTERM);
        //1.9.4
        sleep(1);
        return 0;
    }

    //2.2.5
    if(!DATE_TIME_OK){
        if(main_TS_Guncelle())
            DATE_TIME_OK = 1;
    }


    ZAMANIS_Kontrol();
    //2.2.1
    if(TERM_AKTIF){
        while(gtk_events_pending())
            gtk_main_iteration();
        //0.0.1
        if(rec_TERM.TERM_TIP == TERM_UVSS)
            UVSS_Karsila();
        else
        if(rezervasyon)
            REZERVASYON_Karsila();//rezervasyonlu calisma, GAZI UNV icin yapildi
        else
            PERSONEL_Karsila();
    }else{
        GdkRGBA font_color;
        font_color.alpha = 1;
        font_color.blue = 1;
        font_color.green = 1;
        font_color.red = 0;
        GdkRGBA bg_color;
        bg_color.alpha = 1;
        bg_color.blue = 0;
        bg_color.green = 0;
        bg_color.red = 1;
        gtk_widget_override_color(label1, GTK_STATE_FLAG_NORMAL, &font_color);
        gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
        gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);
        gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
        gtk_printf(label1, "TERMİNAL KAPALI");
        //while(gtk_events_pending())
          //  gtk_main_iteration();
    }

    FTP_Kontrol();

    //1.8.0
    //if (FTP_RESET)
    if (FTP_RESET == 0x1)
    {
        READER_Uninit(COM1);
        READER_Uninit(COM2);
        lcd_clean();

        lcd_printf_ex(ALG_CENTER, 20, "Ayarlar yenileniyor...");
        lcd_flip();

        gtk_clean();

        GdkRGBA font_color;
        font_color.alpha = 1;
        font_color.blue = 1;
        font_color.green = 1;
        font_color.red = 0;
        GdkRGBA bg_color;
        bg_color.alpha = 1;
        bg_color.blue = 0;
        bg_color.green = 0;
        bg_color.red = 1;
        gtk_widget_override_color(label1, GTK_STATE_FLAG_NORMAL, &font_color);
        gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
        gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);
        gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
        gtk_printf(label1, "AYARLAR YENİLENİYOR...");
        while(gtk_events_pending())
            gtk_main_iteration();
        MSJ_Ses(rec_TERM.SES_IZIN_OK);
        lcd_uninit();
        sleep(1);

        raise(SIGTERM);
        //1.9.4
        sleep(1);
        return 0;
    }else
    if (FTP_RESET == 0x2)
    {
        READER_Uninit(COM1);
        READER_Uninit(COM2);
        lcd_clean();

        lcd_printf_ex(ALG_CENTER, 20, "Terminal Yeniden Baslatiliyor...");
        lcd_flip();

        gtk_clean();

        GdkRGBA font_color;
        font_color.alpha = 1;
        font_color.blue = 1;
        font_color.green = 1;
        font_color.red = 0;
        GdkRGBA bg_color;
        bg_color.alpha = 1;
        bg_color.blue = 0;
        bg_color.green = 0;
        bg_color.red = 1;
        gtk_widget_override_color(label1, GTK_STATE_FLAG_NORMAL, &font_color);
        gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
        gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);
        gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
        gtk_printf(label1, "TERMİNAL YENİDEN BAŞLATILIYOR...");
        while(gtk_events_pending())
            gtk_main_iteration();
        MSJ_Ses(rec_TERM.SES_IZIN_OK);
        lcd_uninit();
        sleep(1);
        system("sudo shutdown -r now");
        sleep(1);
        return 0;
    }
    //1.6.2
    if (FTP_START)
    {
        gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_QUESTION);//GTK_MESSAGE_ERROR
        while(gtk_events_pending())
            gtk_main_iteration();
        if(FTP_SGNL){
            MSJ_Ses(rec_TERM.SES_IZIN_OK);
            FTP_SGNL = 0;
        }
    }

    HTTP_Kontrol();
    if (HTTP_RESET)
    {

        READER_Uninit(COM1);
        READER_Uninit(COM2);
        lcd_clean();

        lcd_printf_ex(ALG_CENTER, 20, "Ayarlar yenileniyor...");
        lcd_flip();

        gtk_clean();

        GdkRGBA font_color;
        font_color.alpha = 1;
        font_color.blue = 1;
        font_color.green = 1;
        font_color.red = 0;
        GdkRGBA bg_color;
        bg_color.alpha = 1;
        bg_color.blue = 0;
        bg_color.green = 0;
        bg_color.red = 1;
        gtk_widget_override_color(label1, GTK_STATE_FLAG_NORMAL, &font_color);
        gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
        gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);
        gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
        gtk_printf(label1, "AYARLAR YENİLENİYOR...");
        while(gtk_events_pending())
            gtk_main_iteration();
        MSJ_Ses(rec_TERM.SES_IZIN_OK);
        lcd_uninit();
        sleep(1);
        raise(SIGTERM);
        //1.9.4
        sleep(1);
        return 0;
    }

    //2.0.6
    //if(KEYPAD_AKTIF){
    if(KEYPAD_AKTIF && ((Okuyucu_TIP[0] != TIP_CN_MFR) && (Okuyucu_TIP[0] != TIP_CN_KYB)) && ((Okuyucu_TIP[1] != TIP_CN_MFR) && (Okuyucu_TIP[1] != TIP_CN_KYB))){
        if(ADMIN_Giris())
        {

            READER_Uninit(COM1);
            READER_Uninit(COM2);
            lcd_clean();
            lcd_printf_ex(ALG_CENTER, 20, "Ayarlar yenileniyor...");
            lcd_flip();

            gtk_clean();

            GdkRGBA font_color;
            font_color.alpha = 1;
            font_color.blue = 1;
            font_color.green = 1;
            font_color.red = 0;
            GdkRGBA bg_color;
            bg_color.alpha = 1;
            bg_color.blue = 0;
            bg_color.green = 0;
            bg_color.red = 1;
            gtk_widget_override_color(label1, GTK_STATE_FLAG_NORMAL, &font_color);
            gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
            gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x_top, logo_y_top);
            gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));
            gtk_printf(label1, "AYARLAR YENİLENİYOR...");
            while(gtk_events_pending())
                gtk_main_iteration();
            MSJ_Ses(rec_TERM.SES_IZIN_OK);
            lcd_uninit();
            sleep(1);

            raise(SIGTERM);
            //1.9.4
            sleep(1);
            return 0;
        }
    }

    //odroid
    //bunları neden zamanis icine almamışım ki?
    //ayrıca is_saat icinde ts guncellenirken hiç ftp_run'a falan bakılmıyor.
    //bunlar buradan kalsın ama zaman.c içine de duzgun girsin.

    //2.2.5
    //if(Timer_End(timer1dk))
    if(Check_Timer(timer1dk))
    {
        //2.2.5
        //timer1dk=Timer_Set(60L*1000L);
        timer1dk = Set_Timer(60, 0);
        syc_dk++;
        if(net_ONLINE>0) net_ONLINE--;

        //1.3.0
        //if((!FTP_RUN)&&(syc_dk>(rec_TERM.TS_GUNCELLE_SAAT*60))) //Girilen Ayar Kadar
        //2.2.5
        //if(syc_dk>(rec_TERM.TS_GUNCELLE_SAAT*60))//Girilen Ayar Kadar
        if(syc_dk==(rec_TERM.TS_GUNCELLE_SAAT*60))//Girilen Ayar Kadar
        {
            //sadece tarih saat set edilirse reboot et.
            if(main_TS_Guncelle()){
                sleep(1);

                READER_Uninit(COM1);
                READER_Uninit(COM2);
                MSJ_Ses(rec_TERM.SES_IZIN_OK);
                lcd_uninit();

                raise(SIGTERM);
                //1.9.4
                sleep(1);
                return 0;
            }
            else{//2.2.7
                DATE_TIME_OK = 0;
            }
        }
    }

goto LOOP;


	READER_Uninit(COM1);
    READER_Uninit(COM2);
	MSJ_Ses(rec_TERM.SES_IZIN_OK);
	lcd_uninit();
	//2.3.1
    //programdan çıkılıyor, zamanli role işine de son ver.
    system("sudo pkill -9 -f zamanlirole");

	return 0;//EXIT_SUCCESS;
}

static void show_cb(GtkWidget *widget, gpointer data)
{
    main_solidus();
}

gint delete_event( GtkWidget *widget,
                   GdkEvent  *event,
                   gpointer   data )
{
    /* If you return FALSE in the "delete_event" signal handler,
     * GTK will emit the "destroy" signal. Returning TRUE means
     * you don't want the window to be destroyed.
     * This is useful for popping up 'are you sure you want to quit?'
     * type dialogs. */


    /* Change TRUE to FALSE and the main window will be destroyed with
     * a "delete_event". */

    return(FALSE);
}

/* Another callback */
void destroy( GtkWidget *widget,
              gpointer   data )
{
    gtk_main_quit();
}


static void CreateTextBox(GtkWidget **entry, GtkWidget *layout)
{

    *entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY (*entry), "");
    gtk_layout_put(GTK_LAYOUT(layout), *entry, 800, 600);
    gtk_entry_set_visibility (GTK_ENTRY (*entry), FALSE);

    gtk_entry_set_max_length(GTK_ENTRY (*entry), 17);
}



int main( int argc, char *argv[])
{
time_t rawtime;
struct tm * timeinfo;

    //1.6.0
    //solidus -v diye çağrılınca program versiyonunu söyler ve çıkar
    if((argc == 2) && (memcmp(argv[1], "-v", 2) == 0)){
        printf("%s\n", PROG_VER);
        return 0;
    }

	time(&rawtime);
	timeinfo = localtime(&rawtime);

//800x480
    window_width = 800;
    window_height = 480;


    logo_x = 270;
    logo_y = 180;

    logo_x_top = 270;
    logo_y_top = 75;

    label_x = 2;
    label_y = 180;

    label_width = 800;
    label_height = 100;

    infobar_x = 0;
    infobar_y = 430;

    asprintf(&small_font_name, "Monospace Bold 20");
    asprintf(&big_font_name, "Monospace Bold 21");

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    /* When the window is given the "delete_event" signal (this is given
     * by the window manager, usually by the "close" option, or on the
     * titlebar), we ask it to call the delete_event () function
     * as defined above. The data passed to the callback
     * function is NULL and is ignored in the callback function. */
    //g_signal_connect (G_OBJECT (window), "delete_event",
    //                    G_CALLBACK (delete_event), NULL);


    /* Here we connect the "destroy" event to a signal handler.
     * This event occurs when we call gtk_widget_destroy() on the window,
     * or if we return FALSE in the "delete_event" callback. */


    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    gtk_window_set_default_size(GTK_WINDOW(window), window_width, window_height);
    gtk_window_set_title(GTK_WINDOW(window), "ODI-SOLI");

    gtk_window_set_decorated (GTK_WINDOW(window), FALSE);

    gtk_window_set_keep_above(GTK_WINDOW(window), TRUE);
    main_layout = gtk_layout_new(NULL, NULL);


    logo = gtk_image_new_from_file("/home/odroid/Solidus/Utarit.png");

    gtk_layout_put(GTK_LAYOUT(main_layout), logo, logo_x, logo_y);


    CreateTextBox(&txtBarcode, main_layout);

    label1 = gtk_label_new("");


    gtk_layout_put(GTK_LAYOUT(main_layout), label1, label_x, label_y);


    gtk_widget_set_size_request(label1, label_width, label_height);

    gtk_label_set_justify(GTK_LABEL(label1), GTK_JUSTIFY_CENTER);


    infobarr = gtk_info_bar_new();

    content_area = gtk_info_bar_get_content_area (GTK_INFO_BAR (infobarr));
    label_status = gtk_label_new("");
    gtk_label_set_justify(GTK_LABEL(label_status), GTK_JUSTIFY_RIGHT);
    gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_WARNING);//GTK_MESSAGE_ERROR
    gtk_container_add (GTK_CONTAINER (content_area), label_status);

    gtk_layout_put(GTK_LAYOUT(main_layout), infobarr, infobar_x, infobar_y);

    gtk_widget_set_size_request(infobarr, 800, 30);

    gtk_container_add(GTK_CONTAINER(window), main_layout);

    gtk_container_set_border_width(GTK_CONTAINER(window), 0);

    gtk_widget_override_font(label_status, pango_font_description_from_string(small_font_name));
    gtk_printf(label_status, "%02d/%02d/%04d       www.utarit.com ©2017       %02d:%02d", timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min);

    g_signal_connect(G_OBJECT(window), "show", G_CALLBACK(show_cb), NULL);

    g_signal_connect (G_OBJECT (window), "destroy",
                        G_CALLBACK (gtk_main_quit), NULL);
    g_signal_connect_swapped(G_OBJECT(window), "delete-event", G_CALLBACK(gtk_widget_destroy), G_OBJECT(window));


    gtk_widget_show_all(window);

    gtk_main();

    //2.3.1
    //programdan çıkılıyor, zamanli role işine de son ver.
    system("sudo pkill -9 -f zamanlirole");

    return 0;
}
