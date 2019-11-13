#include <ncurses.h>
#include <stdio.h>

void PressToContinue(){
    printw("\nPulse cualquier tecla para continuar...\n");                        // Espera por una tecla para continuar.
    noecho();                                                                     // Impide que se muestre el caracter en consola.
    getch();                                                                      // Obtiene el caracter.
    echo();                                                                       // Permite de nuevo el ingreso de caracteres.
    clear();
}

void InitConsole(){
    initscr();                                                                   //Inicializa la consola para uso de la libreria ncurses.
    clear();                                                                     //Limpia la consola.
    refresh();                                                                   //Refresca la consola para poder ser escrita.
}

void DisposeConsole(){
    clear();
    endwin();                                                                   //Antes de finalizar, elimina la configuración hecha sobre la consola por parte de la libreria ncurses.
}
