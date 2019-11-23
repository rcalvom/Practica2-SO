CC = gcc

all: Server/SourceCode/p2-dogServer.c Client/SourceCode/p2-dogClient.c Generator/SourceCode/Generator.c
	@ $(CC) -IServer/Headers Server/SourceCode/Opciones.c Server/SourceCode/Functions.c Server/SourceCode/p2-dogServer.c Server/SourceCode/Hash.c -o Server/p2-dogServer -lpthread
	@ $(CC) -IClient/Headers Client/SourceCode/Opciones.c Client/SourceCode/Functions.c Client/SourceCode/p2-dogClient.c -o Client/p2-dogClient -lncurses
	@ $(CC) -IGenerator/Headers Generator/SourceCode/Hash.c Generator/SourceCode/Functions.c Generator/SourceCode/Generator.c -o Generator/Generator 
	@ echo "Se ha compilado todo correctamente."

server:	Server/SourceCode/p2-dogServer.c
	@ $(CC) -IServer/Headers Server/SourceCode/Opciones.c Server/SourceCode/Functions.c Server/SourceCode/p2-dogServer.c Server/SourceCode/Hash.c -o Server/p2-dogServer -lpthread
	@ echo "Se ha compilado la aplicación servidor correctamente."

client:	Client/SourceCode/p2-dogClient.c
	@ $(CC) -IClient/Headers Client/SourceCode/Opciones.c Client/SourceCode/Functions.c Client/SourceCode/p2-dogClient.c -o Client/p2-dogClient -lncurses
	@ echo "Se ha compilado la aplicación cliente correctamente."

generator:	Generator/SourceCode/Generator.c
	@ $(CC) -IGenerator/Headers Generator/SourceCode/Hash.c Generator/SourceCode/Functions.c Generator/SourceCode/Generator.c -o Generator/Generator 
	@ echo "Se ha compilado la aplicación del generador de estructuras correctamente."

clean:
	@ rm Server/p2-dogServer rm Client/p2-dogClient rm Generator/Generator
	@ echo "\nSe han eliminado los archivos ejecutables correctamente."