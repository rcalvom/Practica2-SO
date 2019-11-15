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
#include "Opciones.h"
#include "ShippingData.h"

#define PORT 1234
#define BACKLOG 32

int serverfd;
socklen_t len;
int CurrentUsers;

struct Client{
    int clientfd;
    int idSolicitud;
};

void* ListenRequest(void* client){
    printf("Escuchando peticiones...\n");
    struct Client *toListen = (struct Client*)client;
    while(true){                                                                        //que despues hay que cambiar por alga más elegante mirando si se desconecta el cliente
        while(toListen->idSolicitud != 0){                                              //Y te preguntarás joven richie por qué este while, y yo te diré, porque no queremos que lea más solicitudes hasta que no ejecute la que le llegó 
            sleep(1);
        }
        int rec = recv(toListen->clientfd, &toListen->idSolicitud, sizeof(int), 0);     //Se guarda la id de solicitud
    }
}

void* ListenConections(void* clients){
    CurrentUsers = 0;
    while(true){
        struct Client *toAccept = (struct Client*)(clients+CurrentUsers);
        int clientfd = accept(serverfd, (struct sockaddr*) &clientfd, &len);        // Se acepta una solicitud de conexión entrante.
        if(clientfd == -1){
            printf("La conexión no pudo ser aceptada.\n");
        }else{
            printf("Se ha conectado un nuevo usuario correctamente.\n");
            toAccept->clientfd = clientfd;
            toAccept->idSolicitud = 0;
            CurrentUsers++;
        }
    } 
}


int main(){
    int r;                                                                          // Se declaran variables.
    struct sockaddr_in server;                                  
    pthread_t ListenThread;
    struct Client *clientsConnected;
    struct HashTable table = CreateTable();

    printf("Bienvenido a la apliación cliente.\n\n");
    printf("Inicializando servidor...\n");
    len = sizeof(struct sockaddr);
    clientsConnected = (struct Client*)malloc(sizeof(struct Client)*BACKLOG);              // Se solicita memoria para almacenar los clientes
    bzero(clientsConnected, sizeof(struct Client)*BACKLOG);
    serverfd = socket(AF_INET, SOCK_STREAM, 0);                                     // Se configura y se crea el socket.
    if(serverfd == -1){                                                             // Verificación de error
        perror("El socket no pudo ser creado");
        exit(EXIT_FAILURE);
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    bzero((server.sin_zero), 8); 
    r = bind(serverfd, (struct sockaddr*)&server, len);
    if(r == -1){
        perror("La dirección IP no pudo ser asignada.\n");
        exit(EXIT_FAILURE);
    }
    r = listen(serverfd, BACKLOG);                                                  // Se pone el servidor a la escucha de solicitudes entrantes.
    if(r == -1){
        perror("El servidor no puede escuchar conexiones.\n");
        exit(EXIT_FAILURE);
    }

    printf("El servidor fue inicializado correctamente, se están escuchando solicitudes de conexión entrantes.\n\n");

    pthread_create(&ListenThread,NULL,ListenConections, clientsConnected);          // Se crea un hilo que se encarga de escuchar conexiónes entrantes.

    int i = 0;
    while(true){
        if(i<CurrentUsers){
            int l = 0;
            int solicitud = (clientsConnected+i)->idSolicitud;
            if(solicitud == 1){
                size_t dogSize = sizeof(struct dogType);
                struct dogType *new = (struct dogType*)malloc(dogSize);
                bzero(new, dogSize);
                l = recv((clientsConnected+i)->clientfd, new, dogSize, 0);
                IngresarRegistro(&table, new);
                free(new);
            } else if (solicitud == 2){
                long id;
                l = recv((clientsConnected+i)->clientfd, &id, sizeof(id), 0);
                VerRegistro(id);
            } else if (solicitud == 3){
                long id;
                l = recv((clientsConnected+i)->clientfd, &id, sizeof(id), 0);
                BorrarRegistro(&table, id);
            } else if (solicitud == 4){
                char *nombre = (char *)malloc(32);
                bzero(nombre, 32);
                l = recv((clientsConnected+i)->clientfd, nombre, 32, 0);
                BuscarRegistro(&table, nombre);
                free(nombre);
            } else if (solicitud == 5){
                //Semaforo
                for(int j = i; i < CurrentUsers-1; j++){
                    *(clientsConnected+j) = *(clientsConnected+j+1);
                }
                CurrentUsers--;
            }
            (clientsConnected+i)->idSolicitud = 0;
            i++;
        } else {
            i = 0;
        }
    }
    DisposeConsole();

    return 0;
}