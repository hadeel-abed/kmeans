# Compiler and Flags
CC = gcc
CFLAGS = -ansi -Wall -Wextra -Werror -pedantic-errors
LDFLAGS = -lm  # Link the math library
OBJFILES = symnmf.o

# Executable name
TARGET = symnmf

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)  # Ensure -lm is after the object files

# Compile object files
symnmf.o: symnmf.c symnmf.h
	$(CC) $(CFLAGS) -c symnmf.c

# Clean up generated files
clean:
	rm -f $(TARGET) $(OBJFILES)

# Phony targets
.PHONY: all clean
