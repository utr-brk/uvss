//#include <dos.h>
//#include <alloc.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <time.h>
#include "keyb.h"
#include "lcd.h"
#include "tool.h"
#include "timer.h"
//#include "gpio.h"
//#include "acctimer.h"

//0.55
//#define Buzzer  21
//int ops[8] = {14, 0, 2, 3, 12, 13, 14, 21};
//int inps[8] = {6, 5, 4, 1, 10, 11, 26, 27};

#define Buzzer  24
//int ops[7] = {8,9,7,21,22,23,24};
//int inps[5] = {15,16,1,4,5};

//1.0.0
//int ops[7] = {26, 27, 7, 21, 22, 23, 24};
//int inps[4] = {1, 4, 5, 6};
int ops[8] = {26, 27, 7, 21, 22, 23, 24, 13};
int inps[6] = {1, 4, 5, 6, 0, 2};

static volatile int globalCounter[4];

void keyInterrupt0 (void) { ++globalCounter[0];}
void keyInterrupt1 (void) { ++globalCounter[1];}
void keyInterrupt2 (void) { ++globalCounter[2];}
void keyInterrupt3 (void) { ++globalCounter[3];}

void key_Click(void)
{
    //odroid//touch
	digitalWrite(Buzzer, HIGH);
	napms(40);
	digitalWrite(Buzzer, LOW);
}


//versiyon 0.43
//her column'a sırasıyla voltaj verilerek çalışan klasik versiyon
int KEYB_press(void)
{
	int i, j, key_press, key_read;
	//unsigned char key_read;
	//unsigned char key_pump[5] = {0x04, 0x08, 0x10, 0x20, 0x40};

	//GPIO_Unlock();
	//for(i=0; i<8; i++)
    //{
    //    pinMode(ops[i], OUTPUT);
    //    //digitalWrite(outputs[i], LOW);
    //    pinMode(inps[i], INPUT);
    //}
    key_press=-1;
    for(i=0 ; i<5; i++){
        //burada sırasıyla 5 column'dan birisine voltaj verilip, 4 row okunacak.
        //pinMode(ops[i], OUTPUT);
        digitalWrite(ops[i], HIGH);

        //GPIO_Set(0x47, key_pump[i]); // send data to relay
        // input data from GPIO[15-8]
        //key_read = GPIO_Get(0x4c);
        //key_read = key_read & 0x0F;
        napms(10);
        key_read = -1;
        for(j=0; j<4; j++)
        {
            //pinMode(inps[j], INPUT);
            if(digitalRead(inps[j])){
                key_read = j;
                break;
            }
        }

        switch (key_read){
            case -1: break;
            case 0:
                if(i==0)
                    key_press = 0xF1;
                else
                    if(i==1)
                        key_press = 1;
                else
                    if(i==2)
                        key_press = 2;
                else
                    if(i==3)
                        key_press = 3;
                    else
                        key_press = 0xF5;
                break;
            case 1:
                if(i==0)
                    key_press = 0xF2;
                else
                    if(i==1)
                        key_press = 4;
                else
                    if(i==2)
                        key_press = 5;
                else
                    if(i==3)
                        key_press = 6;
                    else
                        key_press = 0xF6;
                break;
            case 2:
                if(i==0)
                    key_press = 0xF3;
                else
                    if(i==1)
                        key_press = 7;
                else
                    if(i==2)
                        key_press = 8;
                else
                    if(i==3)
                        key_press = 9;
                    else
                        key_press = 0xF7;
                break;
            case 3:
                if(i==0)
                    key_press = 0xF4;
                else
                    if(i==1)
                        key_press = KEY_BKSP;
                else
                    if(i==2)
                        key_press = 0;
                else
                    if(i==3)
                        key_press = KEY_ENTR;
                    else
                        key_press = 0xF8;
                break;
            default : break;
        }
        digitalWrite(ops[i], LOW);
        if (key_press>-1) break;
    }
	//GPIO_Lock();
	return key_press;
}


