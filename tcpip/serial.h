// Serial header file

#ifndef SERIAL_H
#define	 SERIAL_H

int Serial_Open(void);
int Serial_Write(int serial_port, char* buf);
int Serial_Read(int serial_port, char* read_buf);
int Serial_Close(int serial_port);

#endif
