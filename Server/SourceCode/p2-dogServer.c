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
struct HashTable *Table;
sem_t *semaphore;

// Método para el hilo que se encarga de recibir datos de los clientes y ejecutar las solicitudes.
void* ListenRequest(void* args){
    struct Client *Client = (struct Client*) args;

    while(true){    
        int option;                                  
        recv(Client->clientfd, &option, sizeof(option), 0);                     // Recibe la opción del menú dada por el usuario.

        switch (option){
            case 1: {                                                           // Si la opción del cliente es Ingresar Registro.
                sem_wait(semaphore);

                struct dogType *new = malloc(sizeof(struct dogType));
                bzero(new, sizeof(struct dogType));
                recv(Client->clientfd, new, sizeof(struct dogType), 0);         // Recibe la estructura del cliente.
                bool flag = IngresarRegistro(Table, new);                       // La ingresa al sistema (Archivo dataDogs.dat y historia.)
                send(Client->clientfd, &flag, sizeof(flag), 0);                 // Envía confirmación al cliente si pudo ingresar el registro.
                if(flag){
                    WriteLog(1, inet_ntoa(Client->Ip.sin_addr), new->name);     // Si se pudo añadir la historia correctamente, Se muestra lo dicho en el Log.
                }               
                free(new);

                sem_post(semaphore);
                break;
            }

            case 2: {                                                           // Si la opción del cliente es Ver Registro.
                sem_wait(semaphore);

                long idRegister;
                recv(Client->clientfd, &idRegister, sizeof(idRegister), 0);     // Recibe el id del registro que va a buscar.
                bool existFile = ExisteElElemento(idRegister);                  // Analiza si esa id existe en la tabla hash.
                send(Client->clientfd, &existFile, sizeof(existFile), 0);       // Envía al cliente si existe o no dicha id en la tabla hash.
                if(existFile){                                                  // Si exíste....
                    bool answer;
                    recv(Client->clientfd, &answer, sizeof(answer), 0);         // Recibe la respuesta de si el cliente quiere abrir el archivo.
                    if(answer){                                                 // Si la respuesta es afirmativa...
                        FILE *file;
                        char* data, *id;
                        file = fopen(FilePath(idRegister), "r");
                        if(file == NULL){                                       // Si la historia clínica no exite...
                            struct dogType* pet = FindPetById(idRegister);
                            CreateClinicHistory(idRegister, pet);               // Y cree el archivo de historia clínica.
                            free(pet);
                            file = fopen(FilePath(idRegister), "r");
                        }
                        
                        fseek(file, 0L, SEEK_END);
                        long size = ftell(file);
                        rewind(file);
                        send(Client->clientfd, &size, sizeof(size), 0);         // Envía el tamaño del archivo a recibir.
                        data = malloc(size + 1);
                        bzero(data, size);
                        fread(data, size, 1, file);
                        send(Client->clientfd, data, size, 0);                  // Envía la historia clinica.
                        fclose(file);
                        free(data); 
                        recv(Client->clientfd, &size, sizeof(size), 0);         // Luego que el usuario edite la historia. Recibe el tamaño de la historia modificada.
                        data = malloc(size + 1);
                        bzero(data, size);
                        recv(Client->clientfd, data, size, 0);                  // Recibe la historia nueva.
                        file = fopen(FilePath(idRegister), "w+");        
                        fwrite(data, size, 1, file);                            // La escribe en el archivo.
                        fclose(file);
                        
                        id = malloc(10);
                        bzero(id, 10);
                        sprintf(id, "%li", idRegister);
                        WriteLog(2, inet_ntoa(Client->Ip.sin_addr), id);        // Registra la busqueda en los Logs.                        
                        free(id);
                        free(data);
                    }
                }

                sem_post(semaphore);
                break;
            }
            case 3: {                                                                               // Si la opción del cliente es Borrar Registro.
                sem_wait(semaphore);

                long NumRegisters;
                bool flag;

                NumRegisters = Table->Elements;
                send(Client->clientfd, &NumRegisters, sizeof(NumRegisters), 0);                     // Envía al cliente la cantidad de registros en la tabla hash.
                recv(Client->clientfd, &flag, sizeof(flag), 0);                                     // Recibe la confirmación del cliente si continuar con la operación.    
                if(flag){                                                                           // Si el usuario permite continuar con la operación.
                    long id, exist;
                    bool answer;

                    recv(Client->clientfd, &id, sizeof(id), 0);                                     // Recibe la id del registro a eliminar.
                    exist = borrar(Table, id);                                                      // Intenta borrar de la tabla hash.

                    if(exist != -1){                                                                // Si el registro fue borrado de la tabla hash ...                     
                        char* idChar;
                        answer = true;

                        BorrarRegistro(id);                                                         // Borra el registro del archivo de estructuras.
                        send(Client->clientfd, &answer, sizeof(answer), 0);                        
                                                                               
                        idChar = malloc(10);
                        sprintf(idChar,"%li",id);
                        WriteLog(3, inet_ntoa(Client->Ip.sin_addr), idChar);                        // Escribe el registro de la acción.
                        free(idChar);
                    }else{
                        answer = false;
                        send(Client->clientfd, &answer, sizeof(answer), 0);
                    }                                                                               
                }

                sem_post(semaphore);
                break;
            }
            case 4: {                                                               // Si la opción del cliente es Buscar Registro.
                sem_wait(semaphore);

                char* name = malloc(32);
                bzero(name, 32);
                recv(Client->clientfd, name, 32, 0);                                // Recibe el nombre de la mascota a buscar.
                struct String* search = buscarId(Table, name);
                long size = search->length;
                send(Client->clientfd, &size, sizeof(size), 0);
                send(Client->clientfd, search->string, size, 0);
                WriteLog(4, inet_ntoa(Client->Ip.sin_addr), name);                  // Escribe la acción en el Log.
                free(search);
                free(name);

                sem_post(semaphore);
                break;
            }
        }
        option = 0;                                                                 // Reinicia la opción recibida.
    }
}

