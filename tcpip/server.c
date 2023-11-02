/*
 * server.c
 */

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "serial.h"

#define ROBOT_STOP "S"

void communicate(int clientID, int serialID);
void sigCatcher(int n);

int quit;

int main(int argc, char* argv[]) {
    int serverSocket, clientSocket;
    unsigned int clientLen;
    struct sockaddr_in serverAddr, clientAddr;
    int serialPort;
    quit = 0;

    if (argc != 2) {
        printf("Usage: ./server PORT\n");
        return -1;
    }

    // Send when child terminates
//    signal(SIGCHLD, sigCatcher);

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        printf("[Server] Socket creation failed...\n");
        return -1;
    }
    else {
        printf("[Server] Socket creation successful...\n");
    }

    // Assign IP and PORT
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(atoi(argv[1]));

    // Bind socket to IP
    if ((bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr))) != 0) {
        printf("[Server] Socket bind failed...\n");
        close(serverSocket);
        return -1;
    }
    else {
        printf("[Server] Socket bind successful...\n");
    }
    
    // Open serial port
    serialPort = Serial_Open();
    if (serialPort == -1) {
        printf("[Server] Serial port did not open correctly...\n");
        return -1;
    }
    else {
        printf("[Server] Serial port opened...\n");
    }

    // Listen for client connection
    if ((listen(serverSocket, 5)) != 0) {
        printf("[Server] Server listen failed...\n");
        return -1;
    }
    else {
        printf("[Server] Server listening...\n");
    }

    clientLen = sizeof(clientAddr);

    while (quit != 1) {
        // Accept client communication
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientSocket < 0) {
            printf("[Server] Did not accept client...\n");
            return -1;
        }
        else {
            printf("[Server] Accepted client with ID %d...\n", clientAddr.sin_addr.s_addr);
        }
       
        // Communicate with client
//        if (fork() == 0 ) {
            communicate(clientSocket, serialPort);
 //       }

        close(clientSocket);
    }

    // Close the server
    close(serverSocket);
    Serial_Close(serialPort);

    printf("[Server] Closed successfully...\n");
    return 0;
}

void communicate(int clientID, int serialID) {
    char buf[BUFSIZ];
    FILE *p;
    
    while (1) {
        memset(buf, 0, sizeof(buf));
        read(clientID, buf, sizeof(buf));

        // Execute command from client
        if (strncmp("Q", buf, 4) == 0) {
            memset(buf, 0, sizeof(buf));
            strcpy(buf, "Q");
            write(clientID, buf, strlen(buf));
            printf("[Server] Closing connection...\n");
            Serial_Write(serialID, ROBOT_STOP);
            break;
        }
        else if (strncmp("shutdown", buf, 8) == 0) {
            memset(buf, 0, sizeof(buf));
            strcpy(buf, "shutdown");
            write(clientID, buf, strlen(buf));
            printf("[Server] Shutting down...\n");
            Serial_Write(serialID, ROBOT_STOP);
            quit = 1;
            break;
        }
        else {
            printf("[Server] cmd: %s\n", buf);
            Serial_Write(serialID, buf);
        }
    }
}

void sigCatcher(int n) {
    wait3(NULL, WNOHANG, NULL);
    signal(SIGCHLD, sigCatcher);
}
