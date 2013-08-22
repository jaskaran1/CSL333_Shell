all:
	gcc myshell.c -lreadline -o myshell
	gcc closh.c -lreadline -o closh
