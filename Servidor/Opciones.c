#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ncurses.h> 
#include "Opciones.h"
#include "Funciones.h"

//Opción del menú Ingresar Registro.
int IngresarRegistro(struct List* tabla){
    struct dogType* newRegister;                                            // Se crean las variables, apuntadores y espacios de memoria.
    int r = 0;
    long idd;
    FILE *file, *historia;
    char* tipo, *values, *mensajeHistoria, *filepath, *filename;
    tipo = (char *) malloc(32);
    newRegister = (struct dogType*)malloc(sizeof(struct dogType));
    bzero(newRegister, sizeof(struct dogType));
    bzero(tipo, 32);
    if(newRegister == NULL){
        printw("Fallo reservando espacio de memoria.\n");
        return -1;
    } 
    printw("Ingresar registro:\n");                                         // Se piden los datos y se almacenan en la estructura.
    printw("Indique el nombre de la mascota: ");
    scanw("%s",newRegister->name);
    printw("Indique el tipo de mascota: ");
    scanw("%s", tipo);
    printw("Indique la edad de la mascota: ");
    scanw("%i", &newRegister->age);
    printw("Indique la raza de la mascota: ");
    scanw("%s", newRegister->breed);
    printw("Indique la estatura (cm) de la mascota: ");
    scanw("%i", &newRegister->height);
    printw("Indique el peso (kg) de la mascota: ");
    scanw("%f", &newRegister->weight);
    printw("Indique el género (M/H) de la mascota: ");
    scanw("%s", &newRegister->gender);
    idd = insertElement(tabla, newRegister->name);                          // Se agrega el elemento a la tabla hash y se obtiene su id.
    copiarCadena(tipo, newRegister->type);
    free(tipo);
    file = fopen("dataDogs.dat","a");                                       // Se abre el archivo "dataDogs.dat" y se coloca el puntero al final.
    if(file == NULL){
        printw("El archivo no ha podido ser abierto.");
        return -1;
    }
    r += fwrite(&idd, sizeof(long), 1, file);                               // Se escribe en el archivo la Id correspondiente.
    r += fwrite(newRegister,sizeof(struct dogType),1,file);                 // Se escriben los datos de la mascota anteriormente solicitados en el archivo.
    if(r == 0){                                                             // Se comprueba el posible error.
        printw("\nEl archivo no ha podido ser escrito.\n");
        return -1;
    } else if(r<2){
        printw("No se escribieron todos los datos\n");
    }
    printw("\nlos datos de %s han sido escritos correctamente.\n",&newRegister->name);
    filepath = (char*) malloc(46);                                         // Se libera espacio para la ruta de la historia clinica.
    filename = (char*) malloc(16);
    bzero(filepath, 46);
    bzero(filename, 16);
    sprintf(filename,"%li",idd);
    strcat(filepath,"historias/");                                          // Se concatenan la extensión y la ubicación.
    strcat(filepath,filename);
    strcat(filepath,".dat");
    toLowerCase(filepath);
    historia = fopen(filepath,"w+");                                        // Se crea el archivo de historia clinica.
    if(historia == NULL){
        printw("No se ha podido crear la historia\n");
        return -1;
    }
    mensajeHistoria = (char*) malloc(175);
    values = (char*) malloc(10);
    bzero(mensajeHistoria, 175);
    bzero(values, 10);
    strcat(mensajeHistoria,"Historia clinica.\n\nNombre: ");               //Escribe en el archivo de historia clinica toda la información de la mascota
    strcat(mensajeHistoria,newRegister->name);
    strcat(mensajeHistoria,"\nTipo: ");
    strcat(mensajeHistoria,&newRegister->type[0]);
    strcat(mensajeHistoria,"\nEdad: ");
    sprintf(values,"%i",newRegister->age);
    strcat(mensajeHistoria,values);
    strcat(mensajeHistoria,"\nRaza: ");
    strcat(mensajeHistoria,&newRegister->breed[0]);
    strcat(mensajeHistoria,"\nEstatura: ");
    sprintf(values,"%i",newRegister->height);
    strcat(mensajeHistoria,values);
    strcat(mensajeHistoria,"\nPeso: ");
    sprintf(values,"%f",newRegister->weight);
    strcat(mensajeHistoria,values);
    strcat(mensajeHistoria,"\nGénero: ");
    strcat(mensajeHistoria,&newRegister->gender);
    r = 0;
    r += fwrite(mensajeHistoria,strlen(mensajeHistoria),1,historia);
    if(r == 0){
        printw("El archivo de historia clinica no ha podido ser escrito.");
        return -1;
    }
    fclose(historia);                                                       // Cierra el archivo de historia clinica.
    printw("Archivo de historia clinica creado.\n");                        // Indica que la historia clinica ya fue creada.
    printw("\nPulse cualquier tecla para continuar...\n");                  // Espera una tecla para continuar.    
    noecho();                                                               // Impide que el caracter se muestre en pantalla.
    getch();                                                                // Recibe el caracter.
    echo();                                                                 // Permite de nuevo que se muestren caracteres en pantalla.
    clear();                                                                // Limpia la consola.
    free(newRegister);                                                      // Libera espacio ya no necesario.
    free(filepath);
    free(filename);
    free(mensajeHistoria);
    free(values);
    fclose(file);                                                           // Cierra el archivo dataDogs.dat.
    return 0;
}

