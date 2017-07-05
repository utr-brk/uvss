#include "lcd.h"
#include <ncurses.h>
//#include <directfb.h>
//#include <posapi.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
//0.50
//#include <gtk/gtk.h>
#include "config.h"
//#include <led.h>
//#include "string_convert.h"
/*
#define font_file_xx  "/usr/share/fonts/wqy-microhei.ttf"
#define font_file_arabic  "/usr/share/fonts/arial.ttf"


IDirectFB				*dfb 	= NULL;
static IDirectFBDisplayLayer	*layer	= NULL;
static IDirectFBWindow			*window	= NULL;
static IDirectFBEventBuffer 	*events	= NULL;
IDirectFBSurface                *main_surface = NULL;
static IDirectFBFont		    *font_16 = NULL;
static unsigned int              back_color = COLOR_WITE;
static unsigned int              font_color = COLOR_BLACK;
*/

//0.50
//clears all of the lines
void gtk_clean(void){
    gtk_widget_override_color(label1, GTK_STATE_FLAG_NORMAL, NULL);//clear color
    gtk_widget_override_font(label1, NULL);//clear font
    gtk_label_set_text(GTK_LABEL(label1), "");
    //0.52
    gtk_widget_override_background_color(label1, GTK_STATE_FLAG_NORMAL, NULL);//clear background
    //0.54
    //gtk_image_clear(GTK_IMAGE(cancel_logo));

//    gtk_label_set_text(GTK_LABEL(label2), "");
//    gtk_label_set_text(GTK_LABEL(label3), "");
//    gtk_label_set_text(GTK_LABEL(label4), "");
 //   gtk_label_set_text(GTK_LABEL(label5), "");
}

void gtk_printf(GtkWidget *label_name, const char * pszFmt,...){

    //int row, col, y, x;
	char textbuf[4096];

	va_list arg;

	va_start(arg, pszFmt);
	vsnprintf(textbuf, sizeof(textbuf), pszFmt, arg);
	va_end (arg);

    gtk_label_set_text(GTK_LABEL(label_name), textbuf);
	/*
	getmaxyx(stdscr, row, col); //get screen size
	getyx(stdscr, y,x); //get current position
    //printf("col: %d, row: %d, len: %d, pos: %d\n", col, row, strlen(textbuf), (col-strlen(textbuf))/2);
	if (ALG_CENTER == alg)
        mvprintw(y, (col - strlen(textbuf))/2, textbuf);
	else if (ALG_LEFT == alg)
		mvprintw(y, x, textbuf);
	else
		mvprintw(y, (col - strlen(textbuf)), textbuf);
    printw("\n");
    */
}

int lcd_init(){
	int retval = 0;
	initscr();
    curs_set(0);
	return retval;
}

void lcd_uninit(void){
    endwin();
}
/*
void lcd_set_bk_color(unsigned int color){
	back_color = color;
}

void lcd_set_font_color(unsigned int color){
	font_color = color;
}
*/

//static int current_y = 1;
void lcd_clean(void){
    clear();
}

