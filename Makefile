CC = gcc
CFLAGS = -O3 -march=native -mtune=native -ffast-math -Wall -Wextra -std=c99
LDFLAGS = -lm
TARGET = tray.x
SOURCES = main.c engine.c renderer.c input.c map.c
OBJECTS = $(SOURCES:.c=.o)
HEADERS = tray.h

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET)

run: $(TARGET)
	./$(TARGET)

debug: CFLAGS += -g -DDEBUG
debug: $(TARGET)

.PHONY: all clean run debug
