all: second

second: second.c
	gcc -Wall -Werror -fsanitize=address second.c -o second -lm -O3
clean: 
	rm -rf second

