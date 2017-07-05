#ifndef LCD_H_INCLUDED
#define LCD_H_INCLUDED
#include <ncurses.h>
#include <gtk/gtk.h>
/*
//#include <directfb.h>
//extern IDirectFB				*dfb;
//#define RGBA(R, G, B, A) ((((unsigned int)(unsigned char)R)<< 16) | \
//						  (((unsigned int)(unsigned char)G)<< 8) | \
//						  (((unsigned int)(unsigned char)B)<< 0)  | \
//						  (((unsigned int)(unsigned char)A)<< 24))
*/
/*
#define COLOR_RED   RGBA(0xFF, 0x00, 0x00, 0xFF)
#define COLOR_GREEN RGBA(0x00, 0xFF, 0x00, 0xFF)
#define COLOR_BLUE  RGBA(0x00, 0x00, 0xFF, 0xFF)
#define COLOR_WITE  RGBA(0xFF, 0xFF, 0xFF, 0xFF)
#define COLOR_BLACK RGBA(0x00, 0x00, 0x00, 0xFF)
#define COLOR_YELLOW RGBA(0xFF, 0xFF, 0x00, 0xFF)
*/
#pragma pack(push,1)

typedef enum {
	ALG_CENTER,
	ALG_LEFT,
	ALG_RIGHT
}LCD_ALG;

typedef enum{
    DIKS_ESCAPE     = 0x1B,
    DIKS_ENTER      = 0x0D,
    DIKS_BACKSPACE  = 0x08,
    DIKS_ALT        = 0x41 //bunu unutma
}DFBInputDeviceKeySymbol;

//0.50
void gtk_clean(void);
void gtk_printf(GtkWidget *label_name, const char * pszFmt,...);

int  lcd_init(void);
void lcd_uninit(void);
void lcd_set_bk_color(unsigned int color);
void lcd_set_font_color(unsigned int color);
void lcd_clean(void);
void lcd_printf(LCD_ALG alg, const char * pszFmt,...);
void lcd_printf_ex(LCD_ALG alg, int y, const char * pszFmt,...);
void debug_print(const char * pszFmt,...);
void lcd_draw_rectangle(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
void lcd_set_background_picture(const char * pszPictureFileName);
void lcd_clear_background_picture(void);
void lcd_show_picture(const char * pszPictureFileName, int xx, int yy);//0.0.8
void lcd_draw_point(unsigned int x, unsigned int y, unsigned int color);
void lcd_save(void);
void lcd_restore(void);
void lcd_flip(void);
int lcd_menu(const char * pszTitle, char *menu[], int count, int select);
void lcd_print_menu(WINDOW *menu_win, const char * pszTitle, char *choices[], int count, int highlight);
int  kb_hit(void);
int  kb_getkey(void);
//2.3.1
//int get_keyms(int timeout);
#pragma pack(pop)
#endif // LCD_H_INCLUDED