void lcd_printf(LCD_ALG alg, const char * pszFmt,...){

    int row, col, y, x;
	char textbuf[4096];

//	const char * pnewline = NULL;
//	int str_length, temp_width;
	va_list arg;



	va_start(arg, pszFmt);
	vsnprintf(textbuf, sizeof(textbuf), pszFmt, arg);
	va_end (arg);

/*
	main_surface->GetFont(main_surface, &font);
	font->GetHeight(font, &font_height);

	main_surface->GetSize(main_surface, &width, &height);
	main_surface->SetColor(main_surface,
			0xFF & (font_color>> 16),
		0xFF & (font_color >> 8),
		0xFF & (font_color >> 0),
		0xFF & (font_color>>24));

	if (current_y + font_height > height){
		rect.x = 0;
		rect.y = current_y + font_height - height;
		rect.w = width;
		rect.h = current_y - rect.y;
		main_surface->GetSubSurface (main_surface, &rect, &sub_surface);

	  	surfdesc.flags = DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT;
		surfdesc.caps  = DSCAPS_SYSTEMONLY;
		surfdesc.width = width;
		surfdesc.height= height;
	   dfb->CreateSurface(dfb, &surfdesc, &store_surface);
	   store_surface->SetBlittingFlags(store_surface, 	DSBLIT_BLEND_ALPHACHANNEL);
	   store_surface->Blit(store_surface, sub_surface, NULL, 0, 0);
	   lcd_clean();
	   main_surface->SetBlittingFlags(main_surface, 	DSBLIT_BLEND_ALPHACHANNEL);
	   rect.y = 0;
	   main_surface->Blit(main_surface, store_surface, &rect, 0, 0);

	   store_surface->Release(store_surface);
	   sub_surface->Release(sub_surface);
	   current_y = rect.h;

	}


	font->GetStringBreak(font, textbuf, strlen(textbuf), width -2, &temp_width, &str_length, &pnewline);

	//utf8text = string_covert(textbuf, str_length);
	//memcpy(utf8text, textbuf, str_length);//asagidaki textbuf^'lar utf8text idi

	if (ALG_CENTER == alg)
		main_surface->DrawString(main_surface, textbuf, string_len(textbuf), width/2, current_y, DSTF_TOPCENTER);
	else if (ALG_LEFT == alg)
		main_surface->DrawString(main_surface, textbuf, string_len(textbuf), 2, current_y, DSTF_TOPLEFT);
	else
		main_surface->DrawString(main_surface, textbuf, string_len(textbuf), width, current_y, DSTF_TOPRIGHT);



	current_y = current_y + font_height + 1;


	if (NULL != pnewline)
	{
		lcd_printf(alg, pnewline);
	}
	*/
	getmaxyx(stdscr, row, col); //get screen size
	row++,
	getyx(stdscr, y,x); //get current position
    //printf("col: %d, row: %d, len: %d, pos: %d\n", col, row, strlen(textbuf), (col-strlen(textbuf))/2);
	if (ALG_CENTER == alg)
        mvprintw(y, (col - strlen(textbuf))/2, textbuf);
	else if (ALG_LEFT == alg)
		mvprintw(y, x, textbuf);
	else
		mvprintw(y, (col - strlen(textbuf)), textbuf);
    printw("\n");
}

void lcd_printf_ex(LCD_ALG alg, int y, const char * pszFmt,...){

	char textbuf[4096];
    int row, col, x;
	va_list arg;
	va_start(arg, pszFmt);
	vsnprintf(textbuf, sizeof(textbuf), pszFmt, arg);
	va_end (arg);
	/*current_y = y;
	lcd_printf(alg, textbuf);
    */
    x = getcurx(stdscr); //get current x position
    getmaxyx(stdscr, row, col); //get screen size
    row++;
	//getyx(stdscr, y,x); //get current position
    if (ALG_CENTER == alg)
        mvprintw(y, (col - strlen(textbuf))/2, textbuf);
	else if (ALG_LEFT == alg)
		mvprintw(y, x, textbuf);
	else
		mvprintw(y, (col - strlen(textbuf)), textbuf);
}

void debug_print(const char * pszFmt,...){

	char textbuf[4096];

	va_list arg;
	va_start(arg, pszFmt);
	vsnprintf(textbuf, sizeof(textbuf), pszFmt, arg);
	va_end (arg);
	//current_y = y;
	lcd_printf(ALG_LEFT, textbuf);
	lcd_flip();
	//beep(500,300);
	//get_keyms(1000);//kb_getkey();
}

