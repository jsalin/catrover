CC=gcc

all: cmdmotor catservice

cmdmotor: cmdmotor.c config.h
	$(CC) cmdmotor.c -o cmdmotor -lwiringPi
	strip cmdmotor
	./owner.sh

catservice: catservice.c config.h
	$(CC) catservice.c -o catservice -lwiringPi

install: cmdmotor catservice
	sudo cp -vf cmdmotor /usr/local/bin
	sudo cp -vf scripts/* /usr/lib/cgi-bin
	sudo cp -vrf index.html favicon.ico images /var/www
	sudo cp -vf catservice delayed_hdmi_off.sh start_catrover.sh public_ip.sh /usr/local/sbin
	sudo cp -vf wlan-up /etc/network/if-up.d

test:
	./test.sh

clean:
	rm -f cmdmotor catservice *.o
