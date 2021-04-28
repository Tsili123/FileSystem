# Define the symbols we might want to change:
CC=gcc
OBJECTS1=heap_main.o HeapFile.o# main_example_BF.o
OBJECTS2=hash_main.o HashFile.o# main_example_BF.o
CFLAGS=-g
all:my_hp my_ht
# comment
# (note: the <tab> in the command line is necessary for make to work)
#target: dependency1 dependency2 ...
#<tab> command


#my_bf: $(OBJECTS)
#	$(CC) $(CFLAGS) -o my_bf $(OBJECTS) -lm -lbf 

my_hp: $(OBJECTS1)
	$(CC) $(CFLAGS) -o my_hp $(OBJECTS1) BF_64.a -no-pie

my_ht: $(OBJECTS2)
	$(CC) $(CFLAGS) -o my_ht $(OBJECTS2) BF_64.a -no-pie

heap_main.o: heap_main.c 
	$(CC) -c -o heap_main.o heap_main.c

HeapFile.o: HeapFile.c 
	$(CC) -c -o HeapFile.o HeapFile.c

hash_main.o: hash_main.c 
	$(CC) -c -o hash_main.o hash_main.c

HashFile.o: HashFile.c 
	$(CC) -c -o HashFile.o HashFile.c

#main_example_BF.o: main_example_BF.c
#	$(CC) -c -o main_example_BF.o main_example_BF.c

.PHONY: clean

clean:
	rm -f my_hp my_hp.o
	rm -f my_ht my_ht.o
#	rm -f my_bf my_bf.o
	rm -f heap_main.o
	rm -f HeapFile.o
	rm -f  main_example_BF.o
	rm -f hash_main.o
	rm -f HashFile.o
       
