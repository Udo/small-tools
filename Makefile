CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic
TARGET = colorize
SRC = colorize.c
PREFIX = /usr/local

.PHONY: all clean install

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) -lc

clean:
	rm -f $(TARGET)

install: $(TARGET)
	install -Dm755 $(TARGET) $(PREFIX)/bin/$(TARGET)