//1.5.0
//interrupt'dan eski yönteme dönüldü
/*
int KEYB_Init(void)
{

    if(wiringPiISR(inps[0], INT_EDGE_RISING, &keyInterrupt0)<0){
        lcd_printf_ex(ALG_CENTER, 5, "unable to setup ISR on keyInterrupt0");
        lcd_flip();
        return 0;
    }
    if(wiringPiISR(inps[1], INT_EDGE_RISING, &keyInterrupt1)<0){
        lcd_printf_ex(ALG_CENTER, 5, "unable to setup ISR on keyInterrupt1");
        lcd_flip();
        return 0;
    }
    if(wiringPiISR(inps[2], INT_EDGE_RISING, &keyInterrupt2)<0){
        lcd_printf_ex(ALG_CENTER, 5, "unable to setup ISR on keyInterrupt2");
        lcd_flip();
        return 0;
    }
    if(wiringPiISR(inps[3], INT_EDGE_RISING, &keyInterrupt3)<0){
        lcd_printf_ex(ALG_CENTER, 5, "unable to setup ISR on keyInterrupt3");
        lcd_flip();
        return 0;
    }

    return 1;
}

//versiyon 0.44
//interrupt ile yapılan yeni versiyon
int KEYB_press(void)
{
	int i, j, key_press, key_read;
	//unsigned char key_read;
	//unsigned char key_pump[5] = {0x04, 0x08, 0x10, 0x20, 0x40};

	//GPIO_Unlock();
	//for(i=0; i<8; i++)
    //{
    //    pinMode(ops[i], OUTPUT);
    //    //digitalWrite(outputs[i], LOW);
    //    pinMode(inps[i], INPUT);
    //}
    int myCounter[4];

    for(i=0; i<4; i++)
        globalCounter[i] = myCounter[i] = 0;

    key_press=-1;
    for(i=0 ; i<5; i++){
        //burada sırasıyla 5 column'dan birisine voltaj verilip, 4 row okunacak.
        //pinMode(ops[i], OUTPUT);
        digitalWrite(ops[i], HIGH);

        //GPIO_Set(0x47, key_pump[i]); // send data to relay
        // input data from GPIO[15-8]
        //key_read = GPIO_Get(0x4c);
        //key_read = key_read & 0x0F;
        //odroid
        //burası optimize edilmeli
        //10 msec azaltılsa ne olur bakılmalı.
        napms(5);
        key_read = -1;
        for(j=0; j<4; j++)
        {
            //pinMode(inps[j], INPUT);
            //if(digitalRead(inps[j])){
            //    key_read = j;
            //    break;
            //}
            if(globalCounter[j] != myCounter[j]){
                myCounter[j] = globalCounter[j];
                key_read = j;
                break;
            }
        }

        switch (key_read){
            case -1: break;
            case 0:
                if(i==0)
                    key_press = 0xF1;
                else
                    if(i==1)
                        key_press = 1;
                else
                    if(i==2)
                        key_press = 2;
                else
                    if(i==3)
                        key_press = 3;
                    else
                        key_press = 0xF5;
                break;
            case 1:
                if(i==0)
                    key_press = 0xF2;
                else
                    if(i==1)
                        key_press = 4;
                else
                    if(i==2)
                        key_press = 5;
                else
                    if(i==3)
                        key_press = 6;
                    else
                        key_press = 0xF6;
                break;
            case 2:
                if(i==0)
                    key_press = 0xF3;
                else
                    if(i==1)
                        key_press = 7;
                else
                    if(i==2)
                        key_press = 8;
                else
                    if(i==3)
                        key_press = 9;
                    else
                        key_press = 0xF7;
                break;
            case 3:
                if(i==0)
                    key_press = 0xF4;
                else
                    if(i==1)
                        key_press = KEY_BKSP;
                else
                    if(i==2)
                        key_press = 0;
                else
                    if(i==3)
                        key_press = KEY_ENTR;
                    else
                        key_press = 0xF8;
                break;
            default : break;
        }
        digitalWrite(ops[i], LOW);
        if (key_press>-1) break;
    }
	//GPIO_Lock();
	return key_press;
}

*/
int KEYB_pressup(void)
{
	int key_press;
	int key_up;

	key_press = KEYB_press();
	key_up		= key_press;
	while(key_up>0)
	{
		key_press=key_up; //Son Kaldırılan Gecerli
		key_up=KEYB_press();
	}
	return key_press;
}

