cflags = -g -Wall

aula2:
	gcc $(cflags) -o aula2 aula2.c -lm

clean:
	rm aula2
