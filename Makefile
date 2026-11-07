CC = clang
CFLAGS = -Wall -Wextra -std=c11

emulator: main.c cpu.c memory.c
	$(CC) $(CFLAGS) main.c cpu.c memory.c -o emulator

run: emulator
	./emulator

clean:
	rm -f emulator