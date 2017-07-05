/*
 * mesaj.c
 *
 *  Created on: 10 Haz 2014
 *      Author: Burak
 */


//#include <dos.h>
#include "mesaj.h"
#include "lcd.h"
#include "tool.h"
#include "config.h"
#include "zamanis.h"
//#include "posapi.h"
#include <wiringPi.h>
#include <softTone.h>
//2.0.0
#include "smm.h"
#include "cn56b.h"
#include "reader.h"

//1.1.3
//uzun path kullanılmaya başlandı
#define LG_BMP   "/home/odroid/Solidus/LG.BMP"
//#define LG_BMP   "utaritIcon.bmp"
//#define LG_BMP	"Lake.bmp"
#define OK_BMP   "/home/odroid/Solidus/OK.BMP"
#define NK_BMP   "/home/odroid/Solidus/NK.BMP"
//#define NK_BMP   "YHY.png"
#define TK_BMP   "/home/odroid/Solidus/TK.BMP"

//odroid
//bu Buzzer olarak da tanımlı keyb.c içinde
//0.55
//#define Bzzr    21
#define Bzzr    24

char MSJ_OK=0x00;


void MSJ_Goster(unsigned char TIP)
{
    GdkRGBA bg_color;
    GdkRGBA font_color;
	z_MSJ_SURE_SN=0; //Mesaj Zamann Sfrla
	//2.1.1
    time_t rawtime;
    struct tm * timeinfo;

	lcd_clean();
	//0.50
	//gtk_clean();

	//2.1.1
	time(&rawtime);
    timeinfo = localtime(&rawtime);
	switch (TIP)
	{
		case MSJ_TIP_LG:
            gtk_clean();
			//0.60
			//gtk_layout_move(GTK_LAYOUT(main_layout), logo, 50, 75);
			gtk_layout_move(GTK_LAYOUT(main_layout), logo, logo_x, logo_y);
			//2.1.1
            gtk_printf(label_status, "%02d/%02d/%04d       www.utarit.com ©2017       %02d:%02d", timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min);
			gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_WARNING);//GTK_MESSAGE_ERROR

			while(gtk_events_pending())
                gtk_main_iteration();
			MSJ_OK=0x01;
			break;
		case MSJ_TIP_OK:
			lcd_printf_ex(ALG_CENTER, 6, rec_TERM.MSJ_IZIN_OK);
			lcd_flip();
			//0.52
            gtk_printf(label1, "%s\n%s", gtk_label_get_label(GTK_LABEL(label1)),rec_TERM.MSJ_IZIN_OK);
            while(gtk_events_pending())
                gtk_main_iteration();

			MSJ_Ses(rec_TERM.SES_IZIN_OK);
			MSJ_OK=0x00;
			//2.0.0
			//CN56B_Buzzer(ifd[0]);
            //CN56B_Led(ifd[0]);
            //2.0.1                    /
            if((Okuyucu_TIP[SMM_PORT] == 2)||(Okuyucu_TIP[SMM_PORT] == 3)){
                CN56B_Buzzer(ifd[SMM_PORT]);
                CN56B_Led(ifd[SMM_PORT]);
            }
			break;
		case MSJ_TIP_NK:
			lcd_printf_ex(ALG_CENTER, 6, rec_TERM.MSJ_IZIN_NK);
			lcd_flip();
			font_color.alpha = 1;
            font_color.blue = .7;
            font_color.green = .01;
            font_color.red = 0.1;
            gtk_widget_override_color(label1, GTK_STATE_FLAG_NORMAL, &font_color);
            bg_color.alpha = 1;
            bg_color.blue = 0./255;//001
            bg_color.green = 0./255;
            bg_color.red = 255./255;
            gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
            gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12
            gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_ERROR);//GTK_MESSAGE_ERROR

			//0.52
            gtk_printf(label1, "%s\n%s", gtk_label_get_label(GTK_LABEL(label1)),rec_TERM.MSJ_IZIN_NK);
            //1.5.0
            /*
            bg_color.alpha = 1;
            bg_color.blue = 0;
            bg_color.green = 0;
            bg_color.red = 1;
            gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
            //0.61
            //gtk_widget_override_font(label1, pango_font_description_from_string("Droid Sans Bold 12"));//Ubuntu Medium Italic 12
            gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12
            gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_ERROR);//GTK_MESSAGE_ERROR
*/
            while(gtk_events_pending())
                gtk_main_iteration();

			MSJ_Ses(rec_TERM.SES_IZIN_NK);
			MSJ_OK=0x00;
			break;
		default:
			lcd_printf_ex(ALG_CENTER, 6, rec_TERM.MSJ_TANIMSIZ);
			lcd_flip();
			font_color.alpha = 1;
            font_color.blue = .7;
            font_color.green = .01;
            font_color.red = 0.1;
            gtk_widget_override_color(label1, GTK_STATE_FLAG_NORMAL, &font_color);
            bg_color.alpha = 1;
            bg_color.blue = 0./255;//001
            bg_color.green = 0./255;
            bg_color.red = 255./255;
            gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
            gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12
            gtk_info_bar_set_message_type (GTK_INFO_BAR (infobarr),GTK_MESSAGE_ERROR);//GTK_MESSAGE_ERROR

			//0.52
            gtk_printf(label1, "%s\n%s", gtk_label_get_label(GTK_LABEL(label1)),rec_TERM.MSJ_TANIMSIZ);
            /*bg_color.alpha = 1;
            bg_color.blue = 0;
            bg_color.green = 0;
            bg_color.red = 1;
            gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, &bg_color);
            //0.61
            //gtk_widget_override_font(label1, pango_font_description_from_string("Droid Sans Bold 12"));//Ubuntu Medium Italic 12
            gtk_widget_override_font(label1, pango_font_description_from_string(big_font_name));//Ubuntu Medium Italic 12*/
            while(gtk_events_pending())
                gtk_main_iteration();

			MSJ_Ses(rec_TERM.SES_TANIMSIZ);
			MSJ_OK=0x00;
			break;
	}
}


