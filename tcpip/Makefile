# Server and Client w/ joystick Makefile

all: server client

server: server.c serial.c
client: client.c joystick.c -lm

clean:
	rm -f server
	rm -f client

remake:
	make clean
	make
