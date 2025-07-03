tetris: keyboard.c main.c tetris.c
	gcc keyboard.c main.c tetris.c -w -o tetris

install: tetris
	cp tetris ~/.local/bin/

