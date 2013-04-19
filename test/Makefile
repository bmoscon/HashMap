# Makefile for hash map test program
# 
# March 2013 - Bryant Moscon


hash_map_test: hash_map_test.o hash_map.o
	gcc -o hash_map_test -g -Wall hash_map_test.o hash_map.o

hash_map_test.o: hash_map_test.c
	gcc -c -g -Wall hash_map_test.c

hash_map.o: ../src/hash_map.c ../src/hash_map.h
	gcc -c -Wall -g ../src/hash_map.c

clean:
	rm hash_map_test *.o
