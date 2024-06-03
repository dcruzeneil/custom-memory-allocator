# Custom Implementation of `malloc` and Associated Functions (`free`, `calloc`, `realloc`) in C

This project features a custom implementation of `malloc` and its associated functions (`free`, `calloc`, `realloc`) in C, built around the `sbrk` system call to reduce overhead. This custom memory allocator preemptively allocates more memory than requested and uses metadata structures to determine if additional system calls are necessary or if existing space can be used.

<i>Note: This code is written for UNIX systems but should run on any POSIX-compliant machine.</i>

### Building the Shared Library
To create the shared library, use the provided `Makefile`:
```sh
make
```

This will generate the `my-malloc.so` shared library. TO test this implementation by overwriting the existing `malloc` definition used by all programs, use the `LD_PRELOAD` trick:
```sh
LD_PRELOAD=./my-malloc.so [COMMAND/PROGRAM TO RUN]
```

##### Example
To run VIM with this custom implementation of `malloc` (assuming the terminal is opened in the same directory as `my-malloc.so`):
```sh
LD_PRELOAD=./my-malloc.so vim 
```

<i>Note: This custom implementation may cause VIM to run slower than usual.</i>
