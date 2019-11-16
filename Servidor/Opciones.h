#ifndef OPCIONESH
#define OPCIONESH
#include "Functions.h"
#include "Hash.h"
#include "ShippingData.h"

//Funciones correspondientes a las opciones indicadas.
int IngresarRegistro(struct HashTable* tabla, struct dogType *new);
int VerRegistro(long id);
int BorrarRegistro(struct HashTable* tabla, long id);
int BuscarRegistro(struct HashTable* tabla, char *nombre);
int Generado(struct HashTable* tabla);

#endif