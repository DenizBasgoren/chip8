# CHIP-8 Emulator in C and Raylib

This is a simple emulator of the famous CHIP-8 virtual machine, known for its simple architecture, and many games written for it in 80's.

## How to run

To compile, first install a C compiler and Raylib library. Get some games from [here](https://github.com/kripod/chip8-roms), and then type:

```sh
gcc main.c -lraylib
./a.out theGame.ch8
```

## How to play

The inputs are made using keyboard only. We're assuming the following layout for now:

```
 on keyboard            corresponding values
-------------         ------------------------
  1  2  3  4               0  1  2  3
  Q  W  E  R               4  5  6  7
  A  S  D  F               8  9  A  B
  Z  X  C  V               C  D  E  F

```

## Features and Limitations

- Supports CHIP-8, SuperChip and MegaChip8 formats
- Doesn't support permanent storage
- Doesn't support sounds, prints Buzz! instead
