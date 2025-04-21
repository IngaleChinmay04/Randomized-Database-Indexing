CC = gcc
CFLAGS = -Wall -Wextra -g -O2 # Use -O0 or -Og for easier debugging if needed
LDFLAGS = -lm # Link math library if needed (not strictly required here, but good practice)

# Source files
SRCS = main.c skiplist.c record.c persistence.c
# Object files derived from source files
OBJS = $(SRCS:.c=.o)
# Executable name
TARGET = crud_db

# Default target: build the executable
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile source files into object files
# Use standard implicit rule for .c to .o compilation:
# %.o: %.c
#	$(CC) $(CFLAGS) -c $< -o $@
# Or be explicit:
main.o: main.c skiplist.h record.h persistence.h
	$(CC) $(CFLAGS) -c main.c -o main.o

skiplist.o: skiplist.c skiplist.h record.h
	$(CC) $(CFLAGS) -c skiplist.c -o skiplist.o

record.o: record.c record.h
	$(CC) $(CFLAGS) -c record.c -o record.o

persistence.o: persistence.c persistence.h skiplist.h record.h
	$(CC) $(CFLAGS) -c persistence.c -o persistence.o

# Clean up object files and the executable
clean:
	rm -f $(OBJS) $(TARGET) $(DB_FILENAME) # Also remove default db file

# Phony targets are not files
.PHONY: all clean
