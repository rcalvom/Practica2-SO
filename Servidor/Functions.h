#ifndef FUNCIONESH
#define FUNCIONESH

#define SIZE 32

#include <stdbool.h>

void PressToContinue();
void InitConsole();
void DisposeConsole();
bool equals(char *String1, char *String2);
char* IntToString(unsigned int toConvert);
void toUpperCase(char *word);
void toLowerCase(char *word);
void CopyString(char *original, char *copy);
void WriteLog(int opcion, char* IP, char* Registro);
char* FilePath(long id); 
bool ExisteRegistro(long id);

#endif