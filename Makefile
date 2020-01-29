CC=g++
CFLAGS=-std=c++14 -O3 -Wall -lpthread
EXEC=MEHDI
$(EXEC) : nbr_premier.o 
	$(CC) -o $@ nbr_premier.o  $(CFLAGS)
nbr_premier.o:nbr_premier.cpp
	$(CC) -o nbr_premier.o -c nbr_premier.cpp  $(CFLAGS)
clean:
	rm $(EXEC) *.o Test/*.txt	




