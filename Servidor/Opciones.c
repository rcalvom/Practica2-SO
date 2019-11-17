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
    int r = 0; long id;
    FILE *dataDogs, *historia;
    char *values, *mensajeHistoria, *filepath, *filename;

    dataDogs = fopen("dataDogs.dat","a");
    filepath = (char*) malloc(46);
    filename = (char*) malloc(16);
    mensajeHistoria = (char*) malloc(200);
    values = (char*) malloc(10);

    if(dataDogs == NULL || filepath == NULL || filename == NULL || mensajeHistoria == NULL || values == NULL){
        asm("DALL:");
        fclose(dataDogs);
        free(filepath);
        free(filename);
        free(mensajeHistoria);
        free(values);
        return -1;
    }
    id = /*insertElement(tabla, new->name, 1)*/1;
    bzero(filename, 16);
    bzero(filepath, 46);
    bzero(mensajeHistoria,200);
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
    } while(r < 1);

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
        r += fwrite(mensajeHistoria, strlen(mensajeHistoria), 1, historia);
    } while(r < 1);

    fclose(dataDogs);
    fclose(historia);
    free(mensajeHistoria);
    free(filename);
    free(filepath);
    free(values);
    return 0;
}

// Verifica si existe la historia con la id indicada.
_Bool ExisteRegistro(long id){
    char* filepath = FilePath(id);
    printf("Ruta: %s",filepath);
    FILE* file = fopen(filepath,"r");                                       // Se comprueba si el archivo existe.
    if(file == NULL){
        free(filepath);
        if(file != NULL){
            fclose(file);
        }
        return 0;
    }else{
        free(filepath);
        if(file != NULL){
            fclose(file);
        }
        return 1;
    }
}

int VerRegistro2(long id){
    char* idchar = (char*) malloc(5);
    char* filepath = (char*) malloc(46);
    char* batch = (char*) malloc(46);
    bzero(idchar, 5);
    bzero(filepath, 46);
    bzero(batch, 46);
    if(filepath == NULL || batch == NULL){
        return -1;
    }
    strcat(filepath,"historias/");
    sprintf(idchar,"%li",id);
    strcat(filepath,idchar);
    strcat(filepath,".dat");
    strcat(batch,"nano ");
    strcat(batch,filepath);
    FILE* file = fopen(filepath,"r");                                       // Se comprueba si el archivo existe.
    if(file == NULL){
        return -1;
    }else{
        return 0;
        
    }
    free(idchar);                                                           // librera memoria.
    free(filepath);
    free(batch); 
    if(file != NULL){
        fclose(file);
    }
    return 0;
}

//Opción del menú Borrar Registro.
int BorrarRegistro(struct HashTable* tabla, long id){
    struct dogType *registro = (struct dogType*) malloc(sizeof(struct dogType));     // Declara variables que se van a utilizar.
    long idTemp;
    int r, registros;
    FILE *file, *temp;

    bzero(registro, sizeof(struct dogType));

    /*do{
        registros = contarRegistros(tabla);

        Enviar la cantidad de registros.

    }while(registros == -1);*/

    /*if(id == -1){
        asm("jmp End");
    }*/

    file = fopen("dataDogs.dat","r");                                               // Se abre un archivo que contiene las estructuras..
    temp = fopen("temp.dat","w+");                                                  // Se crea un archivo temporal donde se guardaran las estructuras que nos serán eliminadas.
    if(file == NULL || temp == NULL){
        return -1;
    }
    do{                                                                             // Mientras el archivo aún tenga estructuras....
        r = fread(&idTemp,sizeof(long),1,file);
        if(r == 0){
            break;
        }
        r = fread(registro,sizeof(struct dogType),1,file);                          // ..... Lea los datos .....
        if(r == 0){
            return -1;
        }else if(idTemp == id){
            continue;                                                               // ... y exceptuando el que se va a eliminnar...
        }
        r = fwrite(&idTemp,sizeof(long),1,temp);                                    // ... Escribirlos todos en el archivo temporal
        if(r == 0){
            return -1;
        }
        r = fwrite(registro,sizeof(struct dogType),1,temp);
        if(r == 0){
            return -1;
        }
    }while(feof(file) == 0);

    fclose(file);
    fclose(temp);
    remove("dataDogs.dat");                                                         // Se elimina el archivo viejo y ....
    rename("temp.dat","dataDogs.dat");                                              // ... Se renombra el archivo temporal
    char* his = (char*) malloc(46);
    char* his1 = (char*) malloc(5);
    bzero(his,46);
    bzero(his1,5);
    strcat(his,"rm historias/");                                                    // Se elimina el archivo de historia clinica usando un bach del sistema.
    sprintf(his1,"%li",id);
    strcat(his,his1);
    strcat(his,".dat");
    system(his);
    free(his);
    free(his1);
    borrar(tabla, id);
    asm("End:");
    free(registro); 
    return 0;
}

//Opción del menú Buscar Registro.
int BuscarRegistro(struct HashTable* tabla, char *nombre){
    char *nameToSearch = (char *) malloc(32);
    if(nameToSearch == NULL){
        return -1;
    }
    bzero(nameToSearch, 32);

    //buscarId(tabla, nameToSearch);                                          // Se pide a la tabla que imprima los elementos que contengan el nombre dado.

    free(nameToSearch);
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
        if(attempts < 5){
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