/*
void lcd_draw_rectangle(unsigned int x, unsigned int y, unsigned int width, unsigned int height){
	int screen_width, screen_height;
	main_surface->GetSize(main_surface, &screen_width, &screen_height);
	if ((x+1 < screen_width) && (y +1 < screen_height)){
		if (x + 1 + width  > (unsigned int)screen_width)
			width = screen_width - x -1;
		if (x + 1 + height  > (unsigned int)screen_height)
			height = screen_height - y -1;

		main_surface->SetColor(main_surface,
			0xFF & (font_color >> 24),
			0xFF & (font_color >> 16),
			0xFF & (font_color >> 8),
			0xFF & (font_color));
		main_surface->DrawRectangle(main_surface, x, y, width, height);
	}
}

static IDirectFBWindow	*back_ground_window	= NULL;

void lcd_set_background_picture(const char * pszPictureFileName){
	DFBWindowDescription	desc;
	DFBDisplayLayerConfig	config;
	IDirectFBImageProvider *provider;
	IDirectFBSurface		*imgsurf = NULL;
	IDirectFBSurface		*windowSurf = NULL;
	DFBSurfaceDescription	 dsc;
	DFBResult				 err;
	if (NULL == back_ground_window){
		if (0 != layer->GetConfiguration(layer, &config)){
			printf("GetConfiguration Failed\r\n");
		}
		else {
			desc.flags  = DWDESC_POSX | DWDESC_POSY | DWDESC_WIDTH | DWDESC_HEIGHT | DWDESC_CAPS;
			desc.posx	= 0;
			desc.posy	= sys_get_status_bar_height();
			desc.width  = config.width;
			desc.height = config.height - desc.posy;
			desc.caps	= DWCAPS_ALPHACHANNEL;
			if (0 !=  layer->CreateWindow(layer, &desc, &back_ground_window)){
				printf("Create Window Failed\r\n");
			}else{
				back_ground_window->SetOptions(back_ground_window, DWOP_ALPHACHANNEL);
				back_ground_window->SetOpacity(back_ground_window, 0xFF);
				back_ground_window->GetSurface(back_ground_window, &windowSurf);
			}
		}

	}

	if (NULL != back_ground_window){
		 err = dfb->CreateImageProvider(dfb, pszPictureFileName, &provider);
		 if (err != DFB_OK) {
			  printf( "Couldn't load image from file '%s': %s\n",
					  pszPictureFileName, DirectFBErrorString( err ));
		 }else{
			provider->GetSurfaceDescription(provider, &dsc);
		 	dsc.flags = DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT;
		 	dsc.pixelformat = DSPF_ARGB;
		 	if (dfb->CreateSurface(dfb, &dsc, &imgsurf) == DFB_OK)
			  provider->RenderTo(provider, imgsurf, NULL);

			windowSurf->StretchBlit (windowSurf,imgsurf, NULL, NULL);

			imgsurf->Release(imgsurf);
			imgsurf = NULL;
		 	provider->Release(provider);
			provider = NULL;
		 }
	}

	if (NULL != windowSurf){
		windowSurf->Release(windowSurf);
		windowSurf = NULL;
	}
}

void lcd_clear_background_picture(void){
	window->SetOpacity(window, 0xFF);
	window->RaiseToTop(window);
	lcd_flip();
	if (back_ground_window != NULL){
		back_ground_window->Release(back_ground_window);
		back_ground_window = NULL;
	}
}


IDirectFBSurface * __SCREEN_SAVED = NULL;
void lcd_save(void){
	DFBSurfaceDescription	surfdesc;

	if (NULL == __SCREEN_SAVED){
		surfdesc.flags = DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT;
		surfdesc.caps  = DSCAPS_SYSTEMONLY;
		main_surface->GetSize(main_surface, &surfdesc.width, &surfdesc.height);
		dfb->CreateSurface(dfb, &surfdesc, &__SCREEN_SAVED);
	}

	if (NULL != __SCREEN_SAVED)
		__SCREEN_SAVED->Blit(__SCREEN_SAVED, main_surface, NULL, 0, 0);
}

void lcd_restore(void){
	if (NULL != __SCREEN_SAVED)
		main_surface->Blit(main_surface, __SCREEN_SAVED, NULL, 0, 0);
}

*/
/*
void lcd_show_picture(const char * pszPictureFileName, int xx, int yy){
	IDirectFBImageProvider *provider;
	IDirectFBSurface		*imgsurf = NULL;

	DFBSurfaceDescription	 dsc;
	DFBResult				 err;



	 err = dfb->CreateImageProvider(dfb, pszPictureFileName, &provider);
	 if (err != DFB_OK) {
		  printf( "Couldn't load image from file '%s': %s\n",
				  pszPictureFileName, DirectFBErrorString( err ));
	 }else{
		provider->GetSurfaceDescription(provider, &dsc);
	 	dsc.flags = DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT;
	 	dsc.pixelformat = DSPF_ARGB;
	 	if (dfb->CreateSurface(dfb, &dsc, &imgsurf) == DFB_OK)
		  provider->RenderTo(provider, imgsurf, NULL);

		//main_surface->StretchBlit (main_surface,imgsurf, NULL, NULL); //m8110
		//main_surface->Blit(main_surface,imgsurf, NULL, 100, 50);//0.0.8
	 	main_surface->Blit(main_surface,imgsurf, NULL, xx, yy);
		main_surface->Flip(main_surface, NULL, 0);
		imgsurf->Release(imgsurf);
		imgsurf = NULL;
	 	provider->Release(provider);
		provider = NULL;
	 }

}

void lcd_draw_point(unsigned int x, unsigned int y, unsigned int color){
	main_surface->SetColor(main_surface,
			0xFF & (color >> 24),
			0xFF & (color >> 16),
			0xFF & (color >> 8),
			0xFF & (font_color));
	y = y - sys_get_status_bar_height();
	main_surface->DrawLine(main_surface, x, y, x+1, y+1);

}
*/


