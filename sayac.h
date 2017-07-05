#ifndef SAYAC_H_INCLUDED
#define SAYAC_H_INCLUDED

//#include <dos.h>
#include "main.h"
#pragma pack(push,1)
typedef struct
{
    //unsigned long SAYI;//m8110
	uint32_t SAYI;
    struct_date ZAMAN;
}
struct_SAYAC;

extern struct_SAYAC SAYAC;


extern void SAYAC_Arti(void);
extern void SAYAC_Load(void);
extern void SAYAC_Save(void);
extern void SAYAC_Show(void);
#pragma pack(pop)

#endif // SAYAC_H_INCLUDED
