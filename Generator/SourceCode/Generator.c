#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Hash.h"
#include "Functions.h"
#include "ShippingData.h"

#define NUMESTRUCTURAS 1E7

int main(){

    long id;
    FILE *file, *names;
    char* AllNames = (char *)malloc(32*1700);
    if(AllNames == NULL){
        printf("C mamo\n");
        return -1;
    }

    srand(time(NULL));                                                              // Se elige una semilla aleatoria.
    struct HashTable table = CreateTable();                                         // Se Inicializa la tabla hash.
    char* Breeds[] = {"Labrador", "Pug", "Bulldog", "Chihuahua", "Golden",          
                      "Dalmata", "Pastor", "Labrador", "San Bernardo"};             // Se crea arreglo con razas.
    char Genders[] = {'M','H'};                                                     // Se crea arreglo con géneros.
    file = fopen("dataDogs.dat","w");                                               // Se abre el archivo que contendrá las estructuras.
    names = fopen("nombresMascotas.txt","r");                                       // Se abre el archivo que tiene los nombres.
    leer(AllNames, names);
    fclose(names);
    struct dogType newRegister;
    
    for(int i = 0; i<NUMESTRUCTURAS; i++){
        if(i%100000 == 0)
            printf("Registros creados: %i\n", i);
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
        id = insertElement(&table, &newRegister.name[0], 1);                        // Se agrega el elemento a la tabla hash.

        fwrite(&id, sizeof(long), 1, file);                                         // Se escribe la id del elemento en la tabla hash.
        fwrite(&newRegister, sizeof(struct dogType), 1, file);                      // Se escriben los datos anteriormente solicitados en el archivo.
    }
    printf("Registros creados\n\n");
    fclose(file);
    fclose(names);
    printf("%i registros en la tabla\n", contarRegistros(&table));
    printf("Guardando tabla... ");
    SaveTable(&table);
    printf("Tabla guardada\n");
    return 0;
}