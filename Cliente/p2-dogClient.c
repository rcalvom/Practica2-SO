#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <strings.h>
#include <unistd.h>
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

    printw("Se ha logrado conectar al servidor correctamente.");
    PressToContinue();

    _Bool quit = false;
    int MenuOption;
    while(!quit){                                       //Mientras el usuario no pida salir, imprimir el menú.
        fflush(stdin);                        
        printw("Practica 2 - Sistemas Operativos 2019-II.\n\n");
        printw("1. Ingresar registro.\n");
        printw("2. Ver registro.\n");
        printw("3. Borrar registro.\n");
        printw("4. Buscar registro.\n");
        printw("5. Salir.\n\n");
        printw("Esperando opción: ");
        scanw("%i",&MenuOption);
        printw("\n");
        int s = 0, l = 0;

        //Evaluación de las opciones de menú.
        switch (MenuOption){
            case 1:{
                _Bool answer;
                struct dogType new = IngresarRegistro();
                printw("Enviando registro al servidor...\n");
                s = send(clientfd, &MenuOption, sizeof(MenuOption), 0);
                s += send(clientfd, &new, sizeof(new), 0);
                l = recv(clientfd, &answer, sizeof(_Bool), 0);
                if(answer){
                    printw("Los datos de %s se han registrado correctamente.\n",new.name);
                }else{
                    printw("No se ha registrado a la mascota correctamente.\n");
                }
                break;
            }
            case 2:{
                /*long id = VerRegistro();
                s = send(clientfd, &MenuOption, sizeof(MenuOption), 0);
                s += send(clientfd, &id, sizeof(id), 0);
                //Y hay que recibir el archivo
                l = recv(clientfd, answer, 32, 0);
                printw("\n\n%s\n", answer);*/
                break;
            }
            case 3:{
                /*long id = BorrarRegistro();
                s = send(clientfd, &MenuOption, sizeof(MenuOption), 0);
                s += send(clientfd, &id, sizeof(id), 0);
                l = recv(clientfd, answer, 32, 0);
                printw("\n\n%s\n", answer);*/
                break;
            }    
            case 4:{
                /*char *nombre = BuscarRegistro();
                s = send(clientfd, &MenuOption, sizeof(MenuOption), 0);
                s += send(clientfd, nombre, 32, 0);
                l = recv(clientfd, answer, 32, 0);
                printw("\n\n%s\n", answer);
                free(nombre);*/
                break;
            } 
            case 5:{
                quit = true;
                break;
            }                   
            
            default:{
                printw("El valor \"%hi\" no es valido.\n\n",MenuOption);
            }
                
        }
        PressToContinue();
    }
    DisposeConsole();
    return 0;
}