# lsm-tree
This is an implementation of a two-level single-threaded [log structure merge (LSM) tree](https://en.wikipedia.org/wiki/Log-structured_merge-tree). The in-memory data structure is a simple array. The on-disk data structure is also a simple array, which can be sorted using [merge sort](https://en.wikipedia.org/wiki/Merge_sort). 


