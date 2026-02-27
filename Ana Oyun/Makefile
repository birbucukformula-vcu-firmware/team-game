CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LIBS = -lraylib -lm -lopengl32 -lgdi32 -lwinmm
TARGET = formula

SOURCES = main.c game_state.c car.c particles.c weather.c track.c entities.c ui.c graphics.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /Q *.o $(TARGET).exe 2>nul || true

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
