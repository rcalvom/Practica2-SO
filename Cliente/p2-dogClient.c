#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <strings.h>
#include "Functions.h"
#include "ShippingData.h"
#include "Opciones.h"

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
    char *answer = malloc(32);
    while(!quit){                                       //Mientras el usuario no pida salir, imprimir el menú.
        fflush(stdin);                        
        printw("Practica 2 - Sistemas Operativos 2019-II.\n\n");
        printw("1. Ingresar registro.\n");
        printw("2. Ver registro.\n");
        printw("3. Borrar registro.\n");
        printw("4. Buscar registro.\n");
        printw("5. Salir.\n\n");
        printw("Esperando opción: ");
        scanw("%hi",&MenuOption);
        printw("\n");
        bzero(answer, 32);
        int s = 0, l = 0;

        //Evaluación de las opciones de menú.
        if(MenuOption == 1){
            struct dogType new = IngresarRegistro();
            s = send(clientfd, &MenuOption, sizeof(MenuOption), 0);
            s += send(clientfd, &new, sizeof(new), 0);
            l = recv(clientfd, answer, 32, 0);
            printw("%s\n", answer);
        } else if(MenuOption == 2){
            long id = VerRegistro();
            s = send(clientfd, &MenuOption, sizeof(MenuOption), 0);
            s += send(clientfd, &id, sizeof(id), 0);
            //Y hay que recibir el archivo
            l = recv(clientfd, answer, 32, 0);
            printw("\n\n%s\n", answer);
        } else if(MenuOption == 3){
            long id = BorrarRegistro();
            s = send(clientfd, &MenuOption, sizeof(MenuOption), 0);
            s += send(clientfd, &id, sizeof(id), 0);
            l = recv(clientfd, answer, 32, 0);
            printw("\n\n%s\n", answer);
        } else if(MenuOption == 4){
            char *nombre = BuscarRegistro();
            s = send(clientfd, &MenuOption, sizeof(MenuOption), 0);
            s += send(clientfd, nombre, 32, 0);
            l = recv(clientfd, answer, 32, 0);
            printw("\n\n%s\n", answer);
            free(nombre);
        } else if(MenuOption == 5){
            quit = true;
        } else {
            printw("El valor \"%hi\" no es valido.\n\n",MenuOption);
        }
        printw("\nPulse cualquier tecla para continuar...\n");                  // Espera por una tecla para continuar.
        noecho();                                                               // Impide que se muestre el caracter en consola.
        getch();                                                                // Obtiene el caracter.
        echo();                                                                 // Permite de nuevo el ingreso de caracteres.
        clear();   
    }
    free(answer);
    PressToContinue();
    DisposeConsole();
    return 0;
}