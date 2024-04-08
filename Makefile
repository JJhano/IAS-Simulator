CC = g++
CFLAGS = -std=c++14 -Wall
TARGET = programa
SRC = Main.cpp Memory.h # Agrega los archivos fuente adicionales aquí si los hay

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) Main.cpp -o $(TARGET)

clean:
	rm -f $(TARGET)

