CC=gcc

server:    SourceCode/p2-dogServer.c
	$(CC) -IHeaders SourceCode/Opciones.c SourceCode/Functions.c SourceCode/p2-dogServer.c SourceCode/Hash.c -o p2-dogServer -lpthread

all: SourceCode/p2-dogServer.c
	$(CC) -IHeaders SourceCode/Opciones.c SourceCode/Functions.c SourceCode/p2-dogServer.c SourceCode/Hash.c -o p2-dogServer -lpthread

clean: rm SourceCode/p2-dogServer.c