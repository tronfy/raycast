CC = gcc
CFLAGS = -O3 -march=native -mtune=native -ffast-math -Wall -Wextra -std=c99
LDFLAGS = -lm
TARGET = tray.exe
SOURCES = main.c engine.c renderer.c input.c map.c
OBJECTS = $(SOURCES:.c=.o)
HEADERS = tray.h

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /f *.o $(TARGET) 2>nul || true

run: $(TARGET)
	.\$(TARGET)

debug: CFLAGS += -g -DDEBUG
debug: $(TARGET)

.PHONY: all clean run debug
