run: clean 
	gcc lsm.c -o lsm 
	./lsm

clean: 
	rm lsm 
	rm *~