#include "Hash.h"
#include "Functions.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#define TAMANOTABLA 2000

int hash(char *name){
    int index = 0;
    for(int i = 0; i<SIZE; i++){
        if((int)(*(name+i)) == 0)
            break;
        index += (*(name+i)+1)*(i+1);
    }
    return index%TAMANOTABLA;
}

void BorrarNodos(struct Node *node){
    if(node->next != NULL){
        BorrarNodos(node->next);
    }
    node->next = NULL;
    free((void *)node);
}

void SaveList(struct LinkedList *list, int index){
    if(index >= 0){                                                          //Verifica el index de la lista que se va a guardar para saber si existía (>= 0)
        char *fileName = malloc(1);
        char *indexString = IntToString(index);
        *fileName = '\0';
        strcat(fileName, "HashNodes/Node");
        strcat(fileName, indexString);
        strcat(fileName, ".txt");
        FILE *file = fopen(fileName, "w+");
        free(fileName);
        free(indexString);

        struct Node *FN = list->FirstNode;
        while(FN != NULL) {
            int r = 0;
            do{
                r += fwrite(&FN->id, sizeof(long), 1, file);
            } while(r<1);
            do{
                r += fwrite(&FN->name[0], SIZE, 1, file);
            }while(r<2);
            FN = FN->next;
        }
        fclose(file); 
    }
    if(list->FirstNode != NULL){
        BorrarNodos(list->FirstNode);
    }
    list->FirstNode = NULL;
    list->LastNode = NULL;
    list->Size = 0;
}

void CreateList(int index, struct HashTable *table){
    char *fileName = malloc(1), *indexString;                              //Pide memoria para el nombre del archivo donde está la lista
    struct LinkedList newList, *ToDelete;                                           //Crea las estructuras para la lista nueva y para la que va a guardar en disco
    newList.FirstNode = NULL;
    newList.LastNode = NULL;
    newList.Size = 0;
    indexString = IntToString(index);                                               //Convierte el index de la lista en cadena de caracteres
    *fileName = '\0';
    strcat(fileName, "HashNodes/Node");                                             //Concatena cadenas para formar el nombre del archivo
    strcat(fileName, indexString);
    strcat(fileName, ".txt");
    FILE *file = fopen(fileName, "r");                                              //Intenta abrir el archivo
    if(file == NULL){
        file = fopen(fileName, "w+");                                               //Si no lo encuentra lo crea
    } else {
        int read, index, attempts = 0;                                              //Si lo encuentra carga la nueva lista
        newList.FirstNode = NULL;
        newList.LastNode = NULL;
        while(attempts < 5){
            struct Node *nodo;
            do{
                nodo = malloc(sizeof(struct Node));
            }while(nodo == NULL);
            bzero(nodo, sizeof(struct Node));
            read = 0;
            attempts = 0;
            do{
                read += fread(&nodo->id, sizeof(long), 1, file);
                attempts++;
            }while(read != 1 && attempts<5);
            do{
                read += fread(&nodo->name[0], SIZE, 1, file);
            } while(read != 2 && attempts<5);
            if(attempts < 5){
                if(newList.FirstNode == NULL){
                    newList.FirstNode = nodo;
                    newList.LastNode = nodo;
                } else {
                    newList.LastNode->next = nodo;
                    newList.LastNode = nodo;
                }
                newList.Size++;
            } else{
                free(nodo);
            }
        }
    }
    fclose(file);                                                                   //Cierra el archivo
    free(fileName);                                                                 //Libera la memoria del nombre
    free(indexString);                                                              //Libera la memoria del index
    ToDelete = &table->list[NUMOFLISTS-1];                                          //Carga la tabla que va a guardar
    int indexToDlete = table->lastIndex[NUMOFLISTS-1];
    SaveList(ToDelete, indexToDlete);
    for(int i = NUMOFLISTS-1; i>0; i--){
        table->list[i] = table->list[i-1];                                          //Desplaza las demás listas
        table->lastIndex[i] = table->lastIndex[i-1];                                //Desplaza los index de las listas que están en memoria
    }
    table->list[0] = newList;                                                       //Ponde la nueva lista en la hash
    table->lastIndex[0] = index;                                                    //Pone el index de la nueva lista
}

void OrganizeList(int index, struct HashTable *table) {
    int lastIndex;
    for(lastIndex = 0; lastIndex<NUMOFLISTS; lastIndex++){
        if(index == table->lastIndex[lastIndex])
            break;
    }
    if(lastIndex < NUMOFLISTS){
        struct LinkedList res = table->list[lastIndex];
        table->list[lastIndex] = table->list[0];
        table->list[0] = res;
    } else {
        CreateList(index, table);
    }
}

//busca e imprime todos los registros que coincidan con un nombre dado
struct LinkedList buscarId(struct HashTable *table, char *nombre){
    int index;
    char *respaldoNombre = malloc(SIZE);
    bzero(respaldoNombre, SIZE);
    CopyString(nombre, respaldoNombre);
    toUpperCase(respaldoNombre);
    index = hash(respaldoNombre);
    free(respaldoNombre);
    OrganizeList(index, table);

