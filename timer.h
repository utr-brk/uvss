#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED
#pragma pack(push,1)

//2.2.5
//extern unsigned long long Timer_Set(unsigned long MS);
//extern char Timer_End(unsigned long long Timer);

//2.2.5
//extern struct timeval Set_Timer(struct timeval tSon);
extern struct timeval Set_Timer(int sec, long usec);
extern int Check_Timer(struct timeval tEnd);
#pragma pack(pop)

#endif // TIMER_H_INCLUDED
