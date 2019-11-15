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

    _Bool quit = false;
    int MenuOption;
    while(!quit){                                       //Mientras el usuario no pida salir, imprimir el menú.
        fflush(stdin);                        
        printw("\nPractica 1 - Sistemas Operativos 2019-II.\n\n");
        printw("1. Ingresar registro.\n");
        printw("2. Ver registro.\n");
        printw("3. Borrar registro.\n");
        printw("4. Buscar registro.\n");
        printw("5. Salir.\n\n");
        printw("Esperando opción: ");
        scanw("%hi",&MenuOption);
        printw("\n");

        //Evaluación de las opciones de menú.
        switch (MenuOption){
        case 1:
            
            break;
        case 2:
            
            break;
        case 3:
            
            break;
        case 4:
            
            break;
        case 5:
            
            break;
        case 31415:
            
            break;
        default: 
            printw("El valor \"%hi\" no es valido.\n\n",MenuOption);
            endwin();
            exit(-1);
            break;
        }
    }

    PressToContinue();
    DisposeConsole();
    return 0;
}