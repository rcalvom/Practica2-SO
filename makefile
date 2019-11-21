CC = gcc

server:	Server/SourceCode/p2-dogServer.c
	$(CC) -IServer/Headers Server/SourceCode/Opciones.c Server/SourceCode/Functions.c Server/SourceCode/p2-dogServer.c Server/SourceCode/Hash.c -o Server/p2-dogServer -lpthread

client:	Client/SourceCode/p2-dogClient.c
	$(CC) -IClient/Headers Client/SourceCode/Opciones.c Client/SourceCode/Functions.c Client/SourceCode/p2-dogClient.c -o Client/p2-dogClient -lncurses

all: Server/SourceCode/p2-dogServer.c Client/SourceCode/p2-dogClient.c
	$(CC) -IServer/Headers Server/SourceCode/Opciones.c Server/SourceCode/Functions.c Server/SourceCode/p2-dogServer.c Server/SourceCode/Hash.c -o Server/p2-dogServer -lpthread
	$(CC) -IClient/Headers Client/SourceCode/Opciones.c Client/SourceCode/Functions.c Client/SourceCode/p2-dogClient.c -o Client/p2-dogClient -lncurses

clean: rm Server/SourceCode/p2-dogServer.c rm Client/SourceCode/p2-dogClient.c