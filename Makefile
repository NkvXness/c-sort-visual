CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -O2 -g
TARGET  = sort_visual

SRC = src/main.c src/array.c src/sort.c src/visualizer.c src/ui.c
OBJ = $(SRC:.c=.o)

# ── Platform detection ────────────────────────────────────────────────────────
ifeq ($(OS),Windows_NT)
    TARGET  := sort_visual.exe
    LDFLAGS  = -lraylib -lopengl32 -lgdi32 -lwinmm
else
    UNAME := $(shell uname -s)
    ifeq ($(UNAME),Darwin)
        LDFLAGS = -lraylib -framework OpenGL -framework Cocoa -framework IOKit
    else
        LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    endif
endif

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)