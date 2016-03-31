CC=gcc
CFLAGS=-I -Wall -O1
DEPS = lock.h
OBJ = lock.o main.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

Task4: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)
	
clean:
	$(RM) count *.o *~
