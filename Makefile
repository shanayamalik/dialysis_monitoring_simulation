all: main

CC = clang
override CFLAGS += -g -Wno-everything -pthread -lm -lSDL2 -lSDL2_ttf -I/nix/store/xdhh9yzkk2vw7dy2xnl8bgp20gdijj4y-SDL2-2.24.2-dev/include/SDL2
# NOTE ABOVE! Also: replit.nix needs pkgs pkgs.SDL2 and pkgs.SDL2_ttf
# If the -I/nix/store/.../include/SDL2 stops being able to find SDL.h, then do:
#    clang -v -E -x c - < /dev/null 2>&1 | tr ' ' '\n' | grep /include/SDL2

SRCS = $(shell find . -name '.ccls-cache' -type d -prune -o -type f -name '*.c' -print)
HEADERS = $(shell find . -name '.ccls-cache' -type d -prune -o -type f -name '*.h' -print)

main: $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) $(SRCS) -o "$@"

main-debug: $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) -O0 $(SRCS) -o "$@"

clean:
	rm -f main main-debug
