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


int test_put(lsm* tree, int data_size, int buffer_size, bool sorted){
  srand(0);
  int r;
  clock_t start, end;
  start = clock();
  for(int i = 0; i < data_size; i++){
    keyType k;
    valType v;
    k = (keyType)i;
    v = (valType)rand();
    r = put(&k,&v,tree);
    assert(r==0);
  }
  end = clock();
  //printf("data size: %d, buffer size %d \n", data_size, buffer_size);
  double time_elapsed = (double)end-start/CLOCKS_PER_SEC;
  printf("%f,", time_elapsed);
  return r;
}

int test_delete(lsm* tree, int data_size){
  int r = 0; 
  keyType k;

  clock_t start, end;
  start = clock();

  k = (keyType)((rand() % data_size)+10);
  printf("deleting key: %d \n", k);
  r = delete(&k, tree);

  end = clock();
  double time_elapsed = (double)end-start/CLOCKS_PER_SEC;
  printf("%f,", time_elapsed);
  return r; 
}


int test_update(lsm* tree, int data_size){
  printf("testing update\n");
  keyType k;
  valType v;

  clock_t start, end;
  start = clock();

  k = (keyType)(rand() % data_size-1);
  v = (valType)(rand() % data_size-1);
  int r = update(&k, &v, tree);


  end = clock();
  double time_elapsed = (double)end-start/CLOCKS_PER_SEC;
  printf("%f,", time_elapsed);

  printf("tested update\n");
  return r;
}

int test_throughput(lsm* tree, int data_size, int buffer_size, bool sorted){
  printf("testing throughtput\n");

  clock_t start, end;
  start = clock();
  
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
      k = (keyType)rand()  % (i-1);
      v = (valType)rand();
      update(&k, &v, tree);
    } else {
      keyType k;
      k = (keyType)rand() % (i-1);
      get(k, tree);
    }
  }

  end = clock();
  double time_elapsed = (double)end-start/CLOCKS_PER_SEC;
  printf("%f,", time_elapsed);
  printf("tested throughtput\n");
  return 0; 
}

int main(int argc, char* args[]){

  assert(argc == 3); 
  clock_t start, end;

  int r;
  int data_size =  atoi(args[1]);
  int buffer_size =  atoi(args[2]);
  bool sorted = true;
  lsm *tree;
  tree = init_new_lsm(buffer_size, sorted);  

  ///// TEST PUT - SORTED /////
  r = test_put(tree, data_size,buffer_size, sorted);
  // r = test_throughput(tree, data_size, buffer_size, sorted); 

/*   sorted = false;  */
/*   //r = test_put(data_size,buffer_size, sorted); */
/*   r = test_throughput(data_size, buffer_size, sorted);  */
  destruct_lsm(tree); 
  return r;
}
