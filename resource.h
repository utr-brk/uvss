#ifndef RESOURCE_H_INCLUDED
#define RESOURCE_H_INCLUDED
#pragma pack(push,1)

extern char RESOURCE_Init(char * RES_NAME);
extern void RESOURCE_Uninit(void);
extern void RESOURCE_OpenFile(void);
extern void RESOURCE_CloseFile(void);
extern char *RESOURCE_ReadLineOpen(unsigned int NO);
extern char *RESOURCE_ReadLine(unsigned int NO);

#pragma pack(pop)

#endif // RESOURCE_H_INCLUDED
