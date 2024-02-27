# choose compiler
CC=gcc
CXX=g++

# directories
SRC_DIR=src
OBJ_DIR=obj
BIN_DIR=bin

# executable name
EXE=$(BIN_DIR)/sorna_test

# All .c files
SRC=$(wildcard $(SRC_DIR)/*.c)

# List object files from source files...
OBJ=$(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Set compiler and linker flags
CPPFLAGS  =-Iinc -Icore -MMD -MP
CFLAGS    =-Wall
LDFLAGS   =
LDLIBS    =

.PHONY: all clean debug
all: $(EXE)

debug: CFLAGS +=-DDEBUG -g
debug: CPPFLAGS += -DDEBUG
debug: $(EXE)

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	$(RM) -rv $(BIN_DIR) $(OBJ_DIR)

-include $(OBJ:.o=.d)