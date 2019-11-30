#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include "Functions.h"

// Determina si dos cadenas de carcteres son iguales
_Bool equals(char *String1, char *String2){
    for(int i = 0; i<SIZE; i++){
        if(*(String1 + i) != *(String2 + i))
            return 0;
        if((int)(*(String1+i)) == 0)
            break;
    }
    return 1;
}

// Convierte un entero a una cadena de caracteres.
char* IntToString(unsigned long toConvert){
    char *c = malloc(2);
    *c = (char)(toConvert%10 + '0');
    *(c+1) = '\0';
    toConvert /= 10;
    while(toConvert > 0){
        char *a = malloc(2);
        *a = (char)(toConvert%10 + '0');
        *(a+1) = '\0';
        strcat(a, c);
        toConvert /= 10;
        free(c);
        c = a;
    }
    return c;
}

//Pasar cada caracter de una cadena a mayúsculas.
void toUpperCase(char *word){
    for(int i = 0; i<SIZE; i++){
        if(*(word+i) >= 'a' && *(word+i)<='z')
            *(word+i) = (char)(*(word+i) - 32);
    }
}

//Pasar cada caracter de una cadena a minusculas.
void toLowerCase(char *word){
    for(int i = 0; i<SIZE; i++){
        if(*(word+i) >= 'A' && *(word+i)<='Z')
            *(word+i) = (char)(*(word+i) + 32);
    }
}

//Copia el cada caracter de la cadena original en la cadena copia
void CopyString(char *original, char *copia){
    for(int i = 0; i<SIZE; i++){
        *(copia+i) = *(original+i);
        if(*(original+i) == '\0')
            break;
    }
}

// Escribe el archivo de registro con la información pertinente.
void WriteLog(int opcion, char* IP, char* Registro){
    int r;
    time_t t; struct tm *tm;
    char* log = malloc(256);
    bzero(log,256);
    char* date = malloc(50);
    bzero(date,50);
    FILE *file;

    file = fopen("serverDogs.log","a");
    if(file == NULL){
        perror("El archivo de registro no pudo ser abierto.\n");
        return;
    }

    t = time(NULL);
    tm = localtime(&t);
    strftime(date,50,"%Y%m%d%H%M%S",tm);

    strcat(log,"[Fecha ");
    strcat(log,date);
    strcat(log,"] Cliente[");
    strcat(log,IP);
    switch (opcion){
        case 1:
            strcat(log,"] [Insersión] [");
            break;
        case 2:
            strcat(log,"] [Lectura] [");
            break;
        case 3:
            strcat(log,"] [Borrado] [");
            break;
        case 4:
            strcat(log,"] [Búsqueda] [");
            break;
    }

    strcat(log,Registro);
    strcat(log,"]\n");

    r = fwrite(log,strlen(log),1,file);
    if(r == 0){
        perror("El archivo de registros no pudo ser escrito.\n");
        return;
    }

    fclose(file);
    free(log);
    free(date);
}

// Lee un nombre de un archivo.
void leer(char* name, FILE* file){
    for(int j = 0; j<1700; j++){
        int i = 0;
        char *a;
        do{
            a = (name + i + (32*j));
            fread(a, 1, 1, file);
            i++;
        }while(*a !='\n' && i <32);
        *a = '\0';
    }
}