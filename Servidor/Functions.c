#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "Functions.h"

// Bloquea el subproceso hasta que el usuario pulse una tecla para continuar.
void PressToContinue(){
    printw("\nPulse cualquier tecla para continuar...\n");                        // Espera por una tecla para continuar.
    noecho();                                                                     // Impide que se muestre el caracter en consola.
    getch();                                                                      // Obtiene el caracter.
    echo();                                                                       // Permite de nuevo el ingreso de caracteres.
    clear();
}

// Inicializa la configuración de ncurses en la consola.
void InitConsole(){
    initscr();                                                                   //Inicializa la consola para uso de la libreria ncurses.
    clear();                                                                     //Limpia la consola.
    refresh();                                                                   //Refresca la consola para poder ser escrita.
}

// Elimina la configuración de ncurses en la consola.
void DisposeConsole(){
    clear();
    endwin();                                                                   //Antes de finalizar, elimina la configuración hecha sobre la consola por parte de la libreria ncurses.
}

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
char* IntToString(unsigned int toConvert){
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

//copia el cada caracter de la cadena original en la cadena copia
void CopyString(char *original, char *copia){
    for(int i = 0; i<SIZE; i++){
        *(copia+i) = *(original+i);
        if(*(original+i) == '\0')
            break;
    }
}
