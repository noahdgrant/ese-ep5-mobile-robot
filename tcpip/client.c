/*******************************************************************************
* Name: client.c (implementation)
* Author(s): Noah Grant & Wyatt Richard
* Date: October 30, 2023
* Description: joystick/gamepad events and displays them.
* Run: ./client 127.0.0.1 5000 /dev/input/jsX
* 127.0.0.1 ip address is used to refer to the current computer
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

// for Joystick:
#include <fcntl.h>
#include <linux/joystick.h>
#include <math.h>
#include "joystick.h"

char buffer[BUFSIZ];

void sendChar(int client_socket);

int main (int argc, char *argv[]) {
	int client_socket;
	struct sockaddr_in server_addr;
	struct hostent *host;

    // variables for joystick
    const char *device; // path to controller file
    int js;
    int angle;
    int prevAxisState[2] = {0};
    struct js_event event;
    struct axis_state axes[3] = {0};
    size_t axis;

    // ensure port and IP were entered
	if (argc < 3) {
		printf ("usage: ./client IP_ADDRESS PORT_NUMBER\n");
		return 1;
	}

    // check if controller file was specified
    if (argc > 3){
        device = argv[1];
    }
    else{
        device = "/dev/input/js2";
    }

    // Open Joystick
    js = open(device, O_RDONLY);
    if (js == -1){
        perror("Could not open joystick");
        return 1;
    }


    /*
     * get a socket for communications
     */

	if ((client_socket = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
		printf ("grrr, can't get a client socket!\n");
		return 3;
	}	/* endif */

	/*
	 * initialize struct to get a socket to host
	 */
	
	memset (&server_addr, 0, sizeof (server_addr));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons (atoi(argv[2]));

	/*
	 * attempt a connection to server
	 */

	if (connect (client_socket, (struct sockaddr *)&server_addr,
	sizeof (server_addr)) < 0) {
		printf ("grrr, can't connet to server!\n");
		close (client_socket);
		return 4;
	}	/* endif */

	/*
	 * now that we have a connection, get a commandline from
	 * the user, and fire it off to the server
	 */
    printf("Connected to server\n");
    while (read_event(js, &event) == 0) {
        switch (event.type){
            case JS_EVENT_BUTTON:
                //printf("Button %u %s\n", event.number, event.value ? "pressed" : "released");
                switch(event.number) {
                    case 0:     // Y Button
                        printf("Y %s\n", event.value ? "pressed" : "released");
                        break;
                    case 1:     // B Button
                        printf("B %s\n", event.value ? "pressed" : "released");
                        break;
                    case 2:     // A Button
                        printf("A %s\n", event.value ? "pressed" : "released");
                        break;
                    case 3:     // X Button
                        printf("X %s\n", event.value ? "pressed" : "released");
                        break;
                    case 4:     // L Button
                        printf("L %s\n", event.value ? "pressed" : "released");
                        break;
                    case 5:     // R Button
                        printf("R %s\n", event.value ? "pressed" : "released");
                        break;
                    case 6:     // ZL Button
                        printf("ZL %s\n", event.value ? "pressed" : "released");
                        if(event.value){
                            memset(buffer, 0, sizeof(buffer));
                            strcpy(buffer, "9");
                            sendChar(client_socket);
                            close (client_socket);
                        }
                        break;
                    case 7:     // ZR Button
                        printf("ZR %s\n", event.value ? "pressed" : "released");
                        if(event.value){
                            strcpy(buffer, "8");
                            sendChar(client_socket);
                            close (client_socket);
                        }
                        break;
                    case 8:     // - Button
                        printf("- %s\n", event.value ? "pressed" : "released");
                        break;
                    case 9:     // + Button
                        printf("+ %s\n", event.value ? "pressed" : "released");
                        break;
                    case 10:     // L3 Button
                        printf("L3 %s\n", event.value ? "pressed" : "released");
                        break;
                    case 11:     // R3 Button
                        printf("R3 %s\n", event.value ? "pressed" : "released");
                        if(event.value){
                            memset(buffer, 0, sizeof(buffer));
                            strcpy(buffer, "B");
                            sendChar(client_socket);
                            close (client_socket);
                        }
                        break;
                    case 12:     // Home Button
                        printf("Home %s\n", event.value ? "pressed" : "released");
                        if(event.value){
                            strcpy(buffer, "shutdown");
                            sendChar(client_socket);
                            close (client_socket);
                        }
                        break;
                    case 13:     // Capture Button
                        printf("Capture %s\n", event.value ? "pressed" : "released");
                        if(event.value){
                            strcpy(buffer, "Q");
                            sendChar(client_socket);
                            close (client_socket);
                        }
                        break;
                }
                break;
            case JS_EVENT_AXIS:
                axis = get_axis_state(&event, axes);
                if(axis == 0) { // left Joystick
                    if(axes[axis].x == 0 && axes[axis].y == 0){
                        printf("Stop moving\n"); // max value of 32767
                        strcpy(buffer, "A");
                        sendChar(client_socket);
                        prevAxisState[axis] = 0;

                    }
                    else{
                        angle = (int)(1000 * atan2((double)axes[axis].y, (double)axes[axis].x));

                        if(angle >=-392 && angle<=392 && prevAxisState[axis] != 1){

                            printf("Hard Right\n"); // max value of 32767
                            strcpy(buffer, "4");
                            sendChar(client_socket);
                            prevAxisState[axis] = 1;
                        }
                        else if(angle >392 && angle<1178 && prevAxisState[axis] != 2){
                            printf("Soft Right Backward\n"); // max value of 32767
                            strcpy(buffer, "7");
                            sendChar(client_socket);
                            prevAxisState[axis] = 2;
                        }
                        else if(angle >=1178 && angle<=1963 && prevAxisState[axis] != 3){
                            printf("Backward\n"); // max value of 32767
                            strcpy(buffer, "5");
                            sendChar(client_socket);
                            prevAxisState[axis] = 3;
                        }
                        else if(angle >1963 && angle<2748 && prevAxisState[axis] != 4){
                            printf("Soft Left Backward\n"); // max value of 32767
                            strcpy(buffer, "6");
                            sendChar(client_socket);
                            prevAxisState[axis] = 4;
                        }
                        else if((angle >=2748 || angle<=-2748) && prevAxisState[axis] != 5){
                            printf("Hard Left\n"); // max value of 32767
                            strcpy(buffer, "3");
                            sendChar(client_socket);
                            prevAxisState[axis] = 5;
                        }
                        else if(angle > -2748 && angle < -1963 && prevAxisState[axis] != 6){
                            printf("Soft Left Forward\n"); // max value of 32767
                            strcpy(buffer, "1");
                            sendChar(client_socket);
                            prevAxisState[axis] = 6;
                        }
                        else if(angle >=-1963 &&angle<=-1178 && prevAxisState[axis] != 7){
                            printf("Forward\n"); // max value of 32767
                            strcpy(buffer, "0");
                            sendChar(client_socket);
                            prevAxisState[axis] = 7;
                        }
                        else if(angle >-1178 && angle<-392 && prevAxisState[axis] != 8){
                            printf("Soft Right Forward\n"); // max value of 32767
                            strcpy(buffer, "2");
                            sendChar(client_socket);
                            prevAxisState[axis] = 8;
                        }

                    }
                }
                else if(axis == 1) { // right Joystick
                    if(axes[axis].x == 0 && axes[axis].y == 0){
                        printf("Stop moving\n"); // max value of 32767
                        strcpy(buffer, "G");
                        sendChar(client_socket);
                        prevAxisState[axis] = 0;
                    }
                    else{
                        angle = (int)(1000 * atan2((double)axes[axis].y, (double)axes[axis].x));

                        if(angle >=-2356 && angle<=-785 && prevAxisState[axis] != 1){
                            printf("Forward\n"); // max value of 32767
                            strcpy(buffer, "C");
                            sendChar(client_socket);
                            prevAxisState[axis] = 1;
                        }
                        else if(angle >-785 && angle<785 && prevAxisState[axis] != 2){
                            printf("Right\n"); // max value of 32767
                            strcpy(buffer, "F");
                            sendChar(client_socket);
                            prevAxisState[axis] = 2;
                        }
                        else if(angle >=785 && angle<=2356 && prevAxisState[axis] != 3){
                            printf("Backward\n"); // max value of 32767
                            strcpy(buffer, "D");
                            sendChar(client_socket);
                            prevAxisState[axis] = 3;
                        }
                        else if((angle >2356 || angle<-2356) && prevAxisState[axis] != 4){
                            printf("Left\n"); // max value of 32767
                            strcpy(buffer, "E");
                            sendChar(client_socket);
                            prevAxisState[axis] = 4;
                        }
                    }
                }

                break;
            default:
                /* Ignore init events. */
                break;
        }

        fflush(stdout);

        if(event.type == JS_EVENT_BUTTON && (event.number == 12 || event.number == 13 )&& event.value == 1){
            break;
        }

    }
    //send shutdown command
    close(js);
    return 0;
}	/* end main */

void sendChar(int client_socket){
    write (client_socket, buffer, strlen (buffer));
    memset(buffer, 0, sizeof(buffer));
    int len = read (client_socket, buffer, sizeof (buffer));
    printf ("Result of command:\n%s\n\n", buffer);
}