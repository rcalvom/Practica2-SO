#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "Functions.h"
#include "ShippingData.h"
#include "Opciones.h"

#define PORT 1234

int main(){

    InitConsole();                                                                          // Inicializa la consola de la libreria ncurses.
    char* ipAddress = malloc(15);
    printw("Bienvenido a la apliación cliente.\n\n");
    printw("Ingrese la dirección ip del servidor: ");
    scanw("%s", ipAddress);                                                                 // Solicita la dirección ip del servidor.

    int clientfd, r;
    struct sockaddr_in client;
    socklen_t len = sizeof(struct sockaddr);
    clientfd = socket(AF_INET,SOCK_STREAM,0);                                               // Se inicializa el socket.
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
    r = connect(clientfd,(struct sockaddr*)&client,len);                                    // Se conecta a el servidor.
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
    while(!quit){                                                                           // Mientras el usuario no pida salir, imprimir el menú.
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
            case 1:{                                                                        // Si se desea ingresar registro.
                bool answer;
                struct dogType new = IngresarRegistro();                                    
                printw("Enviando registro al servidor...\n");
                send(clientfd, &MenuOption, sizeof(MenuOption), 0);                         // Le envía la opcion al servidor.
                send(clientfd, &new, sizeof(new), 0);                                       // le envia la estructura con la nueva mascota.
                recv(clientfd, &answer, sizeof(answer), 0);                                 // Recibe una respuesta que indica si se pudo realizar la inserción correctamente.
                if(answer){
                    printw("Los datos de %s se han registrado correctamente.\n",new.name);
                }else{
                    printw("No se ha registrado a la mascota correctamente.\n");
                }
                break;
            }
            case 2:{                                                                        // Si se desea ver un registro.
                bool existFile, request = false;
                char clientAnswer;
                send(clientfd, &MenuOption, sizeof(MenuOption), 0);                         // Se envía la opción del menú.    
                long id = VerRegistro();
                printw("Enviando id al servidor...\n");
                send(clientfd, &id, sizeof(id), 0);                                         // Se envia la id al servidor.
                recv(clientfd, &existFile, sizeof(bool),0);                                 // Se recibe un booleano que indica si el archivo existe o no.

                if(existFile){                                                              // Si el archivo si existe.
                    printw("El registro solicitado existe en el sistema.\n");
                    printw("¿Desea abrirlo para editarlo? (Y/N). ");
                    scanw("%s",&clientAnswer);

                    if(clientAnswer == 'Y' || clientAnswer == 'y'){
                        request = true;
                    }

                    send(clientfd, &request, sizeof(request), 0);                           // Envía un booleando que indica si el usuario quiere ver el registro.

                    if(request){
                        FILE *file;
                        char* data;
                        long size;
                        recv(clientfd, &size, sizeof(size), 0);                             // Recibe el tamaño del archivo.
                        file = fopen("Registro.dat","w+");
                        data = malloc(size+1);
                        bzero(data,size);
                        recv(clientfd,data,size,0);                                         // Recibe el archivo.
                        fwrite(data,size,1,file);
                        fclose(file);
                        free(data);
                        system("nano Registro.dat");                                        // Abre el archivo con el comando bash "nano".
                        InitConsole();
                        file = fopen("Registro.dat","r");
                        fseek(file,0L,SEEK_END);
                        size = ftell(file);
                        rewind(file);
                        send(clientfd,&size,sizeof(size),0);                                // Envia el tamaño del archivo modificado.
                        data = malloc(size+1);
                        bzero(data,size);
                        fread(data,size,1,file);
                        send(clientfd,data,size,0);                                         // Envía el archivo modificado.
                        free(data);
                        fclose(file);
                        remove("Registro.dat");                                             // Elimina el archivo de la maquina cliente.
                    }                   
                }else{
                    printw("El registro con la Id dada no existe en el sistema.\n");
                }

                break;
            }
            case 3:{                                                                        // Si se desea borrar registro.
                long id, NumRegisters;
                send(clientfd,&MenuOption,sizeof(MenuOption),0);                            // Envía la opción del menú.
                recv(clientfd,&NumRegisters,sizeof(NumRegisters),0);                        // Recibe el número de registros en el sistema.
                printw("En el sistema hay %li registros.\n",NumRegisters);
                id = BorrarRegistro();
                if(id != -1){                                                               // Si el usuario indica una id...
                    bool answer = true;
                    send(clientfd,&answer,sizeof(bool),0);                                  
                    send(clientfd,&id,sizeof(id),0);                                        // Envia la id.
                    printw("Enviando id al servidor...\n");
                    recv(clientfd,&answer,sizeof(answer),0);                                // Recibe la confirmación si el registro fue borrado satisfactoriamente.
                    if(answer){
                        printw("El registro fue borrado satisfactoriamente.\n");
                    }else{
                        printw("El registro no ha podido ser borrado.\n");
                    }
                }else{                                                                      // Si el usuario cancela la operación.
                    bool answer = false;
                    send(clientfd,&answer,sizeof(bool),0);
                }
                break;
            }    
            case 4:{                                                                        // Si se desea buscar un registro.
                send(clientfd,&MenuOption,sizeof(MenuOption),0);
                char* name = BuscarRegistro();
                send(clientfd,name,32,0);                                                   // Envía el nombre de la mascota.
                long size;
                recv(clientfd,&size,sizeof(size),0);                                        // Recibe el tamaño de la lista a leer.
                char* search = malloc(size);
                bzero(search,size);
                recv(clientfd,search,size,0);                                               // Recibe la lista con las coincidencias encontradas.
                printw("%s",search);                                                        // Imprime la lista.
                free(search);
                break;
            } 
            case 5:{                                                                        // Si el usuario quiere salir del programa.
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