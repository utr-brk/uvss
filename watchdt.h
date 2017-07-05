#ifndef WATCHDT_H_INCLUDED
#define WATCHDT_H_INCLUDED
#pragma pack(push,1)

extern void WDT_Init(const unsigned long);
extern void WDT_Clear(void);
extern void WDT_Disable(void);

#pragma pack(pop)

#endif // WATCHDT_H_INCLUDED
