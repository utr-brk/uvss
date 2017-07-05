/*
 * printer.c
 *
 *  Created on: 18 Haz 2014
 *      Author: Burak
 */


//#include <posapi.h>
#include <printer.h>
//#include <directfb.h>
#include "lcd.h"
#include "string_convert.h"
//#include "visualstring.h"


#define printer_device_name "/dev/printer0"
#define font_file_xx "/usr/share/fonts/wqy-microhei.ttf"
#define font_file_arabic  "/usr/share/fonts/arial.ttf"
//extern IDirectFBSurface         *main_surface;


void Printer_Demo(void){

	int ifd = -1;
	int retval = 0;
	printer_param_t  param;
	printer_status_t status;
	DFBSurfaceDescription	surfdesc;
	IDirectFBSurface        * surface = NULL;
	IDirectFBFont		    *font_16 = NULL;
	IDirectFBFont		    *font_24 = NULL;
	IDirectFBFont		    *font_32 = NULL;
	DFBFontDescription	    fdesc;
	//const char * disp_text = "مجموعة الحكير";
	const char * disp_text ="lovelovelove,love,love,love";
	int x, y,height;
	int rectangle_x, rectangle_y, rectangle_width, rectangle_height;
/*
#if 1
	VisualString vs;
	char outputStringUtf8[255];
	vs.inputString = disp_text;
	vs.inputLength = strlen(disp_text);
	bidi_visualize_line(&vs, outputStringUtf8,sizeof(outputStringUtf8));
#endif
*/
	const char * utf8text = string_covert(disp_text, strlen(disp_text));


	lcd_clean();
	lcd_printf(ALG_LEFT, "Prepare for print...");
	lcd_flip();
	lcd_clean();
	//lcd_printf(ALG_LEFT, outputStringUtf8);

	lcd_flip();

	kb_hit();

	memset(&fdesc,0, sizeof(fdesc));
	fdesc.flags  = DFDESC_HEIGHT;
	fdesc.height = 16;
	dfb->CreateFont(dfb, font_file_xx, &fdesc, &font_16);
	if (NULL == font_16){
		lcd_printf(ALG_LEFT, "craete font16 failed.");
		lcd_flip();
		retval = -1;
	}

	if (0 == retval){
		fdesc.height = 24;
		dfb->CreateFont(dfb, font_file_xx, &fdesc, &font_24);
		if (NULL == font_24){
			lcd_printf(ALG_LEFT, "craete font16 failed");
			lcd_flip();
			retval = -1;
		}
	}

	if (0 == retval){
		fdesc.height = 32;
		dfb->CreateFont(dfb, font_file_arabic, &fdesc, &font_32);
		if (NULL == font_32){
			lcd_printf(ALG_LEFT, "craete font32 failed");
			lcd_flip();
			retval = -1;
		}
	}

	if (0 == retval){
		ifd = printer_open(printer_device_name, O_WRONLY | O_NONBLOCK);
		if (ifd < 0){
			lcd_printf(ALG_LEFT, "Device Open failed.");
			lcd_flip();
		}
	}

	if (0 == retval){

		printer_get_param(ifd, &param);

		surfdesc.flags = DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT;
		surfdesc.caps  = DSCAPS_NONE;
		surfdesc.width = param.ro_width;
		surfdesc.height= 1000;
		dfb->CreateSurface(dfb, &surfdesc, &surface);
		surface->Clear(surface,
						0xFF,
						0xFF,
						0xFF,
						0xFF);

		surface->SetColor(surface, 0x00, 0x00, 0x00, 0xFF);


		x = param.ro_width/2;
		y = 20;

		rectangle_x = 5;
		rectangle_y = y - 10;
		rectangle_width = param.ro_width - 5 * 2,

		surface->SetFont(surface, font_16);
		//surface->DrawString(surface, outputStringUtf8, -1, x, y, DSTF_TOPCENTER);
		surface->DrawString(surface, utf8text, -1, x, y, DSTF_TOPCENTER);
		font_16->GetHeight(font_16, &height);
		y+= height + 1;


		surface->SetFont(surface, font_24);
		surface->DrawString(surface, utf8text, -1, x, y, DSTF_TOPCENTER);
		font_24->GetHeight(font_24, &height);
		y+= height + 1;


		surface->SetFont(surface, font_32);
		surface->DrawString(surface, "ucret-şiğüöçĞÜİŞÖÇıAD-", -1, x, y, DSTF_TOPCENTER);
		font_32->GetHeight(font_32, &height);
		y+= height + 1;

		rectangle_height = y - rectangle_y;
		surface->DrawRectangle(surface, rectangle_x, rectangle_y,
			rectangle_width, rectangle_height );

	}

	if (0 == retval){

			lcd_printf(ALG_LEFT, "Printing....");
			lcd_flip();
			kb_hit();

			//printer_write(ifd,utf8text,sizeof(utf8text));
			print_surface(ifd, surface, y + 100);
			do
			{
				usleep(100000);
				printer_get_status(ifd, &status);
				if (((status.status  >> PRINTER_STATUS_BUSY) & 0x01) == 0x01)
					lcd_printf(ALG_LEFT, "printer status = busy");
				else if (((status.status  >> PRINTER_STATUS_HIGHT_TEMP) & 0x01) == 0x01)
					lcd_printf(ALG_LEFT, "printer status = tempreture high");
				else if (((status.status  >> PRINTER_STATUS_PAPER_LACK) & 0x01) == 0x01)
					lcd_printf(ALG_LEFT, "printer status = no paper");
				else if (((status.status  >> PRINTER_STATUS_FEED) & 0x01) == 0x01)
					lcd_printf(ALG_LEFT, "printer status = feed paper");
				else if (((status.status  >> PRINTER_STATUS_PRINT) & 0x01) == 0x01)
					lcd_printf(ALG_LEFT, "printer status = printing");
				else if (((status.status  >> PRINTER_STATUS_FORCE_FEED) & 0x01) == 0x01)
						lcd_printf(ALG_LEFT, "printer status = force feed paper");
				else if (((status.status  >> PRINTER_STATUS_POWER_ON) & 0x01) == 0x01)
					lcd_printf(ALG_LEFT, "printer status = power on");
				lcd_flip();

			}while (status.status != 0);

			lcd_printf(ALG_LEFT, "Finished.");
			lcd_flip();


	}

	if (NULL != surface)
		surface->Release(surface);

	if (NULL != font_16)
		font_16->Release(font_16);

	if (NULL != font_24)
		font_24->Release(font_24);

	if (NULL != font_32)
		font_32->Release(font_32);




	if (ifd >= 0){
		printer_close(ifd);
		ifd = -1;
	}

	kb_getkey();

}

