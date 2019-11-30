#ifndef FUNCIONESH
#define FUNCIONESH
#include <stdio.h> 

#define SIZE 32

void PressToContinue();
void InitConsole();
void DisposeConsole();
_Bool equals(char *String1, char *String2);
char* IntToString(unsigned long toConvert);
void toUpperCase(char *word);
void toLowerCase(char *word);
void CopyString(char *original, char *copy);
void WriteLog(int opcion, char* IP, char* Registro);
void leer(char* name, FILE* file);


#endif