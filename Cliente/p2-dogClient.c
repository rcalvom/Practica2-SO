#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <strings.h>
#include "Functions.h"

#define PORT 1234

int main(){
    InitConsole();

    char* ipAddress = malloc(15);
    
    printw("Bienvenido a la apliación cliente.\n\n");
    printw("Ingrese la dirección ip del servidor: ");
    scanw("%s",ipAddress);                              // Solicita la dirección ip del servidor.

    int clientfd,r ;                                    // Se declaran variables.
    struct sockaddr_in client;
    socklen_t len = sizeof(struct sockaddr);
    clientfd = socket(AF_INET,SOCK_STREAM,0);           // Se inicializa el socket.
    if(clientfd == -1){
        printw("El socket no pudo ser creado.\n");
        PressToContinue();
        DisposeConsole();
        exit(EXIT_FAILURE);
    }
    client.sin_family = AF_INET;
    client.sin_port = htons(PORT);
    client.sin_addr.s_addr = inet_addr(ipAddress);    
    bzero(client.sin_zero,8);
    r = connect(clientfd,(struct sockaddr*)&client,len); // Se conecta a el servidor.
    if(r == -1){
        printw("No se pudo conectar al servidor en la dirección ip indicada.\n");
        PressToContinue();
        DisposeConsole();
        exit(EXIT_SUCCESS);
    }







    PressToContinue();
    DisposeConsole();
    return 0;
}