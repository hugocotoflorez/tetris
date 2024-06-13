# OBJS specifies which files to compile as part of the project
OBJS = ./tetris.c ./main.c ./keyboard.c

#CC specifies which compiler we're using
CC = gcc

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
COMPILER_FLAGS = -Wall

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lpthread

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = tty-tetris

LIBNAME = libttytetris.a

#This is the target that compiles our executable
all: $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

debug: $(OBJS)
	$(CC) -g $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

compile: ./tetris.c ./keyboard.c
	$(CC) -c ./tetris.c ./keyboard.c $(COMPILER_FLAGS) $(LINKER_FLAGS)
	ar rcs $(LIBNAME) *.o
	rm *.o

install: all
	install tty-tetris /usr/local/bin

uninstall:
	rm /usr/local/bin/tty-tetris


clean:
	rm $(OBJ_NAME)

cleanall:
	rm $(OBJ_NAME) $(LIBNAME)


