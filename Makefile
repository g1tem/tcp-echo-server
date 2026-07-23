CC = clang
CFLAGS = -Wall -Wextra -Wpedantic -std=c11

TARGET = server
BUILD_DIR = build

SOURCES = main.c server.c utils.c
OBJECTS = $(SOURCES:%.c=$(BUILD_DIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET)

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean