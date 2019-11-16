#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ncurses.h> 
#include "Functions.h"
#include "Hash.h"
#include "ShippingData.h"

//Opción del menú Ingresar Registro.
int IngresarRegistro(struct HashTable* tabla, struct dogType *new){
    int r = 0;
    long id;
    FILE *dataDogs, *historia;
    char *values, *mensajeHistoria, *filepath, *filename;

    dataDogs = fopen("dataDogs.dat","a");
    filepath = (char*) malloc(46);
    filename = (char*) malloc(16);
    mensajeHistoria = (char*)malloc(1);
    values = (char*)malloc(10);

    if(dataDogs == NULL || filepath == NULL || filename == NULL || mensajeHistoria == NULL || values == NULL){
        asm("DALL:");
        fclose(dataDogs);
        free(filepath);
        free(filename);
        free(mensajeHistoria);
        free(values);
        return -1;
    }
    id = insertElement(tabla, new->name, 0);
    bzero(filename, 16);
    bzero(filepath, 46);
    *mensajeHistoria = '\0';
    bzero(values, 10);

    sprintf(filename,"%li", id);
    strcat(filepath,"historias/");
    strcat(filepath,filename);
    strcat(filepath,".dat");
    toLowerCase(filepath);
    historia = fopen(filepath,"w+");    

    if(historia == NULL){
        fclose(historia);
        borrar(tabla, id);
        asm("jmp DALL");
    }

    do{
        r += fwrite(&id, sizeof(long), 1, dataDogs);                            // Se escribe en el archivo la Id correspondiente.
    } while(r<1);
    do{
        r += fwrite(new, sizeof(struct dogType), 1, dataDogs);                  // Se escriben los datos de la mascota anteriormente solicitados en el archivo.
    } while (r < 2);

    strcat(mensajeHistoria,"Historia clinica.\n\nNombre: ");               //Escribe en el archivo de historia clinica toda la información de la mascota
    strcat(mensajeHistoria,new->name);
    strcat(mensajeHistoria,"\nTipo: ");
    strcat(mensajeHistoria,&new->type[0]);
    strcat(mensajeHistoria,"\nEdad: ");
    sprintf(values,"%i",new->age);
    strcat(mensajeHistoria,values);
    strcat(mensajeHistoria,"\nRaza: ");
    strcat(mensajeHistoria,&new->breed[0]);
    strcat(mensajeHistoria,"\nEstatura: ");
    sprintf(values,"%i",new->height);
    strcat(mensajeHistoria,values);
    strcat(mensajeHistoria,"\nPeso: ");
    sprintf(values,"%f",new->weight);
    strcat(mensajeHistoria,values);
    strcat(mensajeHistoria,"\nGénero: ");
    strcat(mensajeHistoria,&new->gender);
    r = 0;
    do{
        r += fwrite(mensajeHistoria,strlen(mensajeHistoria),1,historia);
    } while(r < 1);

    fclose(historia);
    free(filename);
    free(filepath);
    free(dataDogs);
    free(mensajeHistoria);
    free(values);
    return 0;
}

//Opción del menú Ver Registro.
int VerRegistro(long id){
    return 0;
}

//Opción del menú Borrar Registro.
int BorrarRegistro(struct HashTable* tabla, long id){
    return 0;
}

//Opción del menú Buscar Registro.
int BuscarRegistro(struct HashTable* tabla, char *nombre){
    return 0;
}

int Generado(struct HashTable* tabla){
    struct dogType *newRegister;                                            //Se crea el struct
    int r, attempts;
    char *values, *mensajeHistoria, *filename, *filepath;
    long idd;
    FILE *file, *historia;                                                // Cierra el archivo dataDogs.dat.
    file = fopen("dataDogs.dat","r");
    do{
        attempts = 0;
        newRegister = (struct dogType *) malloc(sizeof(struct dogType));
        r = 0;
        do{
            r += fread(&idd, sizeof(long), 1, file);
            if(r == 0) attempts++;
        } while(r == 0 && attempts < 5);
        do{
            r += fread(newRegister, sizeof(struct dogType), 1, file);
            if(r == 1) attempts++;
        } while(r == 1 && attempts < 5);
        if(attempts <5){
            filepath = (char*) malloc(46);                                         // Se libera espacio para la ruta de la historia clinica.
            filename = (char*) malloc(16);
            bzero(filepath, 46);
            bzero(filename, 16);
            sprintf(filename,"%li",idd);
            strcat(filepath,"historias/");                                          // Se concatenan la extensión y la ubicación.
            strcat(filepath,filename);
            strcat(filepath,".dat");
            toLowerCase(filepath);
            historia = fopen(filepath,"w+");                                        // Se crea el archivo de historia clinica.
            if(historia == NULL){
                printw("No se ha podido crear la historia\n");
                return -1;
            }
            mensajeHistoria = (char*) malloc(175);
            values = (char*) malloc(10);
            bzero(mensajeHistoria, 175);
            bzero(values, 10);
            strcat(mensajeHistoria,"Historia clinica.\n\nNombre: ");               //Escribe en el archivo de historia clinica toda la información de la mascota
            strcat(mensajeHistoria,newRegister->name);
            strcat(mensajeHistoria,"\nTipo: ");
            strcat(mensajeHistoria,&newRegister->type[0]);
            strcat(mensajeHistoria,"\nEdad: ");
            sprintf(values,"%i",newRegister->age);
            strcat(mensajeHistoria,values);
            strcat(mensajeHistoria,"\nRaza: ");
            strcat(mensajeHistoria,&newRegister->breed[0]);
            strcat(mensajeHistoria,"\nEstatura: ");
            sprintf(values,"%i",newRegister->height);
            strcat(mensajeHistoria,values);
            strcat(mensajeHistoria,"\nPeso: ");
            sprintf(values,"%f",newRegister->weight);
            strcat(mensajeHistoria,values);
            strcat(mensajeHistoria,"\nGénero: ");
            strcat(mensajeHistoria,&newRegister->gender);
            r = 0;
            r += fwrite(mensajeHistoria,strlen(mensajeHistoria),1,historia);
            if(r == 0){
                printw("El archivo de historia clinica no ha podido ser escrito.");
                return -1;
            }
            fclose(historia);
            free(filename);
            free(filepath);
            free(values);
            free(mensajeHistoria);
        }
        free(newRegister);
    } while(attempts < 5);
    fclose(file);
    return 0;
}