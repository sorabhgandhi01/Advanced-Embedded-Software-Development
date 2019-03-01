#
# If cross-compiling, CC must point to your cross compiler, for example:
# make CC=arm-linux-gnueabihf-gcc

#CFLAGS := -c -Wall 
#CFLAGS := -c -Wall -I../inc
LOCAL_CFLAGS ?= -Wall -g 
PROGS := posix_timer

all: $(PROGS)

posix_timer: posix_timer.c
	$(CC) $(CFLAGS) $(LOCAL_CFLAGS) $(LDFLAGS) $^ -o $@ -pthread -lrt


clean:
	rm -f *.o
	rm -f $(PROGS)

#install:
#	cp $(PROGS) $(TARGET_DIR)/usr/local/bin
