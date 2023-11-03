CC     = gcc
CFLAGS = -std=c99 -Wall# -Wextra -pedantic -Werror 
INCLUDE= -lm
SRC    = *.c ../../src/processes.c
OBJ    = *.o
BUILDS = $(SRC:.c=.o)
EXEC   = processes



all: build clean_build

build: build_objects
	@$(CC) $(CFLAGS) $(OBJ) -o $(EXEC) $(INCLUDE)
#-o [file name] = output file name (gcc [options] [source files] [object files] -o output file)
#-c [file name] = generate object file [*.o] from file name (gcc -c [options] [source files]) {The -c option says not to run the linker. Then the output consists of object files outputted by the assembler.}

build_objects:
	@$(CC) -c $(SRC) $(CFLAGS) $(INCLUDE)

run: build clean_build
	@./$(EXEC)



ifeq ($(OS),Windows_NT)

#Windows stuff
clean: clean_build
	@-del /F /S /Q $(EXEC) 2>NUL || true

clean_build:
	@-del /F /S /Q $(OBJ) 2>NUL || true

else

#Linux stuff
clean: clean_build
	@-rm -rf $(EXEC) 2>/dev/null || true

clean_build:
	@-rm -rf $(OBJ) 2>/dev/null || true

endif



# SOURCES:
# https://www.rapidtables.com/code/linux/gcc/gcc-c.html
# https://linux.die.net/man/1/gcc
