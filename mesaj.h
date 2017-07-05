#ifndef MESAJ_H_INCLUDED
#define MESAJ_H_INCLUDED

#pragma pack(push,1)

#define MSJ_TIP_LG		0x00
#define MSJ_TIP_OK		0x01
#define MSJ_TIP_NK		0x02
#define MSJ_TIP_TK		0x03

extern char MSJ_OK;
extern unsigned char z_MSJ_SURE_SN;

extern void MSJ_Goster(unsigned char TIP);
extern void MSJ_Ses(char *S);

#pragma pack(pop)
#endif // MESAJ_H_INCLUDED
