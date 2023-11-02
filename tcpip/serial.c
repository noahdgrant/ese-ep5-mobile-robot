// Reference and Credit:  https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/
// Find out more on this great tutorial site!

// C library headers
#include <stdio.h>
#include <string.h>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

#include "serial.h"

int Serial_Open(void) {
    // Open the serial port. Change device path as needed (currently set to an standard FTDI USB-UART cable type device)  
    int serial_port = open("/dev/ttyUSB0", O_RDWR);   
    // Other flags can be helpful too:  O_NONBLOCK for async comm.  
    // check https://www.gnu.org/software/libc/manual/html_node/Open_002dtime-Flags.html for the reference of all the available flags for opening a serial port

    // return a non-negative integer if the port is properly opened. otherwise it will return -1.

    // Create new termios struc, we call it 'tty' for convention
    struct termios tty;

    // Read in existing settings, and handle any error
    if(tcgetattr(serial_port, &tty) != 0) {
      printf("Error %i from tcgetattr: %s : %d\n", errno, strerror(errno), serial_port);
      return -1;
    }


    // Convention here:
    // "&= ~" is ANDing a negated bit, so it will clear the bit
    // "|=" is ORing a positive bit, so it will set the bit

    // ***Local Mode***

    tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
    tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
    tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size 
    tty.c_cflag |= CS8; // 8 bits per byte (most common)

    // default for project 5 - don't change these settings.

    //[RESEARCH]: The following flags are by default always set up this way, unless you are dealing with dial-up MODEM. Why?
    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)


    tty.c_lflag &= ~ICANON;  // [RESEARCH]: what is canonical mode mean?  When to set and when to clear?

    //[RESEARCH]: any impact if these echo-related flags are set or cleared in non-canonical mode?
    tty.c_lflag &= ~ECHO; // Disable echo
    tty.c_lflag &= ~ECHOE; // Disable erasure
    tty.c_lflag &= ~ECHONL; // Disable new-line echo

    //Often kept cleared as default.  
    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP


    // ***Input Mode***

    //Keep these settings in default to make sure we are working with raw data
    //[RESEARCH]: What do these flags do?
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes


    // ***Output Mode***

    //Keep these settings in default to make sure we are not sending out data with special character treatment.
    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed  
    // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
    // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

    //leave everything default up to here



    // ***Block Time Settings**

    tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 0;

    // VMIN = 0, VTIME = 0: 	No blocking, return immediately with what is available
    // VMIN > 0, VTIME = 0: 	This will make read() always wait for bytes 
    //							(exactly how many is determined by VMIN), so read() could block indefinitely.
    // VMIN = 0, VTIME > 0: 	This is a blocking read of any number chars with a maximum timeout (given by VTIME). 
    //							read() will block until either any amount of data is available, or the timeout occurs. 
    // VMIN > 0, VTIME > 0: 	Block until either VMIN characters have been received, or VTIME after first character has elapsed. 
    //							Note that the timeout for VTIME does not begin until the first character is received.


    // Set in/out baud rate to be 9600
    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);


    // Save tty settings, also checking for error
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
      printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
      return -1;
    }

    return serial_port;
}

int Serial_Write(int serial_port, char* buf) {

    write(serial_port, buf, sizeof(buf));
    return 0;
}

int Serial_Read(int serial_port, char* buf) {
    // Normally you wouldn't do this memset() call, but since we will just receive
    // ASCII data for this example, we'll set everything to 0 so we can
    // call printf() easily.
    memset(&buf, '\0', sizeof(buf));

    // Read bytes. The behaviour of read() (e.g. does it block?,
    // how long does it block for?) depends on the configuration
    // settings above, specifically VMIN and VTIME
    int num_bytes = read(serial_port, &buf, sizeof(buf));

    // n is the number of bytes read. n may be 0 if no bytes were received, and can also be -1 to signal an error.
    if (num_bytes < 0) {
      printf("Error reading: %s", strerror(errno));
      return -1;
    }

    return 0;
}

int Serial_Close(int serial_port) {
    close(serial_port);
    return 0;
}

