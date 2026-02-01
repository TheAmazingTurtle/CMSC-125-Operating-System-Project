CC = gcc
CFLAGS = -Wall -Wextra
TARGET = mysh
SRC = mysh.c

# CFLAGS enables common warning for better code quality

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) *.o