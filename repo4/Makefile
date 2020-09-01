src=$(CURDIR)/src
exe=$(CURDIR)/exe
test=$(CURDIR)/test

all : CU_DMemory_test Algorithm_Compare DMemory_Function_test

.PHONY : all

CU_DMemory_test : $(test)/CU_DMemory_test.c $(src)/DMemory.c
	gcc -g -Wall -Werror -o $(exe)/CU_DMemory_test $(test)/CU_DMemory_test.c -lcunit

Algorithm_Compare : $(test)/Algorithm_Compare.c $(src)/DMemory.c
	gcc -g -Wall -Werror -o $(exe)/Algorithm_Compare $(src)/DMemory.c $(test)/Algorithm_Compare.c

DMemory_Function_test : $(test)/DMemory_Function_test.c $(src)/DMemory.c
	gcc -g -Wall -Werror -o $(exe)/DMemory_Function_test $(test)/DMemory_Function_test.c $(src)/DMemory.c