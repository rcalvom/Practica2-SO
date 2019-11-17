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
                _Bool existFile, request;
                char clientAnswer;
                long id = VerRegistro();
                s = send(clientfd, &MenuOption, sizeof(MenuOption), 0);
                printw("Enviando id al servidor...\n");
                s = send(clientfd, &id,sizeof(id),0);
                s = recv(clientfd, &existFile, sizeof(_Bool),0);
                if(existFile == 1){ // Si el archivo si existe.
                    printw("El registro con la id ingresada si existe en el sistema.\n");
                    printw("¿Desea abrirlo? (Y/N) . ");
                    scanf("%s",&clientAnswer);
                    if(clientAnswer == 'Y' || clientAnswer == 'y'){
                        FILE *file;
                        char *data;
                        int size;
                        request = 1;
                        s = send(clientfd, &request, sizeof(request), 0);
                        s = recv(clientfd, &size, sizeof(size), 0);
                        data = malloc(size);
                        bzero(data,size);
                        file = fopen("HistoriaClinica.dat","w+");
                        s = recv(clientfd,data,size,0);
                        fwrite(data,size,1,file);
                        fclose(file);
                        system("nano HistoriaClinica.dat");
                        file = fopen("HistoriaClinica.dat","r");
                        fread(data,size,1,file);
                        fseek(file,0L,SEEK_END);
                        fclose(file);
                        size = ftell(file);
                        s = send(clientfd,&size,sizeof(size),0);
                        s = send(clientfd,data,size,0);

                        
                        // Escribir archivo recibido, abrirlo con nano y enviarlo de regreso.

                    }else{
                        request = 0;
                        s = send(clientfd, &request, sizeof(request), 0);
                    }
                }else{
                    printw("El registro con la id dada no existe en el sistema.\n");
                }

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