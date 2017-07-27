/*
 * main.h
 *
 *  Created on: 10 Haz 2014
 *      Author: Burak
 *
 *  =============================
 *  versiyon 0.0.4
 *  05/08/2014
 *  ilk test sürümü
 *  =============================
 *  versiyon 0.0.5
 *  06/08/2014
 *  Para terminali olarak calisirken:
 *  Yukleme sonucu karttaki bakiye 100.00 TL uzerine cikinca ya da
 *  Dusme sonucu eksiye dusunce uyarı vermeden basa donuyordu, uyarı verir hale getirildi.
 *
 *	Master kart okutulduktan sonra sanki normal bir kart okutulmuş gibi devam edip islem yapıyordu, duzeltildi.
 *
 *	Para terminali olarak set edilince Para logu, Harcama terminali olarak set edilince harcama logu yapısında data gonderiyor.
 *
 *	Log gonderme kısımları duzenlendi.
 *	=============================
 *	versiyon 0.0.6
 *  10/08/2014
 *  yazıcılardaki Türkçe karakter sorunu ile ilgilenildi
 *  POS'un kendi uzerindeki yazıcı için YAZICI.INI UTF-8 olarak save edilirse Türkçe harfleri basar.
 *  CUSTOM yazıcının Türkçe desteği olmadığı için YAZICI.INI ANSI save edilmeli ve Türkçe harf kullanılmamalı.
 *
 *  Log göndermedeki sorunlar yemekhane terminali icin halledildi, para terminali offline data gondermede hala problemli.
 *  Offline data varsa 5 sn'de bir log gönderir hale getirildi.
 *
 *  Yemek saati içindeyken ekran hep açık olacak şekilde ayarlandı.
 *
 * ===============================
 * versiyon 0.0.7
 * 11/08/2014
 * para terminaliyken de logları duzgun gonderiyor.
 * para terminaliyken eger data online olarak gonderildiyse log olarak tekrar gondermiyor.
 * tum loglar tek seferde gonderiliyor.
 *
 * ===============================
 * versiyon 0.0.8
 * 11/08/2014
 * yemekhane ya da harcama terminalinde tek seferde 3 log birden gonderiyor, gonderemezse ara veriyor.
 * para terminalinde tek seferde tum logları gonderirken basarısız olursa gondermeye ara veriyor.
 *
 * kendi uzerindeki yazıcıyla ilgili iyileştirmeler yapıldı.
 * eger kagit biterse uyarı verip, rulonun degistirilmesini istiyor.
 * kağıt takılıp bir tusa basılınca yarım kalan baskıyı tekrar ediyor.
 *
 * FTP Idle Timeout yaklaşık 2 dk yapıldı.
 * FTP'den çıkıp reboot ederken UTARIT logosu basıyor.
 *
 * 150x150 pixel'lik universite logosu eklendi (LG.BMP adıyla saklanmalı)
 * Açılışa Utarit logosu eklendi.
 *
 * Ekrandaki saat artık saniyede bir değil, dakikada bir guncelleniyor.
 *
 * ===============================
 * versiyon 0.0.9
 * 12/08/2014
 * kendi ustundeki yazıcıda kagıt biterse baskı almadan devam etme secenegi sunuyor.
 *
 * programın yeni versiyonu ftp ile kopyalanabiliyor.
 *
 * ===============================
 * versiyon 0.0.91
 * 13/08/2014
 * custom yazıcı icin de kagit kontrolu eklendi.
 *
 * ===============================
 * versiyon 0.0.92
 * 18/08/2014
 * harcama ve yükleme terminaliyken fisin 2. nushasi basildiktan sonra
 * ya da islem basarisiz olursa ana ekrana geri donuyor.
 *
 * ===============================
 * versiyon 0.0.93
 * 18/08/2014
 * harcama ve yükleme terminaliyken cancel'a basilinca duzgun davraniyor.
 *
 * ===============================
 * versiyon 0.0.94
 * 18/08/2014
 * yükleme terminaliyken menu'de cancel'a basilinca duzgun davraniyor.
 *
 * ===============================
 * vesiyon 0.1.0
 * 21/08/2014
 * admin menu'su eklendi
 *
 * ftp baglantisinda ip numarasi buyukken olusan hata duzeltildi
 *
 * ===============================
 * vesiyon 0.2.0
 * 04/03/2015
 * sdk-new8210-1.0.16 ile compile edildi.
 * Release versiyonuna compile edildi.
 *
 * zamanis.c'de chr_start ve chr_end array'lerinin boundry'leri geciliyordu, duzeltildi.
 * personel.c'de PERSONEL_Log(1) yerine PERSONEL_Log(Okuyucu) diye cagriliyordu, ama Okuyucu initialize edilmemisti.
 *
 * harcama ya da para terminali olarak calisiyorsa, ikinci nusha fis basmaya yazici.ini icine bakarak karar veriliyor.
 * YAZICI.INI’de TEKNUSHA diye bir alan yoksa cift nusha basar, varsa ve 1 ise tek nusha basar, 0 ise cift nusha basar
 * ===============================
 * vesiyon 0.2.1
 * 25/03/2015
 * FTP bağlantisinda sorun vardı, static char IPP[16]  yapılarak düzeltildi.
 *
 * ===============================
 * versiyon 0.2.2
 * 27/03/2015
 * Para Terminali ise key bilgileri master.key dosyasında saklanmıyor.
 * terminal açılınca default Utarit key'leri ile çalışıyor, her seferinde master kart okutulması lazım.
 * Ayrıca kartta taşınan para mx 200.00 TL yapıldı
 *
 * ===============================
 * versiyon 0.3.0
 * 20/04/2015
 * Harcama ya da yukleme yaparken noktalı gösterime geçildi.
 *
 */

