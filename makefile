# Basic variables
CC = gcc
CFLAGS = -I ./include -I. -Wall
LIBS = -lglfw -lGL -lcglm -lm -ldl

# Directories
SRC_DIR = source
PROG_DIR = program
GLAD_DIR = glad
OBJ_DIR = obj
EXE_DIR = build

# Source files
SRCS = $(PROG_DIR)/main.c $(GLAD_DIR)/glad.c $(wildcard $(SRC_DIR)/*.c)

# Object files
OBJS = $(patsubst %.c, $(OBJ_DIR)/%.o, $(notdir $(SRCS)))

# Target folder
TARGET = $(EXE_DIR)/planets

# Default rule
all: $(TARGET)

# Linking
$(TARGET): $(OBJS)
	@mkdir -p $(EXE_DIR)
	$(CC) $(OBJS) -o $(TARGET) $(LIBS)

# Compilation
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(PROG_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(GLAD_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean: 
	rm -rf $(TARGET)