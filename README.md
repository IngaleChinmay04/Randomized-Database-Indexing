# CRUD Database with Skip List Implementation

A fast, memory-efficient CRUD (Create, Read, Update, Delete) database implementation in C using a Skip List data structure for efficient searching and sorting.

## Project Overview

This project implements a command-line CRUD database that:

- Uses a Skip List data structure for O(log n) average search, insert, and delete operations
- Persists data to disk in binary format
- Provides a simple command-line interface for database operations
- Supports creating, reading, updating, and deleting records
- Measures and reports performance metrics for operations

## System Requirements

- Windows, Linux, or macOS
- C compiler (GCC recommended)
- Make (optional, for easier building)

## Getting Started on Windows

### Without Make

If you don't have Make installed on Windows, you can compile the application manually using GCC:

1. Ensure you have GCC installed (check with `gcc --version`)

2. Compile the source files:

   ```
   gcc -Wall -Wextra -g -O2 -c main.c -o main.o
   gcc -Wall -Wextra -g -O2 -c skiplist.c -o skiplist.o
   gcc -Wall -Wextra -g -O2 -c record.c -o record.o
   gcc -Wall -Wextra -g -O2 -c persistence.c -o persistence.o
   gcc -Wall -Wextra -g -O2 main.o skiplist.o record.o persistence.o -o crud_db -lm
   ```

3. Run the application:
   ```
   .\crud_db
   ```

### With Make (Optional)

If you want to use Make on Windows:

1. Install Make through one of these methods:

   - Install [MinGW](https://www.mingw-w64.org/downloads/) which includes Make
   - Install [Chocolatey](https://chocolatey.org/install) and then run `choco install make`
   - Install [MSYS2](https://www.msys2.org/) which provides a Linux-like environment

2. Build the project:

   ```
   make
   ```

3. Run the application:
   ```
   .\crud_db
   ```

## Getting Started on Linux/macOS

1. Make sure you have GCC and Make installed:

   ```
   sudo apt-get install gcc make   # Ubuntu/Debian
   sudo yum install gcc make       # CentOS/RHEL
   brew install gcc make           # macOS with Homebrew
   ```

2. Build the project:

   ```
   make
   ```

3. Run the application:
   ```
   ./crud_db
   ```

## Using the Application

Once running, the application provides these commands:

```
--- C-CRUD SkipList DB ---
Commands:
  add <id> <name> <value>  - Add a new record
  get <id>               - Retrieve a record by ID
  del <id>               - Delete a record by ID
  update <id> <name> <val>- Update record (name/value)
  save [filename]        - Save DB (default: crud_database.bin)
  load [filename]        - Load DB (default: crud_database.bin)
  list                   - Display skip list levels (debug)
  stats                  - Show list size and height
  bulkadd <count>        - Add N random records for testing
  help                   - Show this help message
  quit                   - Exit the application
--------------------------
```

### Examples

Here are some example commands to get started:

```
> add 1 FirstItem 123.45
> add 2 SecondItem 67.89
> get 1
> update 1 UpdatedItem 98.76
> stats
> list
> save mydata.bin
> quit
```

## Project Structure

- `main.c` - Contains the command loop and user interface logic
- `skiplist.h/c` - Skip list data structure implementation
- `record.h/c` - Record data structure and handling functions
- `persistence.h/c` - Database save/load functionality
- `Makefile` - Build configuration

## Performance Characteristics

The Skip List implementation provides:

- Average O(log n) search, insert, and delete operations
- Efficient memory usage compared to tree-based structures
- Fast sequential access for range queries

## Troubleshooting

- **Compilation warnings about %zu format specifier**: Some Windows compilers don't support %zu for size_t. The code uses (unsigned long) casts to address this.
- **Database loading fails**: Make sure the file exists and has correct permissions.
- **Performance issues with large datasets**: Tune the MAX_LEVEL parameter in skiplist.h.

## Contributors

- [Hritesh Maikap](https://github.com/hriteshMaikap)
- [Chinmay Ingale](https://github.com/IngaleChinmay04)
