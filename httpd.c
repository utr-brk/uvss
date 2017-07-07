#include <stdlib.h>
#include <unistd.h>
#include "main.h"

//1.4.0
//2.3.1
//void HTTP_Kontrol(void);

/*
1.4.0
16/12/2015
/home/odroid/Solidus/HTTP dizininin altında HTTP.END diye bir dosya varsa
AYARLAR.INI dosyasını /home/odroid/Solidus altına çeker
HTTP_RESET'i 1 yapar, böylece program yeni ayarlar ile çalışmaya başlar.
*/
void HTTP_Kontrol(void)
{
    if(access("/home/odroid/Solidus/HTTP/HTTP.END", 0) == 0)
    {
        system("rsync -a /home/odroid/Solidus/HTTP/ /home/odroid/Solidus/");
        system("rm -rf /home/odroid/Solidus/HTTP/*");
        HTTP_RESET = 0x1;
    }
}

