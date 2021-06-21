CC = gcc
CFLAGS = -std=gnu99 -g -Wpedantic -lm

all: FSreport

FSreport.o: FSreport.c
	$(CC) $(CFLAGS) -c FSreport.c -o FSreport.o

FSreport: FSreport.o FSreport.c
	$(CC) $(CFLAGS) -o FSreport FSreport.o

clean:
	rm *.o FSreport