CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic
TARGET = colorize
SRC = colorize.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) -lc

clean:
	rm -f $(TARGET)