//Opción del menú Ver Registro.
int VerRegistro(){
    printw("Ver registro.\n");
    printw("Indique el ID del registro que desea buscar: ");
    long id = 0;
    char* idchar = (char*) malloc(5);                                       // Se declaran variables para las rutas y bach, luego se les reserva espacio.
    bzero(idchar,5);
    char* filepath = (char*) malloc(46);
    bzero(filepath,46);
    char* batch = (char*) malloc(46);
    bzero(batch,46);
    if(filepath == NULL || batch == NULL){                                  // Se comprueba posible error.
        printw("Error reservando memoria.");
        return -1;
    }

    scanw("%li",&id);                                                       // Se pide el dato y se concatena para obtener la ruta.
    strncat(filepath,"historias/",10);
    sprintf(idchar,"%li",id);
    strncat(filepath,idchar,strlen(idchar));
    strncat(filepath,".dat",4);
    strncat(batch,"nano ",5);
    strncat(batch,filepath,strlen(filepath));
    FILE* file = fopen(filepath,"r");                                       // Se comprueba si el archivo existe.
    if(file == NULL){                                                       // Si no existe...
        printw("\nEl registro dado no se ha encontrado en el sistema.");
    }else{                                                                  // si existe...
        printw("\nEl registro se encuentra en el sistema.\n");
        printw("¿Desea abrir el registro en un editor de texto? (Y/N): ");
        char option;
        scanw("%c",&option);                                                // se pregunta si se desea abrir.
        if(option == 'Y' || option == 'y'){
            system(batch);                                                  // se ejecuta un bach para la edicion de la historia clinica.
            initscr();                                                      // Al salir del bach se reestablece la configuración de consola para la escritura.
            clear();                                                        // limpia la consola.
            refresh();                                                      // Refresca la consola.
        }
    }
    free(idchar);                                                           // librera memoria.
    free(filepath);
    free(batch); 
    printw("\nPulse cualquier tecla para continuar...\n");                  // Espera por una tecla para continuar.
    noecho();                                                               // Impide que se muestre el caracter en consola.
    getch();                                                                // Obtiene el caracter.
    echo();                                                                 // Permite de nuevo el ingreso de caracteres.
    clear();                                                                // Limpia la consola
    if(file != NULL){
        fclose(file);
    }
    return 0;
}

//Opción del menú Borrar Registro.
int BorrarRegistro(struct List* tabla){
    struct dogType *registro = (struct dogType*)malloc(sizeof(struct dogType));     // Declara variables que se van a utilizar.
    bzero(registro, sizeof(struct dogType));
    long id, idTemp;
    int r, registros;
    FILE *file, *temp;
    printw("Borrar registro.\n\n");
    do{
        registros = contarRegistros(tabla);
    }while(registros == -1);
    printw("Existen %i registros\n", registros);
    printw("Ingrese el ID de la mascota de la cual va a borrar el registro (-1 para cancelar): ");
    scanw("%li",&id);                                                               // Se pide el id del registro a borrar y se almacena.
    if(id == -1){
        asm("jmp End");
    }
    printw("\nBuscando Registro a eliminar.");
    file = fopen("dataDogs.dat","r");                                               // Se abre un archivo que contiene las estructuras..
    temp = fopen("temp.dat","w+");                                                  // Se crea un archivo temporal donde se guardaran las estructuras que nos serán eliminadas.
    if(file == NULL || temp == NULL){
        printw("\nNo se ha podido abrir el archivo.");
        return -1;
    }
    do{                                                                             // Mientras el archivo aún tenga estructuras....
        r = fread(&idTemp,sizeof(long),1,file);
        if(r == 0){
            break;
            printw("Error leyendo archivo");
            return -1;
        }
        r = fread(registro,sizeof(struct dogType),1,file);                          // ..... Lea los datos .....
        if(r == 0){
            printw("Error leyendo archivo.");
            return -1;
        }else if(idTemp == id){
            continue;                                                               // ... y exceptuando el que se va a eliminnar...
        }
        r = fwrite(&idTemp,sizeof(long),1,temp);                                    // ... Escribirlos todos en el archivo temporal
        if(r == 0){
            printw("Error escribiendo archivo archivo.");
            return -1;
        }
        r = fwrite(registro,sizeof(struct dogType),1,temp);
        if(r == 0){
            printw("Error escribiendo archivo archivo.");
            return -1;
        }
    }while(feof(file) == 0);
    fclose(file);
    fclose(temp);
    remove("dataDogs.dat");                                                         // Se elimina el archivo viejo y ....
    rename("temp.dat","dataDogs.dat");                                              // ... Se renombra el archivo temporal
    char* his = (char*) malloc(46);
    char* his1 = (char*) malloc(5);
    bzero(his,46);
    bzero(his1,5);
    strcat(his,"rm historias/");                                                    // Se elimina el archivo de historia clinica usando un bach del sistema.
    sprintf(his1,"%li",id);
    strcat(his,his1);
    strcat(his,".dat");
    system(his);
    free(his);
    free(his1);
    borrar(tabla, id);
    asm("End:");
    free(registro); 
    printw("\nPulse cualquier tecla para continuar...\n");                        // Espera por una tecla para continuar.
    noecho();                                                                     // Impide que se muestre el caracter en consola.
    getch();                                                                      // Obtiene el caracter.
    echo();                                                                       // Permite de nuevo el ingreso de caracteres.
    clear();  
    return 0;
}

