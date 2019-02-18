LDIR=$(HOME)/APES/buildroot/output/build/linux-4796173fc58688055a99a1cef19a839174067220/

CDIR=$(HOME)/APES/buildroot/output/host/bin/


obj-m +=my_timer.o

all:
	make ARCH=arm CROSS_COMPILE=$(CDIR)arm-buildroot-linux-uclibcgnueabihf- -C $(LDIR) M=$(shell pwd) modules

clean:
	make -C $(LDIR) M=$(shell pwd) clean
