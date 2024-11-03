CC=gcc
LIBS=-lpthread
FONTES=$(wildcard *.c)
OBJETOS=$(FONTES:.c=.o)
mergesort: $(OBJETOS)	
	$(CC) $(OBJETOS) $(LIBS) -o $@

%.o: %.c	
	$(CC) -c $<  -o $@

clean:
	rm -f *.o Prog.exe
