#ifndef GUNISIGI_H_INCLUDED
#define GUNISIGI_H_INCLUDED

//#include <dos.h>
#include <time.h>
#pragma pack(push,1)
//extern char GUNISIGI_Kontrol(struct dosdate_t *d,struct dostime_t *t);

extern char GUNISIGI_Kontrol(struct tm *timeinfo);

#pragma pack(pop)
#endif // GUNISIGI_H_INCLUDED
