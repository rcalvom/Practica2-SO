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
#include <semaphore.h>
#include <fcntl.h>
#include "Functions.h"
#include "Hash.h"
#include "ShippingData.h"
#include "Opciones.h"

#define PORT 1234
#define BACKLOG 32

// Variables globales.
int serverfd;
socklen_t len;
int CurrentUsers;
struct Client* clientsConnected[BACKLOG];
struct HashTable *Table;
sem_t * semaphore;

// Método para el hilo que se encarga de recibir datos de los clientes y ejecutar las solicitudes.
void* ListenRequest(void* args){
    struct Client *Client = (struct Client*) args;
    printf("Escuchando peticiones del cliente %s ...\n\n",inet_ntoa(Client->Ip));
    while(true){                                   
        int option;                                  
        recv(Client->clientfd, &option, sizeof(int), 0);                         // Recibe la opción del menú dada por el usuario.
        switch (option){
            case 1:{                        // Si la opción del cliente es Ingresar Registro.
                sem_wait(semaphore);
                struct dogType *new = malloc(sizeof(struct dogType));
                bzero(new,sizeof(struct dogType));
                recv(Client->clientfd,new,sizeof(struct dogType),0);            // Recibe la estructura del cliente.
                bool flag = IngresarRegistro(Table,new);                        // La ingresa al sistema (Archivo dataDogs.dat y historia.)
                send(Client->clientfd,&flag,sizeof(flag),0);                    // Envía confirmación al cliente si pudo ingresar el registro.
                if(flag){
                    WriteLog(1,inet_ntoa(Client->Ip),new->name);                // Si se pudo añadir la historia correctamente, Se muestra lo dicho en el Log.
                    printf("Inserción de %s por el cliente %s correctamente...\n",new->name,inet_ntoa(Client->Ip));
                }               
                free(new);
                sem_post(semaphore);
                break;
            }
            case 2:{                        // Si la opción del cliente es Ver Registro.
                sem_wait(semaphore);
                long idRegister;
                recv(Client->clientfd,&idRegister,sizeof(idRegister),0);        // Recibe el id del registro que va a buscar.
                bool existFile = ExisteRegistro(idRegister);                    // Analiza si exíste la historia clínica de dicha id.
                send(Client->clientfd,&existFile,sizeof(existFile),0);          // Envía al cliente si existe o no dicho archivo.
                if(existFile){                                                  // Si exíste....
                    bool answer;
                    recv(Client->clientfd,&answer,sizeof(bool),0);              // Recibe la respuesta de si el cliente quiere abrir el archivo.
                    if(answer){                                                 // Si la respuesta es afirmativa...
                        FILE *file;
                        char* data;
                        file = fopen(FilePath(idRegister),"r");                 // Abre el archivo.
                        fseek(file,0L,SEEK_END);
                        long size = ftell(file);
                        rewind(file);
                        send(Client->clientfd,&size,sizeof(size),0);            // Envía el tamaño del archivo a recibir.
                        data = malloc(size+1);
                        bzero(data,size);
                        fread(data,size,1,file);
                        send(Client->clientfd,data,size,0);                     // Envía la historia clinica.
                        fclose(file);
                        free(data); 
                        recv(Client->clientfd,&size,sizeof(size),0);            // Luego que el usuario edite la historia. Recibe el tamaño de la historia modificada.
                        data = malloc(size+1);
                        bzero(data,size);
                        recv(Client->clientfd,data,size,0);                     // Recibe la historia nueva.
                        file = fopen(FilePath(idRegister),"w+");        
                        fwrite(data,size,1,file);                               // La escribe en el archivo.
                        fclose(file);
                        free(data);
                        char* id = malloc(10);
                        sprintf(id,"%li",idRegister);
                        WriteLog(2,inet_ntoa(Client->Ip),id);                   // Registra la busqueda en los Logs.
                        printf("Edición de la historia de %s por el cliente %s correctamente...\n",id,inet_ntoa(Client->Ip));
                        free(id);
                    }
                }
                sem_post(semaphore);
                break;
            }
            case 3:{                                                                                // Si la opción del cliente es Borrar Registro.
                sem_wait(semaphore);
                long NumRegisters = 0/*ContarTabla()*/;
                long id;
                bool flag;
                send(Client->clientfd,&NumRegisters,sizeof(NumRegisters),0);                        // Envía al cliente la cantidad de registros en la tabla Hash.
                recv(Client->clientfd,&flag,sizeof(flag),0);                                        // Recibe la confirmacion del cliente si continuar con la operación.    
                if(flag){                                                                           // Si el usuario permite continuar con la operación.
                    long idTemp;
                    FILE *file, *temp;
                    struct dogType* registro = malloc(sizeof(struct dogType));
                    bzero(registro,sizeof(struct dogType));
                    recv(Client->clientfd,&id,sizeof(id),0);

                    long exist = borrar(Table,id);  
                    if(exist != -1){                                                                // Si el registro existe en la tabla hash y se puede borrar...
                        bool answer = true;
                        send(Client->clientfd,&answer,sizeof(answer),0);
                        file = fopen("dataDogs.dat","r");                                           // Se abre un archivo que contiene las estructuras..
                        temp = fopen("temp.dat","w+");                                              // Se crea un archivo temporal donde se guardaran las estructuras que nos serán eliminadas.
                        do{                                                                         // Mientras el archivo aún tenga estructuras....
                            if(fread(&idTemp,sizeof(long),1,file) == 0){
                                break;
                            }
                            fread(registro,sizeof(struct dogType),1,file);                          // ..... Lea los datos .....
                            if(idTemp == id){
                                continue;                                                           // ... y exceptuando el que se va a eliminnar...
                            }
                            fwrite(&idTemp,sizeof(long),1,temp);                                    // ... Escribirlos todos en el archivo temporal
                            fwrite(registro,sizeof(struct dogType),1,temp);                             
                        }while(feof(file) == 0);
                        fclose(file);
                        fclose(temp);
                        remove("dataDogs.dat");                                                     // Se elimina el archivo viejo y ....
                        rename("temp.dat","dataDogs.dat");
                        remove(FilePath(id));                                                       // Elimina la historia clínica de la mascota eliminada.
                        char* idChar = malloc(10);
                        sprintf(idChar,"%li",id);
                        WriteLog(3,inet_ntoa(Client->Ip),idChar);                                   // Escribe el registro de la acción.
                        printf("Eliminación de %s por el cliente %s correctamente...\n",idChar,inet_ntoa(Client->Ip));
                        free(registro);
                        free(idChar);
                    }else{
                        bool answer = false;
                        send(Client->clientfd,&answer,sizeof(answer),0);
                    }                                                                               
                }
                sem_post(semaphore);
                break;
            }
            case 4:{                        // Si la opción del cliente es Buscar Registro.
                sem_wait(semaphore);
                char* name = malloc(32);
                int size;
                recv(Client->clientfd,name,32,0);                                   // Recibe el nombre de la mascota a buscar.

                //size = tamaño de la cadena a enviar;
                //char* search = malloc(size);
                //send(Client->clientfd,search,size,0);

                WriteLog(4,inet_ntoa(Client->Ip),name);                             // Escribe la acción en el Log.
                sem_post(semaphore);
                break;
            }
        }
        option = 0;
    }
}

//Método para hilo que se encarga de cerrar el proceso si el usuario lo solicita.
void* ListenExit(void* client){
    char exitKey;
    while(true){
        scanf("%c",&exitKey);                                                       // Lee del teclado un caracter.
        if(exitKey == 'q' || exitKey == 'Q'){                                       // Si el caracter es 'Q' o 'q'.
            exit(EXIT_SUCCESS);                                                     // Sale del programa sin enviar error.
        }
    }
}

// Método principal. punto de partida inicial de la practica.
int main(){
    int r;                                                                          // Se declaran variables.
    struct sockaddr_in server;                                  
    pthread_t ListenThread;
    Table = CreateTable();                                                          // Se crea la tabla hush.

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
    if(r == -1){
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

    semaphore = sem_open("Semaphore",O_CREAT,0700,1);      // Inicializa el semáforo que ayudará a evitar conciciones de carrera en los archivos de datos.

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