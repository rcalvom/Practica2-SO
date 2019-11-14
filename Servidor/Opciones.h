#ifndef OPCIONESH
#define OPCIONESH
#include "Funciones.h"

// Estructura de almacenamiento de datos.
struct dogType{
    char name[32];
    char type[32];
    int age;
    char breed[16];
    int height;
    float weight;
    char gender;     
};

//Funciones correspondientes a las opciones indicadas.
int IngresarRegistro(struct List* tabla);
int VerRegistro();
int BorrarRegistro(struct List* tabla);
int BuscarRegistro(struct List* tabla);
void Salir(_Bool *valor);
int Generado(struct List* tabla);

#endif