#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include "stdint.h"
/*
versiyon 0.3
20/05/2015
NET_Init() eklendi.
artık ip ayarları düzgün yapılıyor olmalı.
ancak NET_Init() içindeki debug bilgileri kaldırılacak,
sabit ip'ye bağlanıyor, main.cfg'den gelene değil.
aynı şey tcp_connect'de de var.

GPIO kısmı duzenlendi:

|  Port  | GPIO |  I/O   | PinNo |
|========|======|========|=======|
| Buzzer |  0   | Output |  11   |
|========|======|========|=======|
| Relay1 |  1   | Output |  12   |
|========|======|========|=======|
| Sense1 |  2   | Input  |  13   |
|========|======|========|=======|
| Relay2 |  3   | Output |  15   |
|========|======|========|=======|
| Sense2 |  4   | Input  |  16   |
|========|======|========|=======|

COM_avail artık tam olarak milli second cinsinden calisiyor.

***************************
versiyon 0.4
25/05/2015
seri port 115200 ile çalışıyor,
ancak hala COM_init içinde /dev/ttyS2 -- /dev/ttyS0 kısmı duzeltilmedi

CTRL+C ile programdan çıkılıyor

reader.c ve personel.c icinde eskiden kalan POS/PinPad fonksiyonları temizlendi.

***************************
versiyon 0.41
26/05/2015
kodda duzeltmeler yapılmaya baslandı
şimdilik sadece bir tane adam gibi timer eklendi.
ama henuz kullanılmıyor herhangi bir yerde.
bunu belki zamanis.c içine monte etmek lazım.

***************************
versiyon 0.42
11/06/2015
smm kısımları düzeltildi.
sadece 1 tane okuyucu olması lazım, ikinci portta yazıcı olabilir.
yazıcı kısımlarına henuz bakılmadı.
mercure terminalin 6.4.0 versiyonuna kadar olan kısmı var.
6.5.0, 6.5.1, 6.5.3, 6.6.0, 6.7.0, 6.8.0 ve son olarak kou 6.9.0 eklenmeli

***************************
versiyon 0.43
18/06/2015
tuş takımı kullanılmaya başlandı.
keyb.c ve keyb.h eklendi.
keypad şu şekilde

        C1  C2  C3  C4  C5
    R1| X | X | X | X | X |
    R2| X | X | X | X | X |
    R3| X | X | X | X | X |
    R4| X | X | X | X | X |

 C1|C2|C3|C4|C5|R4|R3|R2|R1|NC

Yeni GPIO bağlantıları şu şekilde oldu:
GPIO-7 1-wire olarak ayarlı sanki, o yuzden kullanılamadı.

|  Port   | GPIO |  I/O   | PinNo | TFT-LCD |
|=========|======|========|=======|=========|
| Column1 |  14  | Output |  23   |    X    |
|=========|======|========|=======|=========|
| Column2 |  0   | Output |  11   |    X    |
|=========|======|========|=======|=========|
| Column3 |  2   | Output |  13   |    X    |
|=========|======|========|=======|=========|
| Column4 |  3   | Output |  15   |    X    |
|=========|======|========|=======|=========|
| Column5 |  12  | Output |  19   |    X    |
|=========|======|========|=======|=========|
| Relay1  |  13  | Output |  21   |    X    |
|=========|======|========|=======|=========|
| Relay2  |  14  | Output |  23   |         |
|=========|======|========|=======|=========|
| Buzzer  |  21  | Output |  29   |         |
|=========|======|========|=======|=========|
| Row1    |  6   | Input  |  22   |         |
|=========|======|========|=======|=========|
| Row2    |  5   | Input  |  18   |         |
|=========|======|========|=======|=========|
| Row3    |  4   | Input  |  16   |    X    |
|=========|======|========|=======|=========|
| Row4    |  1   | Input  |  12   |         |
|=========|======|========|=======|=========|
| Sense1  |  10  | Input  |  24   |    X    |
|=========|======|========|=======|=========|
| Sense2  |  11  | Input  |  26   |    X    |
|=========|======|========|=======|=========|

admin.c duzeltildi, ayarlar yapılabiliyor artık.

***************************
versiyon 0.44
22/06/2015
keypad işlerinde interrupt'a geçildi.
admin.c pos'lardaki hale cevrildi.
artık tum ayarlar duzgun olarak yapılabiliyor.

***************************
versiyon 0.50
29/06/2015
gtk+3.0 ui yapılmaya başlandı.

***************************
versiyon 0.52
02/07/2015
admin.c hariç ekranlar duzeltildi.
roelyi cektikten sonra fazladan 3 sn kadar bekliyor gibi
oralara bakılmalı, ayrıca neden surekli role pinini high yapıyor,
gpio ayarlarına bakılmalı.
admin için veri girişi kısmı yapılmalı,
gtk_entry_new() ile yapılabilir belki???

***************************
versiyon 0.53
03/07/2015
admin.c duzeltilmeye baslandi.

***************************
versiyon 0.54
03/07/2015
admin.c duzenlendi.
ses softtonewrite'dan buzzer'a (digitalwrite) cevrildi.
master key okuma duzeltildi,
mesajlar duzeltildi.

logo işi olmadı
label'ın yanına koymak lazım ama
cancel_logo = gtk_image_new_from_file("cancel2.png");
gtk_layout_put(GTK_LAYOUT(main_layout), cancel_logo, 25,110);

gpio bağlantıları ekrana ggöre değiştirilmeli

bakiye yetersizken infobarr'ı geç update ediyor, eski yazı kalıyor.

***************************
versiyon 0.55
22/07/2015
lcd ekran icin gpio baglantilari duzenlendi.
lcd 3-5-7-8-10-22 numarali bacaklari kullanmiyor,
ayrica 12-16-18 numarali bacaklarda KEY1-2-3 var,
simdilik kullanmayacagim.
UPS icin, 11-13-15. pinler kullanilmis, ben degistirdim
22-32-36 numarali pinleri kullanacagim.
http://odroid.com/dokuwiki/doku.php?id=en:odroid-ups2
adresindeki ups2.sh scripti buna gore duzeltilmeli.

        C1  C2  C3  C4  C5
    R1| X | X | X | X | X |
    R2| X | X | X | X | X |
    R3| X | X | X | X | X |
    R4| X | X | X | X | X |

 C1|C2|C3|C4|C5|R4|R3|R2|R1|NC


|  Port   |Wiring|  I/O   | PinNo | TFT-LCD |
|=========|======|========|=======|=========|
| Column1 |  26  | Output |  32   |         |
|=========|======|========|=======|=========|
| Column2 |  27  | Output |  36   |         |
|=========|======|========|=======|=========|
| Column3 |  7   | Output |  7    |         |
|=========|======|========|=======|=========|
| Column4 |  21  | Output |  29   |         |
|=========|======|========|=======|=========|
| Column5 |  22  | Output |  31   |         |
|=========|======|========|=======|=========|
| Relay1  |  23  | Output |  33   |         |
|=========|======|========|=======|=========|
| Buzzer  |  24  | Output |  35   |         |
|=========|======|========|=======|=========|
|=========|======|========|=======|=========|
| Row1    |  1   | Input  |  12   |  KEY1   |
|=========|======|========|=======|=========|
| Row2    |  4   | Input  |  16   |  KEY2   |
|=========|======|========|=======|=========|
| Row3    |  5   | Input  |  18   |  KEY3   |
|=========|======|========|=======|=========|
| Row4    |  6   | Input  |  22   |         |
|=========|======|========|=======|=========|
| Sense1  |      | Input  |       |         |
|=========|======|========|=======|=========|
|=========|======|========|=======|=========|
| AC_OK   |      | Output |       |         |
|=========|======|========|=======|=========|
| BAT_OK  |      | Output |       |         |
|=========|======|========|=======|=========|
| LATCH   |      | Input  |       |         |
|=========|======|========|=======|=========|

touchscreen'i çalıştırmak için
1) sudo su
2) wget http://respectech.com/odroid/c1-touch.sh
3) chmod 755 c1-touch.sh
4) ./c1-touch.sh

touchscreen'den hdmi ekrana dönmek için yapılması gerekenler:
1) /usr/shar/X11/xorg.conf.d/99-odroidc-tftlcd.conf rename edilecek
2) /media/boot/boot.ini sonundaki fbcon=map:22 comment out edilecek
3) /etc/modules son iki satırı (spicc ve fbtft_device..) comment out edilecek
4) reboot edilecek.

şayet Lubuntu Desktop çalışmaz da odroid için password sorarsa:
1) ssh ile bağlan,
2) sudo su
3) cd ~odroid
4) chown odroid:odroid .*

***************************
versiyon 0.56
13/08/2015

ncurses ile yazılan yazılar duzeltildi

**************************
versiyon 0.60
20/08/2015

ekran tipi secmeli hale getirildi:
ekran.ini
1: ncurses (xterm)
2: 128x64
4: 320x240
8: 800x480

3: 128x64 + ncurses
5: 320x240 + ncurses
9: 800x480 + ncurses

default ncurses
TIP: 9

800x480 icin ayarlar:
/media/boot/boot.ini içindeki değişiklikler:
1)  setenv m "800x480p60hz" 	# 800x480 açılacak
    # setenv m "1080p"                # 1080P@60Hz kapanacak
2)  # HDMI DVI Mode Configuration
    #setenv vout_mode "hdmi"
    setenv vout_mode "dvi"
3)  # HDMI BPP Mode
    # setenv m_bpp "32"
    setenv m_bpp "24"
    # setenv m_bpp "16"

**************************
versiyon 0.61
22/08/2015

buyuk ve kucuk ekranlara gore font size'ları ayarlandı

**************************
versiyon SOLIDUS 1.0.0
18/11/2015
Gumushane için yapılan versiyon


        C1  C2  C3  C4  C5
    R1| X | X | X | X | X |
    R2| X | X | X | X | X |
    R3| X | X | X | X | X |
    R4| X | X | X | X | X |

 C1|C2|C3|C4|C5|R4|R3|R2|R1|NC


|  Port   |Wiring|  I/O   | PinNo | TFT-LCD |
|=========|======|========|=======|=========|
| Column1 |  26  | Output |  32   |         |
|=========|======|========|=======|=========|
| Column2 |  27  | Output |  36   |         |
|=========|======|========|=======|=========|
| Column3 |  7   | Output |  7    |         |
|=========|======|========|=======|=========|
| Column4 |  21  | Output |  29   |         |
|=========|======|========|=======|=========|
| Column5 |  22  | Output |  31   |         |
|=========|======|========|=======|=========|
| Relay1  |  23  | Output |  33   |         |
|=========|======|========|=======|=========|
| Buzzer  |  24  | Output |  35   |         |
|=========|======|========|=======|=========|
| Relay2  |  13  | Output |  21   |         |
|=========|======|========|=======|=========|
|=========|======|========|=======|=========|
| Row1    |  1   | Input  |  12   |  KEY1   |
|=========|======|========|=======|=========|
| Row2    |  4   | Input  |  16   |  KEY2   |
|=========|======|========|=======|=========|
| Row3    |  5   | Input  |  18   |  KEY3   |
|=========|======|========|=======|=========|
| Row4    |  6   | Input  |  22   |         |
|=========|======|========|=======|=========|
| Sense1  |  0   | Input  |  11   |         |
|=========|======|========|=======|=========|
| Sense2  |  2   | Input  |  13   |         |
|=========|======|========|=======|=========|
| AC_OK   |      | Output |       |         |
|=========|======|========|=======|=========|
| BAT_OK  |      | Output |       |         |
|=========|======|========|=======|=========|
| LATCH   |      | Input  |       |         |
|=========|======|========|=======|=========|

**************************
versiyon SOLIDUS 1.0.1
24/11/2015
KAPI iken log gönderildiğinde 1 byte eksik kalıyordu, düzeltildi.
READER_TYPE 0 olarak kaldığı için barcode'un portundan veri göndermeye uğraşıyordu.
personel unvanı da ekrana yazılıyor.
tcp_connect'deki timeout değeri offline'da çok beklemesin diye 5 sn'den 1 sn'ye indirilmişti
ancak bağlantı sağlamak için yeterli olmadı bu süre, 2 sn'ye çıkartıldı.

**************************
versiyon SOLIDUS 1.0.2
24/11/2015
yemek fiyatı sabit olarak 0.01 TL kalmış, kaldırdım.
dusme ve yukleme de sabit olarak 1. okuyucu ile islem yapılıyordu, değişkene bağladım

**************************
versiyon SOLIDUS 1.0.3
24/11/2015
tcp_connect'deki timeout tekrar 5 sn yapıldı.

**************************
versiyon SOLIDUS 1.0.4
24/11/2015
tcp_connect'deki timeout tekrar 2 sn yapıldı.
barkod okuma kısmı ana ekrana aktarıldı
barkod okuyucu 1. okuyucu gibi davranıyor
gtk_window_set_keep_above TRUE yapıldı yeniden.
main.c'de tarih/saat set edilirse boot ediyor, edilmezse etmiyor.

**************************
versiyon SOLIDUS 1.0.5
25/11/2015
GPIO_Gecti ayarlar.ini'deki sens cevabına göre hareket ediyor artık

**************************
versiyon SOLIDUS 1.0.6
25/11/2015
PERSONEL_Lcd'de Personel.UNVAN hatalı yazılıyordu, düzeltildi
Barkod okutulduğunda TCP_Connect ile PORT_SERVER+10 ile bağlanırken
ziyaretci fişi basan poslarla çakışıyordu, o yüzden +20 yapıldı

**************************
versiyon SOLIDUS 1.0.7
25/11/2015
PERSONEL_IzinAl icinde de TCP_Connect çağrılıyor ve POS'larla çakışıyordu
o yüzden +10 yerine +20 yapıldı sunucu portu.
Kiosktan basılan ve 'K' ile başlayan barkodları da okur hale getirildi

**************************
versiyon SOLIDUS 1.1.0
25/11/2015
artık programdan çıkılabiliyor.

start-soli şu şekilde değiştirilmeli:

#!/bin/sh
while true; do sudo ./solidus; done

**************************
versiyon SOLIDUS 1.1.1
25/11/2015

offline log gonderirken log.txt yanlıs okunup bozuluyordu,
master_key hicbir yerde girilmemisti, config.c'ye eklendi

**************************
versiyon SOLIDUS 1.1.2
25/11/2015
LOG dosyaları ve sayac dosyası oluşturulurken absolute path girilmemişti,
hepsinin başına /home/odroid/Solidus/ eklendi

**************************
versiyon SOLIDUS 1.1.3
27/11/2015
kapi modundaki log dosyasi işlenirken hata vardı.
ayrıca açılışta versiyon ve ip gösteriyor
LTD yerine AŞ logosu kullanılamay başlandı (Utarit.png)

**************************
versiyon SOLIDUS 1.2.0
27/11/2015
HTTP sunucu eklendi.

http://192.168.17.225/OZEL?I=0
TERMINAL PROGRAMINDAN CIKILIYOR

http://192.168.17.225/OZEL?I=1&Y=2015&A=27&G=12&S=05&D=17&SA=41
TARIH SAAT AYARLANDI: 12/27/2015, 05:17:41

http://192.168.17.225/OZEL?I=2
27/11/2015, 05:01:01

http://192.168.17.225/OZEL?I=3
SOLI-TERM 1.2.0
TERMINAL IP: 192.168.17.225
SERVER IP: 192.168.17.21
SUBNETMASK: 255.255.255.0
GATEWAY: 192.168.17.17
SERVER PORT: 4444

http://192.168.17.225/OZEL?I=5&S=1
SENSE[1] DURUM=0

http://192.168.17.225/OZEL?I=6&R=1&W=750
ROLE[1] SURE=750 ms. OK

http://192.168.17.225/OZEL?I=6&R=1&W=1
ROLE[1] DURUM=1

http://192.168.17.225/OZEL?I=6&R=1&W=0
ROLE[1] DURUM=0

http://192.168.17.225/OZEL?I=7&Port=1
OKUYUCU [COM1] TEST: UM 1.3r

**************************
versiyon SOLIDUS 1.2.1
27/11/2015
http'den komut geldiğinde şayet karşı taraf cevabı okumadan bağlantıyı kaparsa
cevabı yazmak için bekleyip duruyor olabilirdi o yüzden cevap gönderimi
1000 usec timeoutlu hale getirildi.

**************************
versiyon SOLIDUS 1.2.2
27/11/2015
http'ye http://192.168.17.225/OZEL?I=99 eklendi.
terminali poweroff eder.

**************************
versiyon SOLIDUS 1.3.0
02/12/2015
main_solidus'un başına gtk_main_iteration() eklendi, artık ekran açılışta beyaz kalmıyor.
FTP_SERVER çıkarıldı, artık FTP dizininde FTP.END dosyası var mı diye bakıyor,
FTP.END dosyası varsa, ftp dizinindeki tüm dosyaları bin/Debug'a çekip,
(solidus dosyası varsa owner ve mod'unu değiştirip) programı kapıyor.
programın nihai olarak çalışacağı yere karar verilip, program içindeki path'ler düzeltilmeli,
ayrıca debug değil release versiyonuna compile edilmeli.

**************************
versiyon SOLIDUS 1.4.0
16/12/2015
/home/odroid/SOLIDUS dizininin altında HTTP.END diye bir dosya varsa
AYARLAR.INI dosyasını /home/odroid/Solidus altına çeker
HTTP_RESET'i 1 yapar, böylece program yeni ayarlar ile çalışmaya başlar.

**************************
versiyon SOLIDUS 1.4.1
25/12/2015
RESOURCE ile ilgili kısımlar kaldırılmaya başlandı.

**************************
versiyon SOLI-TERM 1.4.2
30/12/2015
fis basma işleri personel.c'den çıkartılıp
yazici.c/yazici.h içine alındı.
yazıcı tipi 1: custom, 2:bixolon, 3:bilecik_custom

**************************
versiyon SOLI-TERM 1.5.0
30/12/2015
soli-mer 7.6.4 versiyonu ile aynı hale getirildi
13/01/2016
keypad işlerinde interrupt'dan eski versiyona dönüldü


**************************
versiyon SOLI-TERM 1.6.0
29/01/2016
admin kartının 2. sektor 0. bloğunda SHUTDOWN yazıyorsa soli-term kapatılır,
REBOOT yazıyorsa, reboot edilir.

ayrıca solidus -v diye çağrılınca program versiyonunu söyler ve çıkar,
bu özellik browser üzerinden bağlanıldığında programın versiyonunu göstermeye yarar.

smm ile haberleşmesi iyileştirildi:
com_avail'e 2 ms bekleme eklendi + select'den once halt edildi + select'lerin bir kısmı kaldırıldı.

**************************
versiyon SOLI-TERM 1.6.1
01/02/2016

smm haberleşmesi düzeltmeleri:
com_avail'e eklenen süre 5 ms yapildi

**************************
versiyon SOLI-TERM 1.6.2
01/02/2016
/home/odroid/Solidus/FTP/FTP.START dosyası varsa buzzer öter ve infobar rengi mavi olur.


**************************
versiyon SOLI-TERM 1.7.0
01/02/2016
KAPI modunda çalışırken AYARLAR.INI içinde
[KEYPAD]
AKTIF=1
KART_SIFRE_AKTIF=1
ise smm.c içinde kartın 14|2 sektor/blokundan 0xFE key'i ile kart şifresini okur ve
personel.c içinde 4 basamaklı bu şifrenin girilmesi için 5 sn bekler.

ayarlar.ini içindeki admin_kart_no okutulursa admin menusune girer

kapı terminaliyken sadece online ya da offline izin verildiğinde log'a kaydeder,
sunucu bağlantısı var fakat zamanında cevap dönmezse offline izin verir,

karta üniversite key'i ile login olamazsa SOLİDUS KARTI DEĞİL diye hata verir

**************************
versiyon SOLI-TERM 1.7.1
04/02/2016
2 okuyucu ile düzgün çalışır hale getirildi.

**************************
versiyon SOLI-TERM 1.8.0
03/02/2016
KAPI modunda çalışırken RF-ID okuyucu kısmı eklendi.
AYARLAR.INI [OKUYUCU1] ve [OKUYUCU2] kısmına TIP=RF-ID, TIP=MIFARE eklendi.
bunlara ilave olarak aşağıdaki bölüm de eklendi:
[RF-ID]
MASK_AKTIF=1
MASK=701100037AF8

SFTP ile Ayarlar.php gönderilirse, web sayfasını yeniler,
/home/odroid/Solidus/FTP altındaki Ayarlar.php dosyasını
/var/www/html altına çeker, owner ve group'unu ayarlar.

SFTP ile eth0.cfg gönderilirse, network ayarlarını yeniler,
/home/odroid/Solidus/FTP altındaki eth0.cfg dosyasını
/etc/network/interfaces.d altına çeker ve terminali reboot eder

**************************
versiyon SOLI-TERM 1.8.1
08/02/2016
ekran ve seslerle ilgili düzenlemeler yapıldı

admin kartında once 2|1'de personel key'i ya da UTARIT-ADMIN yazıyor mu diye bakar,
sonra SHUTDOWN/REBOOT'u kontrol eder


**************************
versiyon SOLI-TERM 1.9.0
09/02/2016
admin kartında 1.8.1'de yapılan değişiklik şimdilik geri alındı

Okuyucular için röle ve senler ayrı ayrı seçilebilir hale getirildi.
bu özellik için Ayarlar.php'nin 1.9.0  versiyonu kullanılmalı.
AYARLAR.INI içinde
[OKUYUCU1]
ROLE_AKTIF=0 ise röle pasif,
R0LE_AKTIF=1 ise 1. röle,
ROLE_AKTIF=2 ise 2. röle kullanılır.
Aynısı sens ve okuyucu2 için de geçerlidir.


**************************
versiyon SOLI-TERM 1.9.1
15/02/2016
comport'dan data okurken en az 6 byte veri gelene kadar bekliyor


**************************
versiyon SOLI-TERM 1.9.2
26/02/2016
7-byte seri numaralı kartları da okur hale getirildi

**************************
versiyon SOLI-TERM 1.9.3
04/03/2016
7-byte seri numaralı kart da okusa  sadece soldan 4-byte'ını dikkate alır
bir önceki versiyonda 4 byte'lık yere 7-byte data yazmaya çalışıyordu, düzeltildi.

**************************
versiyon SOLI-TERM 1.9.4
23/03/2016
AYARLAR.INI'den SERVER PORT hatalı olarak okunmaya çalışılıyordu, düzeltildi.
SIGTERM raise edildikten sonra 1 sn bekleniyor.

**************************
versiyon SOLI-TERM 1.9.5
04/04/2016
Kapı Terminali olarak çalışırken online'da okuyucu numaraları bir fazla gidiyordu,
Ayrıca ekrana röle işlemlerinden sonra yazılıyordu, düzeltildi

**************************
versiyon SOLI-TERM 2.0.0
08/04/2016
CN56 okuyucular eklendi,
Tuş takımlı ya da tuş takımsız olarak web sayfasında/Ayarlar.Ini içinde seçilmeli
Okuyucu tipi CN-MFR ya da CN-KYB olacak.
Web sayfası olarak da 2.0.0 versiyon kullanılmalı
Yukleme ve Düşme işlemleri tamamen çıkartıldı (SOLI-POS ile yapılıyor)


**************************
versiyon SOLI-TERM 2.0.1
13/04/2016
CN56 Okuyucularda buzzer ve led yanlış okuyucuda çalışıyordu, düzeltildi.

**************************
versiyon SOLI-TERM 2.0.2
18/04/2016
OGUNKONTROLU AYARLAR.INI içinde yanlış okunuyordu, düzeltildi.
açılışta tarih/saat ayarlanamazsa, 500 ms bekleyip tekrar deniyor.

**************************
versiyon SOLI-TERM 2.0.3
15/05/2016
CN56B login karta login olamazsa bir kez daha deniyor.

**************************
versiyon SOLI-TERM 2.0.4
23/05/2016
Eğer kapı terminaliyse ve okuyuculardan herhangi birisi CN56 reader ise karta login olmak yerine
/home/odroid/Solidus/KARTLAR.DAT dosyasında kart numarasını arar (4-byte olarak), bulamazsa Admin Kart olarak login olmaya uğraşır.
eğer kart no KARTLAR.DAT dosyasında varsa normal işleyişe devam ederek sunucuya sorar vs.
Böylelikle artık şifre işlemleri anlamsız hale gelir, bu versiyonda CN56 okuyucu varsa şifreli çalışamaz.
Ayrıca ekrana personel ad/soyadı da basılamaz.


**************************
versiyon SOLI-TERM 2.0.5
24/05/2016
KAPI terminali değilse kartlar.dat'a bakmak yerine kartın içine login olup bilgilerri okuması gerekiyorken
bir önceki versiyonda unutulmuş, düzeltildi.

**************************
versiyon SOLI-TERM 2.0.6
24/05/2016
iki yerde terminal kapatılırken ya da reboot ederken comport kapatılmıyordu, düzeltildi.
ayrıca okuyuculardan herhangi birisi CN56 tipindeyse, Admin Menu kullanımı kaldırıldı (CN56 tuşları desteklemiyor diye).
ComPort açılamazsa da devam ediyor, gerekirse bunu düzelt, devam edemesin.

**************************
versiyon SOLI-TERM 2.1.0 BETA
25/05/2016
İşKur için KAPI modunda çalışırken, TC Kimlik Numarası ya da mfr kart ile çalışan versiyon.
kart okutulunca ya da F4 ile kimlik no girilince sunucudan cevap bekler
onay gelirse röle çeker, sens bekler, sens gelirse hareket isler,
gelmezse işlemez, sunucudan misafir yemeği için onay istenirse keypad'den onay bekler,
gelirse hareket işler, gelmezse işlemez.
offline kısmı yok henüz:

**************************
versiyon SOLI-TERM 2.1.0
30/05/2016
İşKur için KAPI modunda çalışırken, ISKUR.INI varsa kartno/tckimlik oradan okunur.
online/offline çalışır.
barkod 15 karakter olduğu için giden data ve log.txt yapısı değişti.


**************************
versiyon SOLI-TERM 2.1.1
06/06/2016
ISKUR için: kapı modunda ekrana basılan mesajlar düzeltildi.

**************************
versiyon SOLI-TERM 2.1.2
09/06/2016
ISKUR için: kapı modundayken barkod okuyucudan fiş okutulduysa
1. okuyucunun röle/sense'i ile işlem yapması sağlandı.


**************************
versiyon SOLI-TERM 2.1.3
01/07/2016
ISKUR için: kapı modundayken tckimlik ile giriş yapılınca röle çekmiyordu, düzeltildi
ayrıca tckimlik giriş süresi 15 sn'ye çıkartıldı.

**************************
versiyon SOLI-TERM 2.1.4
01/07/2016
ISKUR'a özel islemler (iskur.ini ile yapılanlar) sadece  ISKUR için yapılır hale geldi.
Böylelikle KAPI modunda çalışırken ISKUR.INI yoksa misafir yemeği vs gibi konulara girilmiyor,
doğrudan PDKS_IZINAL ve PDKS_HAREKETISLE yapılıyor.

**************************
versiyon SOLI-TERM 2.2.0
13/07/2016
SMM okuyucularla ilgili iyileştirmeler yapıldı


**************************
versiyon SOLI-TERM 2.2.1
26/07/2016
AYARLAR.INI icinde
[TERMINAL]
AKTIF=0 ise terminal çalışmaz.

**************************
versiyon SOLI-TERM 2.2.2
03/10/2016
yemekhane terminaliyken karta yazamazsa log tutar.

**************************
versiyon SOLI-TERM 2.2.3
28/11/2016
personel/rezervasyon karsilamada ilk once rec_Personel sıfırlanıyor.

**************************
versiyon SOLI-TERM 2.2.5
14/12/2016
ADMIN Kartında DATETIME yazıyorsa terminalin tarih saatini ayarlayıp programı baştan başlatır.
Açılışta tarih saat ayarlanamazsa döngü içinde ayarlanmaya çalışılır, ayarlanınca tekrar denenmez.
Timer'lar değiştirildi.
if(syc_dk>(rec_TERM.TS_GUNCELLE_SAAT*60)) kontrolü == yapıldı.


**************************
versiyon SOLI-TERM 2.2.6
19/12/2016
TCKIMLIK alırken key=KEYB_getch(1); idi, key=KEYB_getch(0); yapıldı.

**************************
versiyon SOLI-TERM 2.2.7
26/12/2016
ADMIN kart ile tarih saat ayarlanamazsa, DATE_TIME_OK = 0 yapılıp,
sonraki her döngüde ayarlanmaya çalışılır.
ayarlar.ini'de set edilen zaman gelince tarih/saat ayarlamaya çalışır,
ayarlayamazsa yine her döngüde ayarlamaya çalışır.

**************************
versiyon SOLI-TERM 2.2.8
05/01/2017
SFTP ile relay1send.php ya da relay2send.php gönderilirse,
/home/odroid/Solidus/FTP altındaki relay1send.php ya da relay2send.php dosyasını
/var/www/html altına çeker, owner ve group'unu ayarlar.

**************************
versiyon SOLI-TERM 2.2.9
06/01/2017
SFTP ile relay1ON.php, relay1OFF.php, relay2ON.php ya da relay2OFF.php gönderilirse,
/home/odroid/Solidus/FTP altından /var/www/html altına çeker, owner ve group'unu ayarlar.
https://192.168.1.242/relay1ON.php bir numaralı röleyi çeker,
https://192.168.1.242/relay2ON.php iki numaralı röleyi çeker,
https://192.168.1.242/relay1OFF.php bir numaralı röleyi bırakır,
https://192.168.1.242/relay2OFF.php iki numaralı röleyi bırakır.

**************************
versiyon SOLI-TERM 2.3.0
09/02/2017
barkod okutulduktan sonra sıfırlanıyor.

**************************
versiyon SOLI-TERM 2.3.1
15/02/2017
zamanlı röle işlerini yapan programları ilgili yere kopyalayacak hale getirildi:
/home/odroid/Solidus/FTP altına zamanlirole ve relayrun isimli uygulamalar kopyalanırsa,
FTP.END dosyasını gördükten sonra, bu programları gerekli yere çeker, eski versiyonunu
kill eder, yeni çektiğini çalıştırır.

zamanlirole uygulaması /home/odroid/Solidus/RLY dizini altına çekilen *.RLY dosyalarını,
RLY.END dosyasını gördüğünde /home/odroid/Solidus altına çeker ve bu dosyalara göre röleleri kontrol eder.
0.RLY Pazar gününe, 1.RLY Pazartesi gününe,..., 5.RLY Cuma gününe, 6.RLY Cumartesi gününe aittir.

1.RLY
[00:00:00]
R1:50
R2:300
[12:00:15]
R1:3600
R2:0
[13:05:00]
R1:60
R2:60


her bir dosyada en fazla 30 tane saat tanımlı olmalıdır.
R1 birinci röle, R2 ikinci röle çekili kalma süresidir, en fazla 86400 olabilir (24 saate tekabül eder)

gün dönünce tüm röle zamanları iptal edilip, yeni günün zamanları set edilir.

zamanlirole programı RLY dosyasında yazan zamanlarda, relayrun programını çağırır:
relayrun r1 300& şeklinde çalıştırılan program birinci röleyi 300 sn çeker.

**************************
versiyon SOLI-TERM 2.3.2
22/02/2017
ISKUR'da offline'dayken barkod okutulunca ilkini onaylayıp,
ikinci okutmayı red etmesine rağmen, terminal online'a geçerse,
şayet offline data henüz gönderilmediyse yine onay veriyordu, düzeltildi.


**************************
versiyon SOLI-TERM 2.4.0
02/03/2017
KAPI modunda çalışırken TCP bekleme süresi ayarlanabilir yapıldı.
AYARLAR.INI içinde [SURELER] bölümüne
KAPI_TCP_BEKLEME_SURESI = 2 şeklinde ekleme yapıldı.
(KAPI modundayken TCP'den ceavp gelmesi için 2 sn bekler.)
Ayarlar.PHP'nin de 2.2.0 versiyonu kullanılmalı.

PERSONEL_TopUp() işlemi sadece yemekhane terminali ise yapılır hale getirildi.

KAPI modunda çalışırken kart numarası okunup, sadece Admin Kart ve Master Kart işlemleri yapılır,
karta login olmaya çalışılmaz, online olarak gönderilir.
offline'a düştüyse kartlar.dat dosyasından kart numaraları aratılır.
bulursa offline izin verildi der, bulamazsa izin verilmedi der.
iskur ise yemek işlemleri eskisindeki gibi yapılır.

loglar 10 sn yerine 5 sn'de bir gönderilmeye başlandı.
kapı modundayken tek seferde 3 kere log gönderir hale getirildi.

**************************
versiyon SOLI-TERM 2.4.1
08/03/2017
KAPI modundayken logların boyu ile ilgili sorun vardı.
şimdi şayet ISKUR ise log 38 byte, değil ise 33 byte olarak gidiyor.
Log_Run_Kapi() ve PDKS_AddLog() değiştirildi.


**************************
versiyon SOLI-TERM 2.4.2
10/03/2017
önceki versiyonda log dosyası boyu ile ilgili sorun çözülememişti.
şimdi düzeltildi.

**************************
versiyon SOLI-TERM 2.4.3
10/04/2017
sadece kapı modundayken kart numarası okuyup geçmesi gerekirken,
bütün durumlar için bunu yapıyor, kart bilgilerini, para vs, okumuyordu.
sadece kapı modunda kart no okur geçer hale getirildi,
diğer durumlarda para, yemek sektörleri vs de okunur.

**************************
versiyon SOLI-TERM 2.5.0
18/04/2017
LOG004.LOG ve LOG004.INF dosyaları ile 100.000 kayıtlık log tutuluyor.
bu loglar hiçbir yere gönderilmediğinden ve hiçbir zaamn silinmediğinden
100.000 kayıt dolduğunda üzerine yazılarak devam ediliyor.

**************************
versiyon SOLI-TERM 2.5.1
13/06/2017
kart okuyucu iyileştirmeleri yapıldı
2.2.0'da kaldırılan smm resetlemesi yeniden eklendi


**************************
versiyon SOLI-TERM 2.5.2
21/06/2017
KAPI modunda offline'da kart bulmaya çalışırken KARTLAR.DAT dosyasını açamıyordu.
sebebi maximum file descriptor sayısına ulaşılmasıymış:
1)socket açılıyor ama sunucuya bağlanılamayınca socket kapatılmıyormuş,
2)topup yaparken topup dosyası açılıyor ama kapatılmıyormuş,
3)yazıcıdan baskı alınırken yazıcı portu açılıyor, ama kapatılmıyormuş.
hepsi düzeltildi.

**************************
versiyon SOLI-TERM 2.5.3
03/07/2017
tcp_connect'de bir yerde socket kapatılmamıştı, kapatıldı.

**************************
versiyon SOLI-TERM 2.6.0
11/07/2017
yazıcıdan baskı alırken buffer overflow oluyordu, kontrol edilip duzeltildi

**************************
versiyon SOLI-TERM UVSS 3.0.0
27/07/2017
UVSS için değiştirilen versiyon.
Terminal Tipi Araç Altı eklendi.

Ayrıca eğer Dağıtık Anahtarlama Aktif ise tüm keyler kart numarası ile 3DES şifrelenerek kullanılır.
Böylelikle her kartın key'i kendisine özel olur.

Keypad aktif değilse tuş takımından TCKimlik No almaya uğraşmaz.
Eğer extra röle ve sens takılıysa, cihazın yemekhane ve kapı modunda olmaması gerekir,
çünkü bu modlarda personel_izinal çalışıyor ve kullanıcıdan misafir fiyatının onaylanması için
enter'a basması isteniyor. extra röle/sens varken tuş takımı kullanılamaz.
ayarlar.php'nin 3.1.0 versiyonu ve relay3send.php, ..., relay7send.php ile senseread.php'nin yeni versiyonu
terminalde /var/www/html dizini altında bulunmalı.
keypad'in column'ları röle (3-7), row'ları ise sense (3-6) olarak kullanılıyor.

*/

