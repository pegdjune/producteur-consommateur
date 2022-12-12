CC = gcc
CFLAGS = -o
library= -lpthread
n1=
n2=
n3=
SRC1 = producer-consommer.c
exe1 = prod-consum


run: 
	$(CC) $(SRC1) $(CFLAGS) $(exe1) $(library)
	./$(exe1) $(n1) $(n2) $(n3)

clean:
	rm *.o
