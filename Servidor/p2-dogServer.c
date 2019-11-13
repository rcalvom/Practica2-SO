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

#define PORT 1234
#define BACKLOG 32

int serverfd;
socklen_t len;
int CurrentUsers;

struct Client{
    int clientfd;
    int idSolicitud;
};

void* ListenConections(void* client){
    CurrentUsers = 0;
    while(true){
        int clientfd = accept(serverfd, (struct sockaddr*) &clientfd, &len); // Se acepta una solicitud de conexión entrante.
        if(clientfd == -1){
            printf("La conexión no pudo ser aceptada.\n");
        }else{
            printf("Se ha conectado un nuevo usuario correctamente.\n");
            CurrentUsers++;
        }
    } 
}


int main(){
    printf("Bienvenido a la apliación cliente.\n\n");
    printf("Inicializando servidor...\n");
    int r;                                                  // Se declaran variables.
    struct sockaddr_in server;                              // Se configura y se crea el socket.
    pthread_t ListenThread;
    len = sizeof(struct sockaddr);
    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if(serverfd == -1){
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
    r = listen(serverfd, BACKLOG);                                  // Se pone el servidor a la escucha de solicitudes entrantes.
    if(r == -1){
        perror("El servidor no puede escuchar conexiones.\n");
        exit(EXIT_FAILURE);
    }

    printf("El servidor fue inicializado correctamente, se están escuchando solicitudes de conexión entrantes.\n\n");

    pthread_create(&ListenThread,NULL,ListenConections,NULL);       // Se crea un hilo que se encarga de escuchar conexiónes entrantes.

    while(true){continue;}
    DisposeConsole();

    return 0;
}