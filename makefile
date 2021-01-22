CC=clang
CFLAGS=-Wall -g


BINS=leakcount memory_shim.so sctracer


all: $(BINS)

memory_shim.so: memory_shim.c
	$(CC) $(CFLAGS) -shared -fPIC memory_shim.c -o memory_shim.so -ldl

%: %.c
	$(CC) $(CFLAGS) -o $@ $<


clean:
	rm $(BINS)
