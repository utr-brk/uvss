#ifndef UVSS_H_INCLUDED
#define UVSS_H_INCLUDED

#pragma pack(push,1)

//3.1.0
extern int uvss_socket;

extern void UVSS_Karsila(void);
extern int UVSS_Vehicle_Check(int IN_OUT);
extern int UVSS_Read(int LP_IMAGE);
extern int UVSS_Send(int CAR_FINISHED);
extern int SP_UVSS(int SP_TYPE);


#pragma pack(pop)


#endif // UVSS_H_INCLUDED
