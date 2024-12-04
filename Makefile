all: compile 

compile: mem_horder

mem_horder: mem_horder.c
	gcc -o $@ $^

