CC:=gcc
CFLAGS:=
STRIP:=strip

all:
	$(CC) fanctl.c $(CFLAGS) -lwiringPi -o fanctl

install:
	$(STRIP) ./fanctl
	install -m 755 ./fanctl /usr/bin/fanctl

clean:
	rm -f ./fanctl
