<<<<<<< HEAD
# Projectshell
this is for upcoming project:
=======
# Myshell
shell
>>>>>>> 2a80981c4bbca987a62d47c955e5629808892859
Custom Shell Project - mysh
===========================

This is a custom shell written in C, supporting built-in commands, command history,
user-defined variables, and execution of external programs.

------------------------------------------------------------
Features
--------

✅ Built-in Commands:
  - pwd       : Show current working directory (internally tracked)
  - cd        : Change directory (relative or absolute paths)
  - set       : Create or update a variable (e.g., set USER=vahe)
  - unset     : Remove a variable
  - echo      : Print arguments, supports variables like $USER
  - history   : Show previous commands (stored in file)
  - help      : Show help info about a built-in command
  - exit      : Exit the shell

✅ External Commands:
  - Any valid Linux command (e.g., ls, cat, gcc) is executed via execvp.

✅ History:
  - Saved to history.txt
  - Limited to 100 recent commands
  - Restored on startup

✅ Variable Handling:
  - Variables are saved in setkey and setvalue files
  - Supports variable expansion in echo (e.g., echo $NAME)

------------------------------------------------------------
File Structure
--------------

- builtin.txt     : List of built-in commands
- setkey          : Stores variable names
- setvalue        : Stores variable values
- history.txt     : Command history across sessions
- mysh (binary)   : Compiled shell executable
- main.c          : Source code

------------------------------------------------------------
Compile & Run
-------------

To compile:

    gcc -o mysh main.c

To run:

    ./mysh

You will see:

    mysh>

------------------------------------------------------------
Example Session
---------------

    mysh> set USER=vahe
    mysh> echo $USER
    vahe
    mysh> cd folder
    mysh> pwd
    ~/folder
    mysh> history
    set USER=vahe
    echo $USER
    cd folder
    pwd

------------------------------------------------------------
Known Limitations
-----------------

- The `cd` command does not change the actual working directory of the process.
- Max 100 history lines are kept.
- No piping, redirection, or background job support.
- No autocomplete or line editing (readline not used).

------------------------------------------------------------
Author
------

Vahe Sahakyan  
GitHub: https://github.com/Vahesahakyann