void MSJ_Ses(char *S)
{
	unsigned int s,d;
	int i=0;
	//int ss;
	//int scale [8] = { 262, 294, 330, 349, 392, 440, 494, 525 } ;
/*
 //odroid notes
    int scale [8] = { 262, 294, 330, 349, 392, 440, 494, 525 } ;

    for (i = 0 ; i < 8 ; ++i)
    {
      printf ("%3d\n", i) ;
      softToneWrite (Bzzr, scale [i]) ;
      delay (500) ;
    }
 */
	while (i<10)
	{
		if (S[i]==0) break;
		if ((i%2)==0)
		{
			s=(unsigned int) S[i];
			//ss=( int) S[i];
			//ss -= 48;
			//s=(s-48)*1000;
			//s=(s-48)*250;
			//0.54
			//2.1.0
			if (s<48) break;
			s=(s-48)*100;
			//odroid
			//bu ses olayını nasıl yapacağız???
			//sound(s);
			//beep(s, 100);
            //
            digitalWrite(Bzzr, 1);

            //softToneWrite(Bzzr, scale[ss]);
            //softToneWrite(Bzzr, s);
            delay(s);
		} else {
			d=(unsigned int) S[i];
			//1.8.1
			//d=(d-48)*50;
			d=(d-48)*10;
			//aDelay(d);
			//odroid
			//usleep(d*100);
			//napms(d/10);
			//digitalWrite(Bzzr, 0);
			//0.54
            digitalWrite(Bzzr, 0);
			delay(d);

		}
		i++;
	}
	//nosound();
	//softToneStop(Bzzr);
	//0.54
	digitalWrite(Bzzr, 0);
	//softToneWrite(Bzzr, 0);
}
