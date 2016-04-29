run: 
	gcc -g lsm.c -o lsm
	./lsm

clean: 
	rm lsm 
	rm *~
	rm *.dSYM
	rm disk_storage.txt