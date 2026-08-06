# Unix-Shell
A simple Unix Shell written in C, built by following Stephen Brennan's tutorial [*Write a Shell in C*](https://brennan.io/2015/01/16/write-a-shell-in-c/).

## Working
The shell runs a simple loop:
 
1. **Read** – reads a line of input from `stdin` using `getline`.
1. **Parse** – splits the line into tokens (whitespace-separated words) to build an argument list.
1. **Execute** – checks if the first token is a built-in command; if not, it `fork()`s a child process and uses `execvp()` to run the program, while the parent waits for it to finish.


This loop repeats until the user runs `exit` or sends EOF (Ctrl+D).

## Builtin Commands

```bash
> pwd
> ls
> help
> cd
```

## How to Run
 
### Requirements

Requires a C compiler (e.g. `gcc`) on a Unix-like system (Linux/macOS/WSL).

### Compile & Run

```bash
gcc -o shell shell.c
./shell
```