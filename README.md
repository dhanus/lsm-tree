# lsm-tree
This is an implementation of a two-level single-threaded [log structure merge (LSM) tree](https://en.wikipedia.org/wiki/Log-structured_merge-tree). The in-memory data structure is a simple array. The on-disk data structure is also a simple array, which can be sorted using [merge sort](https://en.wikipedia.org/wiki/Merge_sort). 

## File structure 
lsm.c holds the implementation of the LSM tree. 
test.c holds the test functions. 
run.py and analyze.py are auxiliary files used to collect and process the data.  
create_plots.upynb is an ipython notebook which takes in the arrays of data as .p files and plots it using [NumPy](http://www.numpy.org/) and [Matplotlib](https://matplotlib.org/).

## To replicate data collection 
Select the test that you want to run from the main function in test.c 
```bash 
make clean 
make 
```
Select the corresponding test from the run.py file 
```bash 
python run.py 
```
Select the corresponding test from analyze.py file 
```bash 
python analyze.py 
```
## To replicate plotting 
Run the jupyter notebook 
```bash 
jupyter notebook create_plots.ipynb
```
Re-run each of the cells. 
