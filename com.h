#ifndef COM_H_INCLUDED
#define COM_H_INCLUDED
#pragma pack(push,1)
enum COM_t
{
  COM1          = 0x00,
  COM2          = 0x01,
  COM3          = 0x02,
  COM4          = 0x03,
};

extern unsigned char COM_init(int com_port, int *srl);
extern void COM_uninit(int srl);
extern unsigned int COM_avail(int com_port,unsigned int max);
//extern unsigned char COM_first_handshake(int com_port);
//extern unsigned char COM_second_handshake(int com_port);
//2.5.1
//extern unsigned int COM_ReadBuffer(int com_port,unsigned char * Buffer);
extern char COM_SendBuffer(int com_port,const char * Buffer,unsigned int Len);
//1.8.0
extern unsigned int COM_ReadBuff(int com_port, char * Buffer, int adet);

#pragma pack(pop)

#endif // COM_H_INCLUDED
