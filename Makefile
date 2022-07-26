all: Objects Shooter

Objects:
	gcc -Wall -Wextra -c Shooter.c -o Shooter.o
	gcc -Wall -Wextra -c FatSystem.c -o FatSystem.o
	gcc -Wall -Wextra -c Ex2System.c -o Ex2System.o

Shooter: FatSystem.o
	gcc Shooter.o FatSystem.o Ex2System.o  -o Shooter -Wall -Wextra


clean:
	rm -f *.o Shooter
