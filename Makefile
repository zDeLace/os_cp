CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude

SRC = src/first_fit.c src/buddy.c src/test.c
OBJ = $(SRC:.c=.o)

TARGET = allocator_test

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET)
