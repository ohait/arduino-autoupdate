

ARDUINO=/home/pi/Downloads/arduino-1.8.5/arduino
BOARD=esp8266:esp8266:huzzah:CpuFrequency=80,FlashSize=4M1M,LwIPVariant=v2mss536,FlashErase=none,UploadSpeed=115200

DATE=$(shell date +%s)

all: build mqtt_build_pub

build: versiontag
	$(ARDUINO) --verify --board $(BOARD) --pref build.path=./build/ *.ino

versiontag:
	sed -i "s/%VER%[0-9]*%/%VER%$(DATE)%/" autoupdate.h

mqtt_build_pub:
	mosquitto_pub -t 'MyHome/sys/update' -m "*"

clean:
	rm -rf build