void lcd_flip(void){
	//main_surface->Flip(main_surface, NULL, 0);
	refresh();
}


int lcd_menu(const char * pszTitle, char *choices[], int n_choices, int select){
//	int retval = -1;

//	int key;
//	unsigned int i;
//	int bLoop;
	//IDirectFBFont * font = NULL;
//	unsigned int oldcolor = 0;


//	unsigned int     max_lines = 0;
//	unsigned int     istart = 0;

//	int screen_width, screen_height, font_height;
/*
	main_surface->GetSize(main_surface, &screen_width, &screen_height);
	main_surface->GetFont(main_surface, &font);
	font->GetHeight(font, &font_height);
*/
/*
	if (select < 0)
		select = 0;

	if (count <= 0)
		retval = -1;
	else{
		//led_set_brightness(LED_ARROW_DOWN,LED_BRIGHTNESS_MAX);
		//led_set_brightness(LED_ARROW_UP,LED_BRIGHTNESS_MAX);

		bLoop = 1;
		while (1 == bLoop)
		{
			lcd_clean();
			//lcd_printf(ALG_CENTER, "************************");//1.0.0.
			lcd_printf(ALG_CENTER, pszTitle);
			lcd_printf(ALG_CENTER, "************************");
			lcd_printf(ALG_CENTER, ""); //m8110 menu item'ları etrafına çizdiği frame bozulmasın diye eklendi.

			//max_lines = (screen_height - current_y - 1) / font_height;


			istart = (select / max_lines) * max_lines;
			for (i = 0;  i < max_lines; i++)
			{
				if (istart + i < count)
				{
					if (istart + i == select)
					{
						//oldcolor = font_color;
						//lcd_set_font_color(COLOR_GREEN);
						//lcd_draw_rectangle(1, current_y -1, screen_width -1, font_height +1);
						lcd_printf(ALG_CENTER, menu[istart + i]); //m8110 left idi
						//lcd_set_font_color(oldcolor);
					}
					else
						lcd_printf(ALG_CENTER, menu[istart + i]); //m8110 left idi


				}
				else
				{
					break;
				}
			}
			lcd_flip();
	LOOP:
			while ((key = getch()))
			{
				//key = kb_getkey();
			//	printf("key:%02x\r\n", key);
				switch((unsigned int)key)
				{
				case 0xF002://up arrow
					select --;
					if (select < 0)
					{
						select = count -1;
					}
					break;
				case 0xF003://down arrow
					select ++;
					if (select >= count)
					{
						select = 0;
					}
					break;
				case 0x1B://cancel
					select = -1;
					bLoop = 0;
					break;
				case 0x0D://enter
					bLoop = 0;
					break;
				default:
					goto LOOP;
				}
			}
			//else
			//
			//	goto LOOP;
			//}

		}

		//led_set_brightness(LED_ARROW_DOWN,LED_BRIGHTNESS_MIN);
		//led_set_brightness(LED_ARROW_UP,LED_BRIGHTNESS_MIN);
	}
	retval = select;
	lcd_clean();
	return retval;
*/
WINDOW *menu_win;
        int highlight = 1;
        int choice = 0;
        int c;
        int WIDTH = 30;
        int HEIGHT = 10;
        int startx = 0;
        int starty = 0;

        initscr();
        clear();
        noecho();
        cbreak();       /* Line buffering disabled. pass on everything */
        startx = (40 - WIDTH) / 2;
        starty = (25 - HEIGHT) / 2;

        menu_win = newwin(HEIGHT, WIDTH, starty, startx);
        keypad(menu_win, TRUE);
        mvprintw(0, 0, "Use arrow keys to go up and down, Press enter to select a choice");
        refresh();
        lcd_print_menu(menu_win, pszTitle, choices, n_choices, highlight);
        while(1)
        {       c = wgetch(menu_win);
                switch(c)
                {       case KEY_UP:
                                if(highlight == 1)
                                        highlight = n_choices;
                                else
                                        --highlight;
                                break;
                        case KEY_DOWN:
                                if(highlight == n_choices)
                                        highlight = 1;
                                else
                                        ++highlight;
                                break;
                        case 10:
                                choice = highlight;
                                break;
                        default:
                                //mvprintw(20, 0, "Character pressed is = %3d Hopefully it can be printed as '%c'", c, c);
                                //refresh();
                                break;
                }
                lcd_print_menu(menu_win, pszTitle, choices, n_choices, highlight);
                if(choice != 0) /* User did a choice come out of the infinite loop */
                        break;
        }
        //mvprintw(23, 0, "You chose choice %d with choice string %s\n", choice, choices[choice - 1]);
        //clrtoeol();
        //refresh();
        //getch();
        endwin();
        return choice;
}