//Opción del menú Buscar Registro.
int BuscarRegistro(struct List* tabla){
    printw("Buscar registro.\n");
    char *nameToSearch = (char *) malloc(32);
    if(nameToSearch == NULL){
        return -1;
    }
    bzero(nameToSearch, 32);
    printw("Indique el nombre de la mascota: ");
    scanw("%s", nameToSearch);                                              // Solicita el nombre a buscar en la tabla Hash.
    printw("\n");
    buscarId(tabla, nameToSearch);                                          // Se pide a la tabla que imprima los elementos que contengan el nombre dado.
    free(nameToSearch);
    printw("Pulse cualquier tecla para continuar...\n");                    // Espera por una tecla para continuar.
    noecho();                                                               // Impide que se muestre el caracter en consola.
    getch();                                                                // Obtiene el caracter.
    echo();                                                                 // Permite de nuevo el ingreso de caracteres.
    clear();
    return 0;
}

//Opción del menú Salir.
void Salir(_Bool *valor){
    printw("Salir.\n\n");
    endwin();                                                               // Elimina el modo de impresión en consola "ncurses"
    *valor = TRUE;                                                          // Cambia el valor de la variable para que el menú no se vuelva a ejecutar.
}

int Generado(struct List* tabla){
    struct dogType *newRegister;                                            //Se crea el struct
    int r, attempts;
    char *values, *mensajeHistoria, *filename, *filepath;
    long idd;
    FILE *file, *historia;                                                // Cierra el archivo dataDogs.dat.
    file = fopen("dataDogs.dat","r");
    do{
        attempts = 0;
        newRegister = (struct dogType *) malloc(sizeof(struct dogType));
        r = 0;
        do{
            r += fread(&idd, sizeof(long), 1, file);
            if(r == 0) attempts++;
        } while(r == 0 && attempts < 5);
        do{
            r += fread(newRegister, sizeof(struct dogType), 1, file);
            if(r == 1) attempts++;
        } while(r == 1 && attempts < 5);
        if(attempts <5){
            filepath = (char*) malloc(46);                                         // Se libera espacio para la ruta de la historia clinica.
            filename = (char*) malloc(16);
            bzero(filepath, 46);
            bzero(filename, 16);
            sprintf(filename,"%li",idd);
            strcat(filepath,"historias/");                                          // Se concatenan la extensión y la ubicación.
            strcat(filepath,filename);
            strcat(filepath,".dat");
            toLowerCase(filepath);
            historia = fopen(filepath,"w+");                                        // Se crea el archivo de historia clinica.
            if(historia == NULL){
                printw("No se ha podido crear la historia\n");
                return -1;
            }
            mensajeHistoria = (char*) malloc(175);
            values = (char*) malloc(10);
            bzero(mensajeHistoria, 175);
            bzero(values, 10);
            strcat(mensajeHistoria,"Historia clinica.\n\nNombre: ");               //Escribe en el archivo de historia clinica toda la información de la mascota
            strcat(mensajeHistoria,newRegister->name);
            strcat(mensajeHistoria,"\nTipo: ");
            strcat(mensajeHistoria,&newRegister->type[0]);
            strcat(mensajeHistoria,"\nEdad: ");
            sprintf(values,"%i",newRegister->age);
            strcat(mensajeHistoria,values);
            strcat(mensajeHistoria,"\nRaza: ");
            strcat(mensajeHistoria,&newRegister->breed[0]);
            strcat(mensajeHistoria,"\nEstatura: ");
            sprintf(values,"%i",newRegister->height);
            strcat(mensajeHistoria,values);
            strcat(mensajeHistoria,"\nPeso: ");
            sprintf(values,"%f",newRegister->weight);
            strcat(mensajeHistoria,values);
            strcat(mensajeHistoria,"\nGénero: ");
            strcat(mensajeHistoria,&newRegister->gender);
            r = 0;
            r += fwrite(mensajeHistoria,strlen(mensajeHistoria),1,historia);
            if(r == 0){
                printw("El archivo de historia clinica no ha podido ser escrito.");
                return -1;
            }
            fclose(historia);
            free(filename);
            free(filepath);
            free(values);
            free(mensajeHistoria);
        }
        free(newRegister);
    } while(attempts < 5);
    fclose(file);
    return 0;
}