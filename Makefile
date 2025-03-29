3. **Makefile**

```makefile
CC = gcc
CFLAGS = -Wall -Wextra -O2 -I./include
LDFLAGS = -lm

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
DOC_DIR = docs
TEST_DIR = tests
EXAMPLE_DIR = examples

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))
EXAMPLE_SRC = $(wildcard $(EXAMPLE_DIR)/*.c)
EXAMPLE_BINS = $(patsubst $(EXAMPLE_DIR)/%.c,$(BIN_DIR)/%,$(EXAMPLE_SRC))
TEST_SRC = $(wildcard $(TEST_DIR)/*.c)
TEST_BINS = $(patsubst $(TEST_DIR)/%.c,$(BIN_DIR)/test_%,$(TEST_SRC))

.PHONY: all clean examples tests

all: directories examples tests

directories:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/hex_converter: $(OBJ_DIR)/hex_converter.o
	$(CC) $^ $(LDFLAGS) -o $@

examples: $(EXAMPLE_BINS)

$(BIN_DIR)/%: $(EXAMPLE_DIR)/%.c $(filter-out $(OBJ_DIR)/main.o, $(OBJ_FILES))
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

tests: $(TEST_BINS)

$(BIN_DIR)/test_%: $(TEST_DIR)/%.c $(filter-out $(OBJ_DIR)/main.o, $(OBJ_FILES))
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
