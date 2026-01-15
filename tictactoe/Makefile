# Compiler setup
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

# SDL2 location (Update this if you move the folder)
SDL_PATH = E:/SDL2_A

# Include paths - covers both <SDL.h> and <SDL2/SDL.h> styles
INCLUDES = -I$(SDL_PATH)/include -I$(SDL_PATH)/include/SDL2

# Linker flags
# NOTE: watch the priority order (mingw32 -> SDL2main -> SDL2 -> extensions)
LIBS = -L$(SDL_PATH)/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf

# Project files
TARGET = tictactoe
SRCS = main.c
OBJS = $(SRCS:.c=.o)

# Main build target
all: $(TARGET)

# Linking step
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LIBS)
	@echo "Build finished. Run ./$(TARGET) to start."

# Compiling step
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Cleanup
clean:
	rm -f $(OBJS) $(TARGET) $(TARGET).exe
