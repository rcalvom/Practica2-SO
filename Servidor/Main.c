#include "Funciones.h"
#include "Hash.h"

int main(){
    struct HashTable table = CreateTable();
    printf("Index: %li\n", insertElement(&table, "Juan", 0));
    printf("%li\n", insertElement(&table, "Pedro", 0));
    printf("%li\n", insertElement(&table, "Natalia", 0));
    printf("%li\n", insertElement(&table, "Carlos", 0));
    printf("%li\n", insertElement(&table, "Ricardo", 0));
    printf("%li\n", insertElement(&table, "Samuel", 0));
    printf("%li\n", insertElement(&table, "Jose", 0));
    printf("%li\n", insertElement(&table, "Camilo", 0));
    printf("%li\n", insertElement(&table, "Daniel", 0));
    printf("%li\n", insertElement(&table, "Esteban", 0));
    printf("%li\n", insertElement(&table, "EL_Papa", 0));
    printf("%li\n", insertElement(&table, "Antonio", 0));
    printf("%li\n", insertElement(&table, "Janethe", 0));
    printf("%li\n", insertElement(&table, "Joseph", 0));
    SaveTable(&table);
}