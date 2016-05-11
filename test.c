#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "lsm.h"
#include "test.h"


void test_print_tree(lsm* tree){
  printf("starting print tree\n");
  struct stat s; 
  if(stat(tree->disk1, &s)){
    perror("print: fstat \n");
  }  
   if(s.st_size == 0 && tree->next_empty != 0){
    printf("data fits in the buffer\n");
    print_buffer_data(tree);
  }
  if(s.st_size > 0 && tree->next_empty == 0){
    printf("data is only on disk\n");
    print_disk_data(tree);
  }
  if(s.st_size >  0 && tree->next_empty != 0){
    printf("data is in buffer & on disk\n");
    print_buffer_data(tree);
    print_disk_data(tree);
  }
  printf("tree printed \n");
}

int test_get(lsm* tree, int data_size, bool sorted){
  for (int i = 0; i < data_size; i++){
    keyType test_k=(keyType)i;
    printf("getting key: %d \n", test_k);
    node* n;
    n =  get(test_k, tree);
    assert(n);
    printf("got key: %d \n", n->key);
    assert(n->key == (valType)test_k);
    }
  return 0; 
}

int test_put(int* data_sizes, int num_data_sizes, int*buffer_sizes, bool sorted){
  srand(0);
  int *max_buffer_size = malloc(sizeof(int));
  int r;
  lsm *tree;
  clock_t start, end;
  // for loop on data sizes 
  for(int d = 0; d < num_data_sizes; d++){
    // for loop on buffer
    int max_buffer_size = data_sizes[d];
    for(int b = 0; buffer_sizes[b] < max_buffer_size; b++){
	tree = init_new_lsm(buffer_sizes[b], sorted);
	start = clock();
	for(int i = data_sizes[d]; i >= 0; i--){
	  keyType k;
	  valType v;
	  k = (keyType)i;
	  v = (valType)rand();
	  r = put(&k,&v,tree);
	  assert(r==0);
	}
	end = clock();
	printf("data size: %d, buffer size %d \n", data_sizes[d],buffer_sizes[b]);
	int time_elapsed = (int)end-start;
	printf("%lu \n", time_elapsed);
	char buf[0x100];
	snprintf(buf , sizeof(buf), "put_%d.txt", data_sizes[d]);   
	FILE *f = fopen(buf, "w");
	fwrite(&time_elapsed, sizeof(int), 1, f);
	fclose(f);
    }
    destruct_lsm(tree); 
  }
  return r;
}

int test_delete(lsm* tree, int data_size){
  int r = 0; 
  keyType k;
  k = (keyType)((rand() % data_size)+10);
  printf("deleting key: %d \n", k);
  r = delete(&k, tree);
  return r; 
}


int test_update(lsm* tree, int data_size){
  printf("testing update\n");
  keyType k;
  valType v;
  k = (keyType)(rand() % data_size-1);
  v = (valType)(rand() % data_size-1);
  int r = update(&k, &v, tree);
  printf("tested update\n");
  return r;
}

int test_throughput(lsm* tree, int data_size){
  printf("testing throughtput\n");
  for(int i = 2; i < data_size+2; i++){
    float rand_val = rand() % 99;
    if(rand_val <= 33.0){
      keyType k;
      valType v;
      k = (keyType)i;
      v = (valType)rand();
      put(&k,&v, tree);
    }else if(rand_val > 33.0 && rand_val <= 66.0){
      keyType k;
      valType v;
    k = (keyType)rand() % (i-1);
    v = (valType)rand();
    update(&k, &v, tree);
    } else {
      keyType k;
      k = (keyType)rand() % (i-1);
      get(k, tree);
    }
  }
  printf("tested throughtput\n");
  return 0; 
}

int main(){
  clock_t start, end;
  printf("testing put\n");
  int r;
  int nsizes = 4;
  int data_sizes[] = {1000, 10000, 100000, 100000};
  int buffer_sizes[] = {100, 1000, 10000};
  ///// TEST PUT - SORTED /////
  bool sorted = true;
  test_put(data_sizes, nsizes,  buffer_sizes, sorted); 
  return r;
}
