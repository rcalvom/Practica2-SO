#ifndef HASHH
#define HASHH

#define NUMOFLISTS 2000

struct Node{
    long id;
    char name[32];
    struct Node *next;
};

struct LinkedList{
    struct Node *FirstNode;
    struct Node *LastNode;
    int Size;
};

struct HashTable{
    struct LinkedList list[NUMOFLISTS];
    int lastIndex[NUMOFLISTS];
};

int hash(char *name);                                                       //Funcion de hash
void SaveList(struct LinkedList *list, int index);                          //Guarda la lista en un archivo y libera la memoria, si index es negativo solo libera la memoria de la lista
struct LinkedList buscarId(struct HashTable *table, char *nombre);          //Busca los elementos que correspondan con el nombre y los devuelve en una lista encadenada
long borrar(struct HashTable *table, long id);                              //Elimina el elemento que corresponda con el ID
long insertElement(struct HashTable *table, char *nombre, short AddLast);   //Obtiene el index de un elemento y llamar a la funcion insertar
struct HashTable CreateTable();                                             //Crea una tabla vacía
void SaveTable(struct HashTable *table);                                    //Guarda la tabla en disco
char* ListToString(struct LinkedList *list);                                //Convierte a list en una cadena de caracteres y devuelve un apuntador donde está el string
int contarRegistros(struct HashTable *table);

#endif