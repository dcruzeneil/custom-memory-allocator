CFLAGS=-Wall -pedantic

my-malloc: my-malloc.c
	gcc -g $(CFLAGS) -rdynamic -shared -fPIC -o my-malloc.so my-malloc.c

cm: my-malloc.c
	gcc $(CFLAGS) -o my-malloc my-malloc.c -lm

test: test-malloc.c
	gcc $(CFLAGS) -o test-malloc test-malloc.c 

.PHONY: clean
clean:
	rm -f my-malloc.so my-malloc test-malloc





















