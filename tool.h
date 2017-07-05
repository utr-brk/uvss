#ifndef TOOL_H_INCLUDED
#define TOOL_H_INCLUDED

#include "main.h"
#pragma pack(push,1)

//2.1.0
extern char TOOL_GetSplit( char* FullText,int Pos,char Ayrac, char* Ret);
extern void get_time(struct_time *timep);
extern void get_date(struct_date *datep);
extern void aDelay(long MiliSec);
extern unsigned char aBinToHex(char* Bin);
extern void aHexToBin(unsigned char Hex,unsigned char* Bin);
extern unsigned long aPower(unsigned long X,unsigned long Y);
extern unsigned long aHexToLong(char *Hex);
extern unsigned int aHexToInt(char *hexstr);
//extern unsigned int aHexToString(unsigned char *strHex,unsigned char *strByte); m8210
extern  int aHexToString( char *strHex, char *strByte);
//extern unsigned int aStringToHex(unsigned char *strByte,unsigned int Len,unsigned char *strHex);

extern char aDayOfWeek(void);
//extern unsigned char aFileexist(unsigned char *FileName); m8210
extern  char aFileexist( char *FileName);
extern void aWinToDos(unsigned char *Win);
#pragma pack(pop)


#endif // TOOL_H_INCLUDED
