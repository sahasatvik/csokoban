# CSokoban
The game Sokoban, implemented in C for the terminal.
The goal is to use the player character to push the yellow boxes onto all
of the red target squares. No diagonal movements are allowed!

![sokoban](demo.png)

Compile with `make`, and run using `./csokoban`.
Using `./csokoban -s` to shuffle the levels is recommended!
Execute `./csokoban -h` for the following help message.

```
The game Sokoban, implemented in C for the terminal.

usage: csokoban [-hs] [-l mapfile] [-n mapnumber]
    -h  Show this help message
    -s  Shuffle maps
    -l  Load puzzles from 'mapfile'
    -n  Start from map 'mapnumber'

keys:
    h j k l     Move the player character left/down/up/right
    4 2 8 6     Same, using the numpad keys
          q     Quit
          r     Restart the current map
        > <     Skip to the next/previous map
```

## Level data
As an example, consider `demolevels.txt`.

```
#######
#@   ##
#  $ .#
##  ###
#######

; The * represents a box
; on top of a target square
####
# .#
#  ###
#*@  #
#  $ #
#  ###
####
```

You can load this file using `./csokoban -l demolevels.txt`.

Different maps must be separated by empty lines.
Map lines, containing tile data, must start with either a `#` (wall) or ` `, `-`, `_` (floor).
Use the following key.

| Character     |   Tile                        |
|---------------|-------------------------------|
| ` `/`-`/`_`   | Empty floor                   |
| `#`           | Wall                          |
| `$`           | Box                           |
| `*`/`%`       | Box, already on target square |
| `.`/`x`/`X`   | Target square                 |
| `@`           | Player starting square        |
| `+`/`&`       | Player, on a target square    |

All other lines are ignored, interpreted as comments.

The default levels (`levels.txt`) have been taken from [Deepmind's boxoban
levels](https://github.com/deepmind/boxoban-levels). You can simply download
any of these files and play, as follows.

```
wget https://raw.githubusercontent.com/deepmind/boxoban-levels/master/medium/train/000.txt
./csokoban -l 000.txt
```

