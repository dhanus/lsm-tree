all: compile

run: 
	./lsm

compile:
	gcc -ggdb -g -O0 lsm.c -o lsm

clean: 
	rm lsm 
	rm *~
	rm -rf *.dSYM
