# _*_Makefile_*_
shellso: main.o shellso.o argv.o auxiliar.o
	gcc -o shellso main.o shellso.o argv.o auxiliar.o

shellso.o: shellso.c shellso.h
	gcc -c shellso.c

argv.o: argv.c argv.h
	gcc -c argv.c

auxiliar.o: auxiliar.c auxiliar.h
	gcc -c auxiliar.c

main.o: main.c shellso.h
	gcc -c main.c

clean:
	rm *.o shellso