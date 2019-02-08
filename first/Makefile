all: first

first: first.c
	gcc -Wall -Werror -fsanitize=address first.c -o first -lm -O3
clean: 
	rm -rf first

