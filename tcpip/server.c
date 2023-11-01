/*
 * server.c
 */

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
#include <signal.h>
#include <sys/wait.h>

#include "serial.h"

char buffer[BUFSIZ];
int serial_port;
char serial_read_buf[256];

/*
 * this signal handler is used to catch the termination
 * of the child. Needed so that we can avoid wasting
 * system resources when "zombie" processes are created
 * upon exit of the child (as the parent could concievably
 * wait for the child to exit)
 */

void SigCatcher (int n)
{
    wait3 (NULL, WNOHANG, NULL);    
	signal (SIGCHLD, SigCatcher);
}

int main (int argc, char *argv[])
{
	int server_socket, client_socket;
	int client_len;
	struct sockaddr_in client_addr, server_addr;
	int len, i;
	FILE *p;


	if (argc != 2) {
		printf ("usage: ./server PORT_NUMBER\n");
		return 1;
	}	/* endif */

	/*
	 * install a signal handler for SIGCHILD (sent when the child terminates)
	 */

	signal (SIGCHLD, SigCatcher);

	/*
	 * obtain a socket for the server
	 */

	if ((server_socket = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
		printf ("grrr, can't get the server socket\n");
		return 1;
	}	/* endif */

	/*
	 * initialize our server address info for binding purposes
	 */

	memset (&server_addr, 0, sizeof (server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl (INADDR_ANY);
	server_addr.sin_port = htons (atoi(argv[1]));

	if (bind (server_socket, (struct sockaddr *)&server_addr, 
	sizeof (server_addr)) < 0) {
		printf ("grrr, can't bind server socket\n");
		close (server_socket);
		return 2;
	}	/* endif */

	/*
	 * start listening on the socket
	 */

	if (listen (server_socket, 5) < 0) {
		printf ("grrr, can't listen on socket\n");
		close (server_socket);
		return 3;
	}	/* endif */
    
    printf("Server startup complete\n\n");

	/*
	 * for this server, run an endless loop that will
	 * accept incoming requests from a remote client.
	 * the server will fork a child copy of itself to handle the
	 * request, and the parent will continue to listen for the
	 * next request
	 */

	while (1) {
		/*
		 * accept a packet from the client
		 */
        client_len = sizeof (client_addr);
		if ((client_socket = accept (server_socket, (struct sockaddr *)&client_addr, &client_len)) < 0) {
			printf ("grrr, can't accept a packet from client\n");
			close (server_socket);
			return 4;
		}	/* endif */

        printf("connection established to client with ID %d\n", client_addr.sin_addr.s_addr);
    
        serial_port = Serial_Open();
        if (serial_port == -1) {
            printf("Serial port did not open correctly\n");
            return 5;
        }
        printf("Serial port setup complete\n");

		/*
		 * fork a child
		 */

		if (fork() == 0) {
            while (1) {
                printf("waiting for command from client\n");
                memset(buffer, 0, sizeof(buffer));
                read(client_socket, buffer, BUFSIZ);
                // PROCESS COMMAND
                // Close client connection
                if (strcmp(buffer, "Q") == 0) {
                    strcpy(buffer, "Server connection closed\n");
                    len = strlen(buffer);
                    write (client_socket, buffer, len);
                    close (client_socket);
                    printf("Client connection closed\n");
                   
                    Serial_Close(serial_port);
                    printf("Serial port closed\n\n");
                    break;
                
                // Shutdown server
                }
                else if (strcmp(buffer, "shutdown") == 0 ) {
                    strcpy(buffer, "Server connection closed\n");
                    len = strlen(buffer);
                    write (client_socket, buffer, len);
                    close(client_socket);
                    
                    Serial_Close(serial_port);
                    printf("Serial port closed\n\n");
                    
                    shutdown(server_socket, SHUT_RDWR);
                    printf("Server shutdown complete\n");
                    return 0;

                // Send command to robot
                }
                else {
                    printf("tx: %s\n", buffer);
                    Serial_Write(serial_port, buffer);
                    Serial_Read(serial_port, buffer);
                    len = strlen(buffer);
                    printf("rx: %s\n", buffer);
                    write(client_socket, buffer, len);
                }
            }
		}
        else {
			/*
			 * this is done by parent ONLY
			 */
			close (client_socket);
		}	/* endif */
	}	/* end while */

	return 0;
}	/* end main */



