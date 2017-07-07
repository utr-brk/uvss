#include <stdlib.h>
#include <unistd.h>

#include "ftp.h"

char FTP_RESET;

//1.6.2
char FTP_START;

/*
1.3.0
02/12/2015
ftp dizininin altında FTP.END diye bir dosya varsa
solidus dosyası varsa owner ve mod'unu değiştirir,
dizin içerisindeki tüm dosya ve dizinleri bin/Debug altına çeker
FTP_RESET'i 1 yapar, böylece program yeni ayarlar/executable
ile çalışmaya başlar.
*/
void FTP_Kontrol(void)
{

    //1.6.2
    //FTP.START dosyası varsa ekranda FTP başladığını göster
    if(access("/home/odroid/Solidus/FTP/FTP.START", 0) == 0)
    {
        FTP_START = 0x1;
    }
    if(access("/home/odroid/Solidus/FTP/FTP.END", 0) == 0)
    {
        //1.8.0
        FTP_RESET = 0x1;
        if(access("/home/odroid/Solidus/FTP/Ayarlar.php", 0) == 0)
        {
            system("sudo chown root:root /home/odroid/Solidus/FTP/Ayarlar.php");
            system("sudo mv /home/odroid/Solidus/FTP/Ayarlar.php /var/www/html/.");
        }
        //2.2.8
        if(access("/home/odroid/Solidus/FTP/relay1send.php", 0) == 0)
        {
            system("sudo chown root:root /home/odroid/Solidus/FTP/relay1send.php");
            system("sudo mv /home/odroid/Solidus/FTP/relay1send.php /var/www/html/.");
        }
        if(access("/home/odroid/Solidus/FTP/relay2send.php", 0) == 0)
        {
            system("sudo chown root:root /home/odroid/Solidus/FTP/relay1send.php");
            system("sudo mv /home/odroid/Solidus/FTP/relay2send.php /var/www/html/.");
        }
        //2.2.9
        if(access("/home/odroid/Solidus/FTP/relay1ON.php", 0) == 0)
        {
            system("sudo chown root:root /home/odroid/Solidus/FTP/relay1ON.php");
            system("sudo mv /home/odroid/Solidus/FTP/relay1ON.php /var/www/html/.");
        }
        if(access("/home/odroid/Solidus/FTP/relay1OFF.php", 0) == 0)
        {
            system("sudo chown root:root /home/odroid/Solidus/FTP/relay1OFF.php");
            system("sudo mv /home/odroid/Solidus/FTP/relay1OFF.php /var/www/html/.");
        }
        if(access("/home/odroid/Solidus/FTP/relay2ON.php", 0) == 0)
        {
            system("sudo chown root:root /home/odroid/Solidus/FTP/relay2ON.php");
            system("sudo mv /home/odroid/Solidus/FTP/relay2ON.php /var/www/html/.");
        }
        if(access("/home/odroid/Solidus/FTP/relay2OFF.php", 0) == 0)
        {
            system("sudo chown root:root /home/odroid/Solidus/FTP/relay2OFF.php");
            system("sudo mv /home/odroid/Solidus/FTP/relay2OFF.php /var/www/html/.");
        }


        //2.3.1
        if(access("/home/odroid/Solidus/FTP/zamanlirole", 0) == 0)
        {
            system("sudo chown root:root /home/odroid/Solidus/FTP/zamanlirole");
            system("sudo chmod a+x /home/odroid/Solidus/FTP/zamanlirole");
            system("sudo mv /home/odroid/Solidus/FTP/zamanlirole /home/odroid/Solidus/.");
            system("sudo pkill -9 -f zamanlirole");
            system("/home/odroid/Solidus/zamanlirole&");
        }
        //2.3.1
        if(access("/home/odroid/Solidus/FTP/relayrun", 0) == 0)
        {
            system("sudo chown root:root /home/odroid/Solidus/FTP/relayrun");
            system("sudo chmod a+x /home/odroid/Solidus/FTP/relayrun");
            system("sudo mv /home/odroid/Solidus/FTP/relayrun /home/odroid/Solidus/.");
        }

        if(access("/home/odroid/Solidus/FTP/eth0.cfg", 0) == 0)
        {
            system("sudo chown root:root /home/odroid/Solidus/FTP/eth0.cfg");
            system("sudo mv /home/odroid/Solidus/FTP/eth0.cfg /etc/network/interfaces.d/.");
            FTP_RESET = 0x2;
        }

        if(access("/home/odroid/Solidus/FTP/solidus", 0) == 0)
        {
            system("sudo chown root:root /home/odroid/Solidus/FTP/solidus");
            system("sudo chmod a+x /home/odroid/Solidus/FTP/solidus");
        }
        system("rsync -a /home/odroid/Solidus/FTP/ /home/odroid/Solidus/");
        system("rm -rf /home/odroid/Solidus/FTP/*");
        //1.8.0
        //FTP_RESET = 0x1;
    }
}