int KEYB_getch(int sn)
{
//unsigned char key_read;
//long timeout;
//2.2.5
//struct timeval set_timeout;
struct timeval chk_timeout;
int key_press,key_up,last_key;

	//if (sn>0) timeout = AccTimer_SetTime(sn*1000);
	if (sn>0){
        //2.2.5
        //set_timeout.tv_sec = sn;
        //set_timeout.tv_usec = 0;
        //chk_timeout = Set_Timer(set_timeout);
        chk_timeout = Set_Timer(sn, 0);
	}

	key_press = -1;
	key_up=-1;
	last_key=-1;
	while(key_up<0){
		key_press=KEYB_press();
		if (key_press>-1)
		{
			last_key=key_press;
			key_press=-1;
		} else {
			if (last_key>-1) key_up=1;
		}
		if (sn>0)
		{
			if (Check_Timer(chk_timeout)) break;
		}
	}
	if (last_key>-1) key_Click();
	return last_key;
}

void KEYB_inputnumeric(char* result,int row,int col,int max)
{
	char *disp;
	int len = strlen(result);
	int key, row_, col_, y, x;
//	char c[2];
	disp=(char*) malloc(max+1);
	strcpy(disp,result);
	getmaxyx(stdscr, row_, col_); //get screen size
    row_++;
	while (1)
	{
		len = strlen(disp);
		//LCD_Print(row,col,disp);
		//odroid
		//burası olmadı/iyi dusun!...
		if(len){
            getyx(stdscr, y, x);
            move(y, x-1);
            clrtoeol();
            mvprintw(row, (col_ - 15)/2+col, disp);
            lcd_flip();
        }
		key=KEYB_getch(0);
		if (key<10)
		{
			if (len<max)
			{
				disp[len]=key+48;
				disp[len+1]=0x00;
			}
		}
		if (key==KEY_BKSP)
		{
			if (len>0) disp[len-1]=0x00;
		}
		if (key==KEY_ENTR) break;
	}
	strcpy(result,disp);
	free(disp);
}

void KEYB_inputwait(char* result,int row,int col,int max,int sn)
{
	char *disp;
	int len = strlen(result);
	int key=0xFF;//2.3.1
	//long timeout;
	//2.2.5
	//struct timeval set_timeout;
	struct timeval chk_timeout;
	disp = (char*) malloc(max+1);
	strcpy(disp,result);
	//2.2.5
	//set_timeout.tv_sec = sn;
    //set_timeout.tv_usec = 0;
    //chk_timeout = Set_Timer(set_timeout);
    chk_timeout = Set_Timer(sn, 0);
	//timeout = AccTimer_SetTime(sn*1000);

	while (!Check_Timer(chk_timeout))
	{
		len = strlen(disp);
		//LCD_Print(row,col,disp);
		mvprintw(row, col, disp);
		lcd_flip();
		key=KEYB_getch(sn);
		if (key<0) break;
		if (key<10)
		{
			if (len<max)
			{
				disp[len]=key+48;
				disp[len+1]=0x00;
			}
		}
		if (key==KEY_BKSP)
		{
			if (len>0) disp[len-1]=0x00;
		}
		if (key==KEY_ENTR) break;
	}
	if (key==0xF9)
	{
		strcpy(result,disp);
    } else {
    strcpy(result,"");
    }
  free(disp);
}

void KEYB_inputip(char *retIP,int row)
{
  char *gIP[4];
  int i, row_, col_;
  for (i=0;i<4;i++) gIP[i] = (char*) malloc(4);
  getmaxyx(stdscr, row_, col_); //get screen size
  row_++;
  for (i=0;i<4;i++)
  {
    memset(gIP[i],0x00,4);
    while(1)
    {
        KEYB_inputnumeric(gIP[i],row,i*4,3);
        if (strlen(gIP[i])==0) continue;
        if ((i==0)&&(atoi(gIP[i])==0)) continue;
        if (atoi(gIP[i])>255) continue;
        break;
    } //Mutlaka duzgun deger girmeli
    //LCD_Print(row,i*4,gIP[i]);
    //LCD_Print(row,((i+1)*4-1),".");
    mvprintw(row, (col_ - 15)/2 +i*4, gIP[i]);
    if(i!=3)
        mvprintw(row,(col_ - 15)/2+((i+1)*4-1),".");
    lcd_flip();
    //odroid nasıl olacak burası???
  }
  sprintf(retIP,"%s.%s.%s.%s",gIP[0],gIP[1],gIP[2],gIP[3]);
  for (i=0;i<4;i++) free(gIP[i]);
}
