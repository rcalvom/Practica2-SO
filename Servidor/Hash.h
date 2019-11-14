#ifndef HASHH
#define HASHH
#include <stdio.h>

#define NUMOFLISTS 10

struct Node{
    long id;
    char name[32];
    struct Node* next;
};

struct LinkedList{
    struct Node* FirstNode;
    struct Node* LastNode;
};

struct HashTable{
    struct LinkedList list[NUMOFLISTS];
    int lastIndex[NUMOFLISTS];
};

int hash(char *name);                                                       //Funcion de hash
void SaveList(struct LinkedList list, int index);                           //Guarda la lista en un archivo y libera la memoria, si index es negativo solo libera la memoria de la lista
void CreateList(int index, struct HashTable *table);                        //Carga una lista de disco y guarda la última
void OrganizeList(int index, struct HashTable *table);                      //Pone la lista con el index especificado en la primera posicion, si no la encuentra la crea
struct LinkedList buscarId(struct HashTable *table, char *nombre);          //Busca los elementos que correspondan con el nombre y los devuelve en una lista encadenada
long borrar(struct HashTable *table, long id);                              //Elimina el elemento que corresponda con el ID
long insertElement(struct HashTable *table, char *nombre, short AddLast);   //Obtiene el index de un elemento y llamar a la funcion insertar
struct HashTable CreateTable();                                             //Crea una tabla vacía
void SaveTable(struct HashTable *table);                                    //Guarda la tabla en disco

#endif