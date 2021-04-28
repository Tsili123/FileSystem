# Define the symbols we might want to change:
CC=gcc
OBJECTS1=sht_main.o sht.o HashFile.o
OBJECTS2=hash_main.o HashFile.o
CFLAGS=-g
all:my_ht my_sht
# comment
# (note: the <tab> in the command line is necessary for make to work)
#target: dependency1 dependency2 ...
#<tab> command


#my_bf: $(OBJECTS)
#	$(CC) $(CFLAGS) -o my_bf $(OBJECTS) -lm -lbf 

my_ht: $(OBJECTS2)
	$(CC) $(CFLAGS) -o my_ht $(OBJECTS2) BF_64.a -no-pie

hash_main.o: hash_main.c 
	$(CC) -c -o hash_main.o hash_main.c

HashFile.o: HashFile.c 
	$(CC) -c -o HashFile.o HashFile.c

my_sht: $(OBJECTS1)
	$(CC) $(CFLAGS) -o my_sht $(OBJECTS1) BF_64.a -no-pie

sht_main.o: sht_main.c 
	$(CC) -c -o sht_main.o sht_main.c

sht.o: sht.c 
	$(CC) -c -o sht.o sht.c


.PHONY: clean

clean:
	rm -f my_ht my_ht.o
	rm -f my_sht my_sht.o
	rm -f sht_main.o
	rm -f sht.o
	rm -f hash_main.o
	rm -f HashFile.o
       
