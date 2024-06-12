# TTY-tetris

## Play tetris from tty! (or any terminal emulator)

### How to compile

``` sh
make all
./tty-tetris
```

### default binds

- `w` : Rotate
- `a` : Move left
- `s` : Move right
- `d` : Move down

### edit controls

You can edit the binds before compilation! Just edit main.c and
edit binds by using the bind function.

`bind( key [char], function)`

Bindable functions are listed in tetris.h behind the comment // bindable functions

### how to exit

It is not implemented yet, but pressing Esc and Ctrl-C will terminate the program
