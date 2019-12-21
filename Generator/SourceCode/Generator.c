#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Hash.h"
#include "Functions.h"
#include "ShippingData.h"

int main(int argc, char* argv[]){
    int NUMESTRUCTURAS = atoi(argv[1]);
    long id;
    FILE *file, *names, *historia;
    char* AllNames = malloc(32*1700);
    char* mensajeHistoria = malloc(200);
    char* path = malloc(46);
    char* idchar = malloc(16);
    char* values = malloc(10);
    if(AllNames == NULL){
        printf("El espacio no pudo ser reservado.\n");
        return -1;
    }

    srand(time(NULL));                                                              // Se elige una semilla aleatoria.
    struct HashTable *table = CreateTable();                                        // Se Inicializa la tabla hash.
    char* Breeds[] = {"Labrador", "Pug", "Bulldog", "Chihuahua", "Golden",          
                      "Dalmata", "Pastor", "Labrador", "San Bernardo"};             // Se crea arreglo con razas.
    char Genders[] = {'M','H'};                                                     // Se crea arreglo con géneros.
    file = fopen("dataDogs.dat","w+");                                              // Se abre el archivo que contendrá las estructuras.
    names = fopen("nombresMascotas.txt","r");                                       // Se abre el archivo que tiene los nombres.
    if(names == NULL){
        perror("Error: ");
        exit(-1);
    }
    leer(AllNames, names);
    fclose(names);
    struct dogType newRegister;
    
    printf("Generador de estructuras:\n\n");
    printf("Se van a crear %i estructuras...\n", NUMESTRUCTURAS);

    for(int i = 0; i<NUMESTRUCTURAS; i++){
        bzero(&newRegister, sizeof(struct dogType));                                // Crea una estructura con 0 en todos sus bits.

        int rnames = rand() % 1700;                                                 // Elije un número aleatorio entre 1 y 1000.
        char *name = AllNames + (rnames * 32);

        strcpy(newRegister.name, name);

        strcpy(newRegister.type,"Perro");
        newRegister.age = rand() % 14 + 1;                                          // Selecciona un número aleatorio entre 1 y 14.

        strcpy(newRegister.breed,Breeds[rand() % 8]);                               // Selecciona una raza aleatoria del arreglo Breeds.

        newRegister.height = rand() % 14 + 1;                                       // Elije una estatura aleatoria entre 1 y 14.

        newRegister.weight = rand() % 14 + 1;                                       // Elije un peso aleatorio entre 1 y 15.

        newRegister.gender = Genders[rand() % 2];                                   // Elije un género aleatorio entre 'M' y 'H'.
        id = insertElement(table, &newRegister.name[0]);                            // Se agrega el elemento a la tabla hash.

        fwrite(&id, sizeof(long), 1, file);                                         // Se escribe la id del elemento en la tabla hash.
        fwrite(&newRegister, sizeof(struct dogType), 1, file);                      // Se escriben los datos anteriormente solicitados en el archivo.

        bzero(mensajeHistoria,200);
        bzero(path,46);
        bzero(idchar,16);
        bzero(values,10);

        sprintf(idchar,"%li",id);
        strcat(path,"historias/");
        strcat(path,idchar);
        strcat(path,".dat");

        strcat(mensajeHistoria,"Historia clinica.\n\nNombre: ");               // Escribe en el archivo de historia clinica toda la información de la mascota
        strcat(mensajeHistoria,newRegister.name);
        strcat(mensajeHistoria,"\nTipo: ");
        strcat(mensajeHistoria,newRegister.type);
        strcat(mensajeHistoria,"\nEdad: ");
        sprintf(values,"%i",newRegister.age);
        strcat(mensajeHistoria,values);
        strcat(mensajeHistoria,"\nRaza: ");
        strcat(mensajeHistoria,&newRegister.breed[0]);
        strcat(mensajeHistoria,"\nEstatura: ");
        sprintf(values,"%i",newRegister.height);
        strcat(mensajeHistoria,values);
        strcat(mensajeHistoria,"\nPeso: ");
        sprintf(values,"%f",newRegister.weight);    
        strcat(mensajeHistoria,values);
        strcat(mensajeHistoria,"\nGénero: ");
        strcat(mensajeHistoria,&newRegister.gender);

        historia = fopen(path,"w+");

        fwrite(mensajeHistoria,strlen(mensajeHistoria),1,historia);
        fclose(historia);


    }
    printf("Registros creados correctamente.\n\n");
    fclose(file);
    printf("%i registros en la tabla\n", table->Elements);
    printf("Guardando tabla... ");
    SaveTable(table);
    printf("Tabla guardada.\n");
    free(path);
    free(mensajeHistoria);
    free(idchar);
    free(values);
    free(AllNames);
    return 0;
}