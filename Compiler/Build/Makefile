CC = gcc
CFLAGS = -W -WALL
TARGET = RE

$(TARGET) : Grammer.o Bra.o Pa.o NFA.o DFA.o main.cpp
	$(CC) $(CFLAGS) -o $(TARGET) Grammer.o Bra.o Pa.o NFA.o DFA.o main.o


