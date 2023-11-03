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
    }   /* endif */

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
    }   /* endif */

    /*
     * now that we have a connection, get a commandline from
     * the user, and fire it off to the server
     */
    while (read_event(js, &event) == 0) {
        if(event.type==JS_EVENT_AXIS){
            axis = get_axis_state(&event, axes);

            if(axis == 0) { // left Joystick
                if(axes[axis].x == 0 && axes[axis].y == 0){
                    printf("Stop moving\n"); // max value of 32767
                    strcpy(buffer, "A");
                    write (client_socket, buffer, strlen (buffer));
                    prevAxisState[axis] = 0;

                }
                else{
                    angle = (int)(1000 * atan2((double)axes[axis].y, (double)axes[axis].x));
                    if(angle >=-392 && angle<=392 && prevAxisState[axis] != 1){
                        printf("Hard Right\n"); // max value of 32767
                        strcpy(buffer, "4");
                        write (client_socket, buffer, strlen (buffer));
                        prevAxisState[axis] = 1;
                    }
                    else if(angle >392 && angle<1178 && prevAxisState[axis] != 2){
                        printf("Soft Right Backward\n"); // max value of 32767
                        strcpy(buffer, "7");
                        write (client_socket, buffer, strlen (buffer));
                        prevAxisState[axis] = 2;
                    }
                    else if(angle >=1178 && angle<=1963 && prevAxisState[axis] != 3){
                        printf("Backward\n"); // max value of 32767
                        strcpy(buffer, "5");
                        write (client_socket, buffer, strlen (buffer));
                        prevAxisState[axis] = 3;
                    }
                    else if(angle >1963 && angle<2748 && prevAxisState[axis] != 4){
                        printf("Soft Left Backward\n"); // max value of 32767
                        strcpy(buffer, "6");
                        write (client_socket, buffer, strlen (buffer));
                        prevAxisState[axis] = 4;
                    }
                    else if((angle >=2748 || angle<=-2748) && prevAxisState[axis] != 5){
                        printf("Hard Left\n"); // max value of 32767
                        strcpy(buffer, "3");
                        write (client_socket, buffer, strlen (buffer));
                        prevAxisState[axis] = 5;
                    }
                    else if(angle > -2748 && angle < -1963 && prevAxisState[axis] != 6){
                        printf("Soft Left Forward\n"); // max value of 32767
                        strcpy(buffer, "1");
                        write (client_socket, buffer, strlen (buffer));
                        prevAxisState[axis] = 6;
                    }
                    else if(angle >=-1963 &&angle<=-1178 && prevAxisState[axis] != 7){
                        printf("Forward\n"); // max value of 32767
                        strcpy(buffer, "0");
                        write (client_socket, buffer, strlen (buffer));
                        prevAxisState[axis] = 7;
                    }
                    else if(angle >-1178 && angle<-392 && prevAxisState[axis] != 8){
                        printf("Soft Right Forward\n"); // max value of 32767
                        strcpy(buffer, "2");
                        write (client_socket, buffer, strlen (buffer));
                        prevAxisState[axis] = 8;
                    }

                }
            }
            else if(axis == 1) { // right Joystick
                if(axes[axis].x == 0 && axes[axis].y == 0){
                    printf("Stop moving\n"); // max value of 32767
                    strcpy(buffer, "GH");
                    write (client_socket, buffer, strlen (buffer));
                    prevAxisState[axis] = 0;
                }
                else{
                    angle = (int)(1000 * atan2((double)axes[axis].y, (double)axes[axis].x));

                    if(angle >=-392 && angle<=392 && prevAxisState[axis] != 1){
                        printf("Pan Right\n"); // max value of 32767
                        strcpy(buffer, "EG");
                        write (client_socket, buffer, strlen (buffer));
                        prevAxisState[axis] = 1;
                    }
                    else if(angle >392 && angle<1178 && prevAxisState[axis] != 2){
                        printf("Right & Up\n"); // max value of 32767
                        strcpy(buffer, "DE");
                        write (client_socket, buffer, strlen (buffer));
                        prevAxisState[axis] = 2;
                    }
                    else if(angle >=1178 && angle<=1963 && prevAxisState[axis] != 3){
                        printf("Up\n"); // max value of 32767
                        strcpy(buffer, "DH");
                        write (client_socket, buffer, strlen (buffer));
                        prevAxisState[axis] = 3;
                    }
                    else if(angle >1963 && angle<2748 && prevAxisState[axis] != 4){
                        printf("Left & Up\n"); // max value of 32767
                        strcpy(buffer, "FD");
                        write (client_socket, buffer, strlen (buffer));
                        prevAxisState[axis] = 4;
                    }
                    else if((angle >=2748 || angle<=-2748) && prevAxisState[axis] != 5){
                        printf("Pan Left\n"); // max value of 32767
                        strcpy(buffer, "FG");
                        write (client_socket, buffer, strlen (buffer));
                        prevAxisState[axis] = 5;
                    }
                    else if(angle > -2748 && angle < -1963 && prevAxisState[axis] != 6){
                        printf("Left & Down\n"); // max value of 32767
                        strcpy(buffer, "FC");
                        write (client_socket, buffer, strlen (buffer));
                        prevAxisState[axis] = 6;
                    }
                    else if(angle >=-1963 &&angle<=-1178 && prevAxisState[axis] != 7){
                        printf("Down\n"); // max value of 32767
                        strcpy(buffer, "CH");
                        write (client_socket, buffer, strlen (buffer));
                        prevAxisState[axis] = 7;
                    }
                    else if(angle >-1178 && angle<-392 && prevAxisState[axis] != 8){
                        printf("Right & Down\n"); // max value of 32767
                        strcpy(buffer, "CE");
                        write (client_socket, buffer, strlen (buffer));
                        prevAxisState[axis] = 8;
                    }
                }
            }
            else if(axis == 2) { // right Joystick
                if(axes[axis].x == 0 && axes[axis].y == 0){
                    printf("Stop moving\n"); // max value of 32767
                    prevAxisState[axis] = 0;
                }
                else if(axes[axis].y == -32767 && prevAxisState[axis] != 1){
                    printf("Increase Speed\n"); // max value of 32767
                    strcpy(buffer, "8");
                    write (client_socket, buffer, strlen (buffer));
                    prevAxisState[axis] = 1;
                }
                else if (axes[axis].y == 32767 && prevAxisState[axis] != 2){
                    printf("Decrease Speed\n"); // max value of 32767
                    strcpy(buffer, "9");
                    write (client_socket, buffer, strlen (buffer));
                    prevAxisState[axis] = 2;
                }
                else if (axes[axis].x == 32767 && prevAxisState[axis] != 3) {
                    printf("Centre camera\n"); // max value of 32767
                    strcpy(buffer, "B");
                    write(client_socket, buffer, strlen(buffer));
                    prevAxisState[axis] = 3;
                }
            }
        }

        if(event.type == JS_EVENT_BUTTON && (event.number == 12 || event.number == 13 )&& event.value == 1){
            strcpy(buffer, "shutdown");
            write (client_socket, buffer, strlen (buffer));
            close (client_socket);
            break;
        }

    }
    //send shutdown command
    close(js);
    return 0;
}   /* end main */