    struct LinkedList Get;
    Get.FirstNode = NULL;
    Get.LastNode = NULL;
    Get.Size = 0;
    struct Node *FN = table->list[0].FirstNode;
    while(FN != NULL){
        if(equals(FN->name, nombre)){
            struct Node *newNode = malloc(sizeof(struct Node));
            bzero(newNode, sizeof(struct Node));
            newNode->id = FN->id;
            CopyString(nombre, newNode->name);
            newNode->next = NULL;
            if(Get.FirstNode == NULL){
                Get.FirstNode = newNode;
                Get.LastNode = newNode;
            } else {
                Get.LastNode->next = newNode;
                Get.LastNode = newNode;
            }
            Get.Size++;
        }
        FN = FN->next;
    }
    return Get;
}

//borra el elemento en la tabla hash que coincida con el id dado en idd y reencadena los demas 
//elementos que esten en la misma posicion de la tabla
long borrar(struct HashTable *table, long id){
    OrganizeList(id%TAMANOTABLA, table);

    struct Node *FN;
    FN->next = table->list[0].FirstNode;
    while(FN->next != NULL){
        if(FN->next->id == id){
            struct Node *res = FN->next;
            FN->next = res->next;
            if(table->list[0].LastNode == table->list[0].FirstNode){
                table->list[0].LastNode = NULL;
                table->list[0].FirstNode = NULL;
            } else if(res == table->list[0].LastNode)
                table->list[0].LastNode = FN;
            else if(res == table->list[0].FirstNode)
                table->list[0].FirstNode = res->next;
            res->next = NULL;
            free((void *)res);
            table->list[0].Size--;
            return id;
        }
        FN = FN->next;
    }
    return -1;
}

/*
Inserta un elemento en la tabla hash
Parametros:
table es la tabla hash donde se va a insertar en elemento
nombre es el nombre del nuevo registro

Devuelve la id con que fue registrado
*/
long insertElement(struct HashTable *table, char *nombre, short AddLast){
    int index;
    long id;
    struct Node *nodo = malloc(sizeof(struct Node));
    char *respaldoNombre = malloc(SIZE);
    bzero(nodo, sizeof(struct Node));
    bzero(respaldoNombre, SIZE);
    CopyString(nombre, nodo->name);
    nodo->next = NULL;
    CopyString(nombre, respaldoNombre);
    toUpperCase(respaldoNombre);
    index = hash(respaldoNombre);
    OrganizeList(index, table);

    if(nodo == NULL){
        return -1;
    } else if(index < 0 || TAMANOTABLA < index){
        free(nodo);
        return -1;
    }
    
    struct Node *FN = table->list[0].FirstNode;
    if(FN == NULL){
        id = index;
        table->list[0].FirstNode = nodo;
        table->list[0].LastNode = nodo;
    }else {
        if(AddLast == 0){
            while(FN->next != NULL && (FN->id + TAMANOTABLA) == FN->next->id){
                FN = FN->next;
            }
            id = FN->id + TAMANOTABLA;
            if(FN->next == NULL){
                asm("jmp AddLast");
            } else {
                nodo->next = FN->next;
                FN->next = nodo;
            }
        } else {
            id = table->list[0].LastNode->id + TAMANOTABLA;
            asm("AddLast:");
            table->list[0].LastNode->next = nodo;
            table->list[0].LastNode = nodo;
        }
        table->list[0].Size++;
    }

    nodo->id = id;
    free(respaldoNombre);
    return id;
}

struct HashTable CreateTable(){
    struct HashTable table;
    for(int i = 0; i < NUMOFLISTS; i++){
        table.list[i].FirstNode = NULL;
        table.list[i].LastNode = NULL;
        table.lastIndex[i] = -1;
        table.list[i].Size = 0;
    }
    return table;
}

void SaveTable(struct HashTable *table){
    for(int i = 0; i<NUMOFLISTS; i++){
        SaveList(&table->list[i], table->lastIndex[i]);
    }
}

char* ListToString(struct LinkedList *list){
    char *ToString = malloc(1);
    if(ToString == NULL) {
        return NULL;
    }
    *ToString = '\0';
    struct Node *FN = list->FirstNode;
    while(FN != NULL) {
        char *IdNode = IntToString(FN->id);
        strcat(ToString, "Id: ");
        strcat(ToString, IdNode);
        strcat(ToString, ", Nombre: ");
        strcat(ToString, &FN->name[0]);
        strcat(ToString, "\n");

        free(IdNode);
        FN = FN->next;
    }
    return ToString;
}

int contarRegistros(struct HashTable *table){
    int registros = 0;
    for(int i = 0; i<TAMANOTABLA; i++){
        CreateList(i, table);
        registros += table->list[0].Size;
    }
    return registros;
}