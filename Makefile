CC=gcc
CFLAGS=-g

BUILD_DIR := ./build

OBJS = $(BUILD_DIR)/parser.o \
	$(BUILD_DIR)/shell.o \
	$(BUILD_DIR)/builtin.o

all: check tsh

tsh: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o tsh
	
check:
	mkdir -p $(BUILD_DIR)
	
$(BUILD_DIR)/parser.o : parser.c
	$(CC) -c $(CFLAGS) parser.c -o $(BUILD_DIR)/parser.o
	
$(BUILD_DIR)/shell.o: shell.c
	$(CC) -c $(CFLAGS) shell.c -o $(BUILD_DIR)/shell.o
	
$(BUILD_DIR)/builtin.o: builtin.c
	$(CC) -c $(CFLAGS) builtin.c -o $(BUILD_DIR)/builtin.o
	
clean:
	rm -vr ./build
	rm -v tsh
