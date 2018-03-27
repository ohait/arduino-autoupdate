

ARDUINO=/home/pi/Downloads/arduino-1.8.5/arduino
#BOARD=esp8266:esp8266:huzzah:CpuFrequency=80,FlashSize=4M1M,LwIPVariant=v2mss536,Debug=Disabled,DebugLevel=None____,FlashErase=none,UploadSpeed=115200
#BOARD=esp8266:esp8266:huzzah:CpuFrequency=80,FlashSize=4M1M,LwIPVariant=v2mss536
#BOARD=esp8266:esp8266:huzzah:CpuFrequency=80,FlashSize=4M1M
#BOARD=esp8266:esp8266:huzzah:CpuFrequency=80,FlashSize=4M3M
BOARD=esp8266:esp8266:huzzah:CpuFrequency=80,FlashSize=4M1M,LwIPVariant=v2mss536,FlashErase=none,UploadSpeed=115200

DATE=$(shell date +%s)

#all: clean build

build: versiontag
	$(ARDUINO) --verify --board $(BOARD) --pref build.path=./build/ *.ino

versiontag:
	sed -i "s/%VER%[0-9]*%/%VER%$(DATE)%/" autoupdate.h

clean:
	rm -rf build
