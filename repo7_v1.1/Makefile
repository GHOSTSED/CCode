src=$(CURDIR)/src
exe=$(CURDIR)/exe
test=$(CURDIR)/test

all : FunctionTest CU_DMailListener_test

.PHONY : all

FunctionTest : $(test)/FunctionTest.c $(src)/DMailListener.c $(src)/DHashTable.c $(src)/DList.c
	gcc -g -Wall -Werror -o $(exe)/FunctionTest $(test)/FunctionTest.c $(src)/DMailListener.c $(src)/DHashTable.c $(src)/DList.c -lpcap

CU_DMailListener_test : $(test)/CU_DMailListener_test.c $(src)/DMailListener.c $(src)/DHashTable.c $(src)/DList.c
	gcc -g -Wall -Werror -o $(exe)/CU_DMailListener_test $(test)/CU_DMailListener_test.c $(src)/DHashTable.c $(src)/DList.c -lcunit -lpcap