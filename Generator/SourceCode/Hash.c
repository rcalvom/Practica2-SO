#include "Hash.h"
#include "Functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

int hash(char *name){
    int index = 0;
    for(int i = 0; i<SIZE; i++){
        if((int)(*(name+i)) == 0)
            break;
        index += (*(name+i)+1)*(i+1);
    }
    if(index < 0){
        index *= -1;
    }
    return index%TAMANOTABLA;
}

char* GetFileName(int index){
    index = index%TAMANOTABLA;
    char *fileName = (char *) malloc(1);
    char *indexString = IntToString(index);
    *fileName = '\0';
    strcat(fileName, "HashNodes/Node");
    strcat(fileName, indexString);
    strcat(fileName, ".txt");
    free(indexString);
    return fileName;
}

//busca e imprime todos los registros que coincidan con un nombre dado
char* buscarId(struct HashTable *table, char *nombre){
    int index;
    char *respaldoNombre = malloc(SIZE), *fileName;
    bzero(respaldoNombre, SIZE);
    CopyString(nombre, respaldoNombre);
    toUpperCase(respaldoNombre);
    index = hash(respaldoNombre);
    fileName = GetFileName(index);
    FILE *file = fopen(fileName, "r");
    if(file == NULL){
        free(respaldoNombre);
        free(fileName);
        return '\0';
    }

    char *ToString = malloc(1);
    if(ToString == NULL) {
        free(respaldoNombre);
        free(fileName);
        fclose(file);
        return '\0';
    }
    *ToString = '\0';
    while(feof(file) == 0) {
        long id;
        int r = fread(&id, sizeof(long), 1, file);
        r += fread(respaldoNombre, SIZE, 1, file);
        if(r < 2){
            break;
        }
        if(equals(nombre, respaldoNombre)){
            char *Id = IntToString(id);

            ToString = realloc(ToString, strlen(ToString) + 67);
            //bzero(ToString + strlen(ToString),68);
            strcat(ToString, "Id: ");
            strcat(ToString, Id);
            strcat(ToString, ", Nombre: ");
            strcat(ToString, respaldoNombre);
            strcat(ToString, "\n\0");

            //printf("ID: %s, Nombre: %s\n", Id, respaldoNombre);

            free(Id);
        }
    }

    free(respaldoNombre);
    free(fileName);
    fclose(file);
    return ToString;
}

//borra el elemento en la tabla hash que coincida con el id dado en idd y reencadena los demas 
//elementos que esten en la misma posicion de la tabla
long borrar(struct HashTable *table, long id){
    char *fileName = GetFileName(id);
    FILE *file = fopen(fileName, "r+");
    free(fileName);
    if(file == NULL)
        return -1;
    while(feof(file) == 0){
        long FNId;
        int r = 0;
        r += fread(&FNId, sizeof(long), 1, file);
        for(int i = 0; i<SIZE; i++){
            char c = fgetc(file);
        }
        if(FNId == id){
            fseek(file, -40, SEEK_CUR);
            int w = 0; long p = -1;
            w += fwrite(&p, sizeof(long), 1, file);
            for (int i = 0; i< SIZE; i++)
                fputc('\0', file);
            table->LastIndex[id%TAMANOTABLA] = -1;
            table->Elements--;
	        fclose(file);
            return id;
        } else if(FNId > id)
	        break;
    }
    fclose(file);
    return -1;
}

/*
Inserta un elemento en la tabla hash
Parametros:
table es la tabla hash donde se va a insertar en elemento
nombre es el nombre del nuevo registro

Devuelve la id con que fue registrado
*/
long insertElement(struct HashTable *table, char *nombre){
    int index;
    long id;
    char *respaldoNombre = malloc(SIZE), *fileName;
    bzero(respaldoNombre, SIZE);
    CopyString(nombre, respaldoNombre);
    toUpperCase(respaldoNombre);
    index = hash(respaldoNombre);
    fileName = GetFileName(index);
    FILE *reg;

    if(table->LastIndex[index] >= 0){
        id = table->LastIndex[index] + TAMANOTABLA;
        table->LastIndex[index] = id;
        reg = fopen(fileName, "a");
    } else if(table->LastIndex[index] == -1) {
        reg = fopen(fileName, "r+");
        long ResId;
        while(feof(reg) == 0){
            int r = fread(&ResId, sizeof(long), 1, reg);
            r += fread(respaldoNombre, SIZE, 1, reg);
            if(ResId == -1){
                asm("jmp Find");
            }
            id = ResId;
        }
        id = ResId+TAMANOTABLA;
        table->LastIndex[index] = id;
        asm("jmp END");
        asm("Find:");
        int f = fseek(reg, -40, SEEK_CUR);
        id = id+TAMANOTABLA;
        asm("END:");
    } else if (table->LastIndex[index] == -2) {
        reg = fopen(fileName, "w+");
        id = index;
        table->LastIndex[index] = id;
    }
    int w = fwrite(&id, sizeof(long), 1, reg);
    w += fwrite(nombre, SIZE, 1, reg);
    free(fileName);
    free(respaldoNombre);
    fclose(reg);
    table->Elements++;
    return id;
}

struct HashTable* CreateTable(){
    struct HashTable *table = malloc(sizeof(struct HashTable));
    FILE *f = fopen("HashNodes/Data.txt", "r");
    if(f == NULL)
        table->Elements = 0;
    else {
        int r = fread(&table->Elements, sizeof(int), 1, f);
        fclose(f);
    }
    for(int i = 0; i<TAMANOTABLA; i++){
        char *fileName = GetFileName(i);
        f = fopen(fileName, "r");
        if(f == NULL){
            table->LastIndex[i] = -2;
        } else {
            table->LastIndex[i] = -1;
            fclose(f);
        }
        free(fileName);
    }
    return table;
}

void SaveTable(struct HashTable *table){
    FILE *f = fopen("HashNodes/Data.txt", "w+");
    int w = fwrite(&table->Elements, sizeof(int), 1, f);
    fclose(f);
    free(table);
}
