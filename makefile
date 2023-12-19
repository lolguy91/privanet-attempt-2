build:
	gcc -g -O0 -Werror main.c -o bin/pn

run: build
	bin/pn