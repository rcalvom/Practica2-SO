#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <strings.h>
#include <unistd.h>
#include <stdbool.h>
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

    printw("\nSe ha logrado conectar al servidor correctamente.");
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
                bool answer;
                struct dogType new = IngresarRegistro();
                printw("Enviando registro al servidor...\n");
                send(clientfd, &MenuOption, sizeof(MenuOption), 0);
                send(clientfd, &new, sizeof(new), 0);
                recv(clientfd, &answer, sizeof(bool), 0);
                if(answer){
                    printw("Los datos de %s se han registrado correctamente.\n",new.name);
                }else{
                    printw("No se ha registrado a la mascota correctamente.\n");
                }
                break;
            }
            case 2:{
                bool existFile, request = false;
                char clientAnswer;
                send(clientfd, &MenuOption, sizeof(MenuOption), 0);   
                long id = VerRegistro();
                printw("Enviando id al servidor...\n");
                send(clientfd, &id, sizeof(id), 0);                
                recv(clientfd, &existFile, sizeof(bool),0);

                if(existFile){// Si el archivo si existe.
                    printw("El registro solicitado existe en el sistema.\n");
                    printw("¿Desea abrirlo para editarlo? (Y/N). ");
                    scanw("%s",&clientAnswer);

                    if(clientAnswer == 'Y' || clientAnswer == 'y'){
                        request = true;
                    }

                    send(clientfd, &request, sizeof(request), 0);


                    if(request){
                        FILE *file;
                        char* data;
                        long size;
                        recv(clientfd, &size, sizeof(size), 0);
                        file = fopen("Registro.dat","w+");
                        data = malloc(size);
                        bzero(data,size);
                        recv(clientfd,data,size,0);
                        fwrite(data,size,1,file);
                        fclose(file);
                        free(data);
                        system("nano Registro.dat");                // Abre el archivo.
                        InitConsole();
                        file = fopen("Registro.dat","r");
                        fseek(file,0L,SEEK_END);
                        size = ftell(file);
                        send(clientfd,&size,sizeof(size),0);
                        data = malloc(size);
                        bzero(data,size);
                        fread(data,size,1,file);
                        send(clientfd,data,size,0);
                        free(data);
                        fclose(file);
                    }                   
                }else{
                    printw("El registro con la id dada no existe en el sistema.\n");
                }

                break;
            }
            case 3:{
                long id, NumRegisters;
                send(clientfd,&MenuOption,sizeof(MenuOption),0);

                recv(clientfd,&NumRegisters,sizeof(NumRegisters),0);

                printw("En el sistema hay %li registros.\n",NumRegisters);

                id = BorrarRegistro();

                if(id != -1){
                    bool answer = true;
                    send(clientfd,&answer,sizeof(bool),0);
                    send(clientfd,&id,sizeof(id),0);
                    printw("Enviando id al servidor...\n");
                    recv(clientfd,&answer,sizeof(answer),0);
                    if(answer){
                        printw("El registro fue borrado satisfactoriamente.\n");
                    }else{
                        printw("El registro no ha podido ser borrado.\n");
                    }
                }else{
                    bool answer = false;
                    send(clientfd,&answer,sizeof(bool),0);
                }
                break;
            }    
            case 4:{
                send(clientfd,&MenuOption,sizeof(MenuOption),0);
                long id = BuscarRegistro();
                long size;
                recv(clientfd,&size,sizeof(size),0);
                char* search = malloc(size);
                recv(clientfd,search,size,0);
                printw("%s",search);
                free(search);
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