#pragma pack(push,1)
#define	PROG_VER "SOLI-TERM UVSS 3.0.0"


typedef struct
{
    uint16_t da_year;
    char 	da_day;
    char 	da_mon;
} struct_date;

typedef struct
{
    unsigned char ti_min;
    unsigned char ti_hour;
    unsigned char ti_hund;
    unsigned char ti_sec;
} struct_time;

typedef struct
{
    char			KARTNO[4]; //m8110 unsigned char idi
    struct_date		OKUMA_TARIH;
    struct_time		OKUMA_ZAMAN;
    char			ISLEM;//m8110 unsigned char idi
    uint32_t		BAKIYE;
    uint32_t		TUTAR;
}
struct_TCP_Para;

//typedef struct
//{
//	unsigned char 	KART_NO[4];
//	struct_date		OKUMA_TARIH;
//	struct_time		OKUMA_ZAMAN;
//	char			CEVAP;
//	char			CEVAP_ONLINE;
//	unsigned char	ISLEM;
//	uint32_t		BAKIYE;
//	uint32_t		TUTAR;
//}
//struct_LOG_PARA;

//struct_LOG_PARA	rec_LogPara;

//1.2.0
extern char HTTP_RESET;
extern char HTTP_RELAY;
//1.4.0
/*
extern int HTTPD_Init(void);
extern void HTTPD_Uninit(int s);
extern void HTTPD_Server(int s);
*/
//1.4.0
extern void HTTP_Kontrol(void);

extern char PERSONEL_ParaIsle(void);
extern uint16_t aStringToHex(unsigned char *strByte,unsigned int Len, unsigned char *strHex);//m8110 unsigned char idiler

//2.2.5
extern int main_TS_Guncelle(void);

//2.2.7
extern int DATE_TIME_OK;

#define 	ic_Izin_Tamam	0x01

/*
#define		KEY_BKSP 		0xFA
#define		KEY_ENTER   	0xF9
*/
//SOCKET tcp_Socket;
int tcp_Socket;

//extern char TCP_GetDateTime(struct_date *d, struct_time *t);
//extern char TCP_PersonelIzinAl(struct_PERSONEL * recPersonel,unsigned char *Response);
//extern char TCP_PersonelHareketIsle(struct_PERSONEL * recPersonel,unsigned char *Response);
//extern char TCP_PersonelParaIsle(struct_TCP_Para * recPara,unsigned char *Response);
//extern char TCP_LogPara(struct_LOG_PARA *recLOG);

//void Printer_Demo(void);
#pragma pack(pop)
#endif // MAIN_H_INCLUDED
