paser_jason_test: jason_test.o
	gcc -o paser_jason_test  jason_test.c -ljson-c

clean:
	rm paser_jason_test *.o
