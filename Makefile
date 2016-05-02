run: 
	gcc -ggdb lsm.c -o lsm
	./lsm

clean: 
	rm lsm 
	rm *~
	rm -rf *.dSYM
