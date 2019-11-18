#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <strings.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include "Functions.h"
#include "Hash.h"
#include "ShippingData.h"
#include "Opciones.h"

#define PORT 1234
#define BACKLOG 32

// Estructura que almacena los datos del cliente.
struct Client{
    int clientfd;
    pthread_t idThread;
    struct in_addr Ip;
};

// Variables globales.
int serverfd;
socklen_t len;
int CurrentUsers;
struct Client* clientsConnected[BACKLOG];
struct HashTable Table;

// Método para el hilo que se encarga de recibir datos de los clientes y ejecutar las solicitudes.
void* ListenRequest(void* args){
    struct Client *Client = (struct Client*) args;
    printf("Escuchando peticiones del cliente %s ...\n\n",inet_ntoa(Client->Ip));
    while(true){                                   
        int option;                                  
        recv(Client->clientfd, &option, sizeof(int), 0);                         // Recibe la opción del menú dada por el usuario.
        switch (option){
            case 1:{                        // Si la opción del cliente es Ingresar Registro.
                struct dogType *new = malloc(sizeof(struct dogType));
                bzero(new,sizeof(struct dogType));
                recv(Client->clientfd,new,sizeof(struct dogType),0);
                bool flag = IngresarRegistro(&Table,new);
                send(Client->clientfd,&flag,sizeof(flag),0);
                if(flag){
                    WriteLog(1,inet_ntoa(Client->Ip),new->name);
                    printf("Inserción de %s por el cliente %s correctamente...\n",new->name,inet_ntoa(Client->Ip));
                }               
                free(new);
                option = 0;
                break;
            }
            case 2:{                        // Si la opción del cliente es Ver Registro.
                long idRegister;
                recv(Client->clientfd,&idRegister,sizeof(idRegister),0);
                bool existFile = ExisteRegistro(idRegister);
                send(Client->clientfd,&existFile,sizeof(existFile),0);
                if(existFile){
                    bool answer;
                    
                    recv(Client->clientfd,&answer,sizeof(bool),0);
                    if(answer){
                        FILE *file;
                        char* data;
                        file = fopen(FilePath(idRegister),"r");
                        fseek(file,0L,SEEK_END);
                        long size = ftell(file);
                        send(Client->clientfd,&size,sizeof(size),0);
                        data = malloc(size);
                        bzero(data,size);
                        fread(data,size,1,file);
                        fclose(file);
                        send(Client->clientfd,data,size,0);
                        free(data); 
                        recv(Client->clientfd,&size,sizeof(size),0);
                        data = malloc(size);
                        bzero(data,size);
                        recv(Client->clientfd,data,size,0);
                        file = fopen(FilePath(idRegister),"w+");
                        fwrite(data,size,1,file);
                        fclose(file);
                        free(data);
                        char* id = malloc(10);
                        sprintf(id,"%li",idRegister);
                        WriteLog(2,inet_ntoa(Client->Ip),id);
                        free(id);
                    }
                }
                option = 0;
                break;
            }
            case 3:{                        // Si la opción del cliente es Borrar Registro.
                break;
            }
            case 4:{                        // Si la opción del cliente es Buscar Registro.
                break;
            }
        }
    }
}

//Método para hilo que se encarga de cerrar el proceso si el usuario lo solicita.
void* ListenExit(void* client){
    char exitKey;
    while(true){
        scanf("%s",&exitKey);
        if(exitKey = 'S'){
            exit(EXIT_SUCCESS);
        }
    }
}

int main(){
    int r;                                                                          // Se declaran variables.
    struct sockaddr_in server;                                  
    pthread_t ListenThread;
    struct HashTable table = CreateTable();

    printf("Bienvenido a la apliación cliente.\n\n");
    printf("Inicializando servidor...\n");

    len = sizeof(struct sockaddr);
    for(int i = 0; i<BACKLOG; i++){
        clientsConnected[i] = (struct Client*) malloc(sizeof(struct Client)); 
        bzero(clientsConnected[i], sizeof(struct Client));
    }

    serverfd = socket(AF_INET, SOCK_STREAM, 0);                                     // Se configura y se crea el socket.
    if(serverfd == -1){                                                             // Verificación de error.
        perror("El socket no pudo ser creado.\n");
        exit(EXIT_FAILURE);
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    bzero((server.sin_zero), 8); 

    if(setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0){ // Libera el puerto para que pueda ser utilizado.
        perror("Error liberando dirección Ip.");
        exit(EXIT_FAILURE);
    }

    r = bind(serverfd, (struct sockaddr*) &server, len);
    if(r == -1){                                                                    // Verificación de error.
        perror("La dirección IP no pudo ser asignada.\n");
        exit(EXIT_FAILURE);
    }
    r = listen(serverfd, BACKLOG);                                                  // Se pone el servidor a la escucha de solicitudes entrantes.
    if(r == -1){                                                                    // Verificación de error.
        perror("El servidor no puede escuchar conexiones.\n");
        exit(EXIT_FAILURE);
    }

    printf("El servidor fue inicializado correctamente, se están escuchando solicitudes de conexión entrantes.\n\n");    

    pthread_create(&ListenThread,NULL, ListenExit, NULL);  // Se crea un hilo que se encarga de esperar salida por parte del usuario.

    Table = CreateTable();                                  // Inicializa la tabla Hash.

    CurrentUsers = 0;
    while(true){
        int clientfd = accept(serverfd, (struct sockaddr*) &clientfd, &len);        // Se acepta una solicitud de conexión entrante.
        clientsConnected[CurrentUsers]->Ip.s_addr = clientfd;
        clientsConnected[CurrentUsers]->clientfd = clientfd;
        if(clientfd == -1){
            printf("La conexión entrante de la Ip %s no pudo ser aceptada.\n",inet_ntoa(clientsConnected[CurrentUsers]->Ip));
        }else{
            printf("El remoto %s se ha conectado correctamente.\n",inet_ntoa(clientsConnected[CurrentUsers]->Ip));      
            pthread_create(&clientsConnected[CurrentUsers]->idThread,NULL, ListenRequest, clientsConnected[CurrentUsers]);  // Se crea un hilo que se encarga de recibir datos entrantes.
            CurrentUsers++;
        }
    } 
    return 0;
}