void lcd_print_menu(WINDOW *menu_win, const char * pszTitle, char *choices[], int count, int highlight)
{
        int x, y, i;

        x = 2;
        y = 2;
        //box(menu_win, 0, 0);
        wattron(menu_win, A_UNDERLINE|A_BOLD);
        mvwprintw(menu_win, 0, x, "%s", pszTitle);
        wattroff(menu_win, A_UNDERLINE|A_BOLD);
        for(i = 0; i < count; ++i)
        {       if(highlight == i + 1) /* High light the present choice */
                {       wattron(menu_win, A_REVERSE);
                        mvwprintw(menu_win, y, x, "%s", choices[i]);
                        wattroff(menu_win, A_REVERSE);
                }
                else
                        mvwprintw(menu_win, y, x, "%s", choices[i]);
                ++y;
        }
        wrefresh(menu_win);
}
/*

int kb_hit(void)
{
	DFBWindowEvent  windowEvent;
	DFBResult ret;

	while(1){
		if (events->HasEvent(events) == DFB_OK){
			if (events->PeekEvent(events, DFB_EVENT(&windowEvent)) == DFB_OK) {
				if ( windowEvent.type == DWET_KEYUP)
					return windowEvent.type;

				ret = events->GetEvent(events, DFB_EVENT(&windowEvent));
				if(ret){
					DirectFBError("IDirectFBEventBuffer::GetEvent() failed", ret);
				}
			}
		} else {
			break;
		}
	}

	return 0;
}

 int kb_getkey(void)
{
	DFBWindowEvent windowEvent;
	DFBResult ret;
	while (1){
		events->WaitForEvent(events);
		if ((ret = events->GetEvent(events, DFB_EVENT(&windowEvent))) == DFB_OK) {
			if (windowEvent.type == DWET_KEYUP)
					return windowEvent.key_symbol;
		} else {
			break;
		}
	}

	return 0;
}

*/
