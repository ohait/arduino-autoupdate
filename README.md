# arduino-autoupdate

* *`makefile`* to build new .bin files in console, and version them
* arduino *`au()`* function to fetch a new .bin file and autoupdate itself
* *`plack`* server to handle requests, compare versions, and returns .bin files or 304

### Makefile

I'll go ahead and build a new .bin file for my feather huzzah esp8266:

```
oha@raspberrypi:~/work/firmware/arduino/example $ make
sed -i "s/%VER%[0-9]*%/%VER%1522176805%/" autoupdate.h
/home/pi/Downloads/arduino-1.8.5/arduino --verify --board esp8266:esp8266:huzzah:CpuFrequency=80,FlashSize=4M1M,LwIPVariant=v2mss536,FlashErase=none,UploadSpeed=115200 --pref build.path=./build/ *.ino
Picked up JAVA_TOOL_OPTIONS:
Loading configuration...
Initialising packages...
Preparing boards...
Verifying...
Archiving built core (caching) in: /tmp/arduino_cache_723975/core/core_esp8266_esp8266_huzzah_CpuFrequency_80,FlashSize_4M1M,LwIPVariant_v2mss536,Debug_Disabled,DebugLevel_None____,FlashErase_none,UploadSpeed_115200_280111de7025b2af083c77835ebbed26.a
Sketch uses 264232 bytes (25%) of program storage space. Maximum is 1044464 bytes.
Global variables use 33820 bytes (41%) of dynamic memory, leaving 48100 bytes for local variables. Maximum is 81920 bytes.
oha@raspberrypi:~/work/firmware/arduino/example $
```

### Upload

The given example checks for a new version of the software only on book, so you need to reset it. Note: the first time you have to upload the image via usb connection, I'm assuming you already did.


```
INIT /home/oha/work/firmware/arduino/example/example.ino Mar 27 2018 18:50:54
connecting to MY_SSID...
connected: 192.168.1.102
[INFO] autoupdate firmware.oha.it:80 - name: example
[INFO] cmdline: HTTP/1.1 200 OK
[INFO] Server: nginx/1.10.3
[INFO] Date: Tue, 27 Mar 2018 18:54:10 GMT
[INFO] Content-Length: 268384
[INFO] update content-length: 268384
[INFO] Connection: close
[INFO] Author: oha[at]oha.it
[INFO] flashing .bin file
update stored 268384 bytes
Update successfully completed. Rebooting...


ets Jan  8 2013,rst cause:2, boot mode:(3,6)

load 0x4010f000, len 1384, room 16 
tail 8
chksum 0x2d
csum 0x2d
v614f7c32
@cp:0
ld

INIT /home/oha/work/firmware/arduino/example/example.ino Mar 27 2018 18:53:36
connecting to MY_SSID...
connected: 192.168.1.102
[INFO] autoupdate firmware.oha.it:80 - name: example
[INFO] cmdline: HTTP/1.1 304 Not Modified
[INFO] nothing to update
```

### Plack

here what happen on plack during the autoupdate:

```
oha@raspberrypi:~/work/firmware $ plackup
HTTP::Server::PSGI: Accepting connections at http://0:5000/

AU 'example' client: Tue Mar 27 18:50:43 2018, server: Tue Mar 27 18:53:25 2018
192.168.1.1 [Tue Mar 27 18:54:10 2018] 11.25ms 200 GET /autoupdate/example

AU 'example' client: Tue Mar 27 18:53:25 2018, server: Tue Mar 27 18:53:25 2018
304 Not Modified at /home/oha/work/firmware/app.psgi line 25.
192.168.1.1 [Tue Mar 27 18:54:27 2018] 8.27ms 304 GET /autoupdate/example
```

## How?

It's probably not the best way, but it works.

The `Makefile` modify autoupdate.h before compiling it, and marking a `%VER%123456%` string with the current timestamp (epoch)

After the arduino compiler finish, the string is baked into the .bin file, and available both to the `au()` function, and the plack server, so when a new .bin file is requested, plack can compare the given version to the last built, and refuse the download with a 304 if they match.