//Método para hilo que se encarga de cerrar el proceso si el usuario lo solicita.
void *ListenExit(void *client){
    char *exitKey = malloc(5);
    bzero(exitKey, 5);
    while(true){
        scanf("%s", exitKey);                                                       // Lee del teclado una cadena.
        toUpperCase(exitKey);
        if(equals(exitKey, "EXIT")){                                                // Si la cadena es "EXIT".
            sem_close(semaphore);
            sem_unlink("S");
            exit(EXIT_SUCCESS);                                                     // Sale del programa sin enviar error.
        }
    }
}

// Método principal. Punto de partida inicial de la practica.
int main(){

    int r, serverfd, CurrentUsers;
    struct sockaddr_in server; 
    socklen_t len;                                 
    pthread_t ListenThread;
    struct Client *clientsConnected[BACKLOG];

    sem_close(semaphore);                                                               // Se eliminan posibles instancias del semáforo.
    sem_unlink("S");
    Table = CreateTable();                                                              // Se crea o carga la tabla hash.
    len = sizeof(struct sockaddr_in);

    for(int i = 0; i < BACKLOG; i++){
        clientsConnected[i] = malloc(sizeof(struct Client)); 
        bzero(clientsConnected[i], sizeof(struct Client));
    }

    serverfd = socket(AF_INET, SOCK_STREAM, 0);                                         // Se configura y se crea el socket.
    if(serverfd == -1){                                                                 // Verificación de error.
        perror("El socket no pudo ser creado.\n");
        exit(EXIT_FAILURE);
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    bzero(server.sin_zero, 8); 

    if(setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0){   // Libera el puerto para que pueda ser utilizado.
        perror("Error liberando dirección Ip.");
        exit(EXIT_FAILURE);
    }

    r = bind(serverfd, (struct sockaddr*) &server, len);
    if(r == -1){
        perror("La dirección IP no pudo ser asignada.\n");
        exit(EXIT_FAILURE);
    }

    r = listen(serverfd, BACKLOG);                                                      // Se pone el servidor a la escucha de solicitudes entrantes.
    if(r == -1){
        perror("El servidor no puede escuchar conexiones.\n");
        exit(EXIT_FAILURE);
    }

    pthread_create(&ListenThread, NULL, ListenExit, NULL);                              // Se crea un hilo que se encarga de esperar salida por parte del usuario.
    semaphore = sem_open("S", O_CREAT, 0700, 1);                                        // Inicializa el semáforo que ayudará a evitar conciciones de carrera en los archivos de datos.

    CurrentUsers = 0;

    while(true){
        int clientfd = accept(serverfd, (struct sockaddr*) &clientsConnected[CurrentUsers]->Ip, &len);        // Se acepta una solicitud de conexión entrante.
        clientsConnected[CurrentUsers]->clientfd = clientfd;
        if(clientfd != -1){
            pthread_create(&clientsConnected[CurrentUsers]->idThread, NULL, ListenRequest, clientsConnected[CurrentUsers]);  // Se crea un hilo que se encarga de recibir datos entrantes.
            CurrentUsers++;
        }
    } 

    return 0;
}
