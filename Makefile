run: 
	gcc -g lsm.c -o lsm
	./lsm

clean: 
	rm lsm 
	rm *~
	rm -rf *.dSYM
