CC ?= cc
CFLAGS ?= -O0 -g
LDFLAGS ?=

hello: hello.c
	$(CC) $(CFLAGS) hello.c -o hello $(LDFLAGS)

clean:
	rm -f hello *.bc *.profraw *.profdata

.PHONY: clean
