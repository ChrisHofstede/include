CC = gcc
CPPFLAGS = -Wall -O3 -D STRICT
LDFLAGS = -mwindows -lsupc++ -lole32
VPATH = 

OBJECTS = main.o registry.o utf8.o
EXE = include.exe

$(EXE) : $(OBJECTS)
	$(CC) -o $(EXE) $(OBJECTS) $(LDFLAGS)
	
main.o : main.cpp

registry.o : registry.cpp

utf8.o : utf8.c
	
clean :
	rm -f $(EXE) $(OBJECTS)