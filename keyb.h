#ifndef KEYB_H_INCLUDED
#define KEYB_H_INCLUDED
#pragma pack(push,1)
#define	KEY_BKSP    0xFA
#define KEY_ENTR    0xF9

extern int KEYB_press(void);
extern int KEYB_pressup(void);
extern int KEYB_getch(int sn);
extern void KEYB_inputnumeric(char* result,int row,int col,int maxx);
extern void KEYB_inputwait(char* result,int row,int col,int maxx,int sn);
extern void KEYB_inputip(char *retIP,int row);
extern int  KEYB_Init(void);

#pragma pack(pop)
#endif // KEYB_H_INCLUDED
