CC=clang
CFLAGS=-Wall -g


BINS=leakcount leaktest memory_shim.so


all: $(BINS)

run: leakcount
	./leakcount

load: memory_shim.so leakcount
	LD_PRELOAD=./memory_shim.so ./leaktest

mac_load:
	DYLD_INSERT_LIBRARIES=./memory_shim.so DYLD_FORCE_FLAT_NAMESPACE=1 ./leaktest

memory_shim.so: memory_shim.c
	$(CC) $(CFLAGS) -shared -fPIC memory_shim.c -o memory_shim.so -ldl

%: %.c
	$(CC) $(CFLAGS) -o $@ $<


clean:
	rm $(BINS)
