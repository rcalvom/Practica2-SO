#include <ncurses.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <strings.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "Functions.h"
#include "Hash.h"
#include "ShippingData.h"
#include "Opciones.h"

#define PORT 1234
#define BACKLOG 32

// Estructura que almacena los datos del cliente.
struct Client{
    int clientfd;
    int idSolicitud;
    pthread_t idThread;
    struct in_addr Ip;
};

// Variables globales.
int serverfd;
socklen_t len;
int CurrentUsers;
struct Client* clientsConnected[BACKLOG];

// Método para el hilo que se encarga de recibir datos de los clientes.
void* ListenRequest(void* client){
    struct Client *toListen = (struct Client*)client;
    printf("Escuchando peticiones del cliente %s ...\n\n",inet_ntoa(toListen->Ip));
    while(true){                                                                     
        while(toListen->idSolicitud != 0){                                              
            sleep(1);
        }
        int rec = recv(toListen->clientfd, &toListen->idSolicitud, sizeof(int), 0);     
    }
}

// Método para hilo que se encarga de estar a la escucha de nuevas solicitudes entrantes.
void* ListenConections(void* clients){
    CurrentUsers = 0;
    while(true){
        int clientfd = accept(serverfd, (struct sockaddr*) &clientfd, &len);        // Se acepta una solicitud de conexión entrante.
        clientsConnected[CurrentUsers]->Ip.s_addr = clientfd;
        if(clientfd == -1){
            printf("La conexión entrante de la Ip %s no pudo ser aceptada.\n",inet_ntoa(clientsConnected[CurrentUsers]->Ip));
        }else{
            printf("El remoto %s se ha conectado correctamente.\n",inet_ntoa(clientsConnected[CurrentUsers]->Ip));      
            clientsConnected[CurrentUsers]->clientfd = clientfd;
            clientsConnected[CurrentUsers]->idSolicitud = 0;
            pthread_create(&clientsConnected[CurrentUsers]->idThread,NULL,ListenRequest, clientsConnected[CurrentUsers]);  // Se crea un hilo que se encarga de recibir datos entrantes.
            CurrentUsers++;
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

    pthread_create(&ListenThread,NULL,ListenConections, clientsConnected);          // Se crea un hilo que se encarga de escuchar conexiónes entrantes.

    int i = 0;
    while(true){
        int l = 0;
        int solicitud = clientsConnected[i]->idSolicitud;
        switch(solicitud){
            case 1: {
                size_t dogSize = sizeof(struct dogType);
                struct dogType *new = (struct dogType*) malloc(dogSize);
                bzero(new, dogSize);
                l = recv((clientsConnected[i])->clientfd, new, dogSize, 0);
                if(l == -1){
                    perror("No se ha recibido el dato correctamente/n");
                    free(new);
                    clientsConnected[i]->idSolicitud = 0;
                    break;
                }
                IngresarRegistro(&table, new);
                free(new);
                clientsConnected[i]->idSolicitud = 0;
                break;
            }
            case 2: {
                long id;
                l = recv(clientsConnected[i]->clientfd, &id, sizeof(id), 0);
                if(l == -1){
                    perror("No se ha recibido el dato correctamente./n");
                    clientsConnected[i]->idSolicitud = 0;
                    break;
                }
                VerRegistro(id);
                clientsConnected[i]->idSolicitud = 0;
                break;
            }
            case 3: {
                long id;
                l = recv(clientsConnected[i]->clientfd, &id, sizeof(id), 0);
                if(l == -1){
                    perror("No se ha recibido el dato correctamente./n");
                    clientsConnected[i]->idSolicitud = 0;
                    break;
                }
                BorrarRegistro(&table, id);
                clientsConnected[i]->idSolicitud = 0;
                break;
            }
            case 4: {
                char *nombre = (char*)malloc(32);
                bzero(nombre, 32);
                l = recv(clientsConnected[i]->clientfd, nombre, 32, 0);
                if(l == -1){
                    perror("No se ha recibido el dato correctamente");
                    free(nombre);
                    break;
                }
                BuscarRegistro(&table, nombre);
                free(nombre);
                clientsConnected[i]->idSolicitud = 0;
                break;
            }
            case 5: {
                //TODO: Semaforo
                for(int j = i; i < CurrentUsers-1; j++){
                    clientsConnected[j] = clientsConnected[j+1];
                }
                CurrentUsers--;
                clientsConnected[i]->idSolicitud = 0;
                break;
            }
        }
        (clientsConnected[i])->idSolicitud = 0;
        if(CurrentUsers != 0){
            i = (i+1)%CurrentUsers;
        }
    }
    
    return 0;
}