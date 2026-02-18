CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
TARGET = mysh

TEST_DIR = tests
TEST_SRC = $(wildcard $(TEST_DIR)/*.c)
TEST_BINS = $(TEST_SRC:.c=)

# CFLAGS enables common warning for better code quality

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET) $(TEST_BINS)

test: $(TARGET) $(TEST_BINS)
	@echo "Running all tests..."
	@for test in $(TEST_BINS); do ./$$test || exit 1; done
	chmod +x tests/test_mysh.sh && tests/test_mysh.sh
	@echo "All tests completed!"

$(TEST_DIR)/%: $(TEST_DIR)/%.c $(filter-out src/mysh.o, $(OBJ))
	$(CC) $(CFLAGS) $< $(filter-out src/mysh.o, $(OBJ)) -o $@	