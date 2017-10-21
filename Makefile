#
# Makefile for shell.c CS3040 Lab 8
#

all: shell

shell: shell.c
	gcc -Wall -g shell.c -o shell

clean:
	rm -f shell

dist:
	tar -cvzf dist8.tgz Makefile shell.c

