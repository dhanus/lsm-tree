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
    v = (valType)i; // usually this should be rand
    r = put(&k,&v,tree);
    assert(r==0);
  }
  end = clock();
  //printf("data size: %d, buffer size %d \n", data_size, buffer_size);
  //double time_elapsed = (double)end-start/CLOCKS_PER_SEC;
  //printf("%f,", time_elapsed);
  return r;
}

int test_delete(lsm* tree, int data_size, int nops){
  int r = 0; 
  clock_t start, end;
  start = clock();
  for(int i = 0; i < nops; i++){
    keyType k;
    k = (keyType)((rand() % data_size)+10);
    printf("deleting key: %d \n", k);
    r = delete(&k, tree);
  }
  end = clock();
  double time_elapsed = (double)end-start/CLOCKS_PER_SEC;
  printf("%f,", time_elapsed);
  
  return r; 
}


int test_get(lsm* tree, int data_size, int nops){
  int r = 0; 
  clock_t start, end;
  start = clock();
  for(int i = 0; i < nops; i++){
    keyType k;
    valType v;
    k = (keyType)(rand() % data_size-1);
    node* n = get(k, tree);
  }
  end = clock();
  double time_elapsed = (double)end-start/CLOCKS_PER_SEC;
  printf("%f,", time_elapsed);
  return r; 
};


int test_update(lsm* tree, int data_size, int nops){
  int r = 0; 
  clock_t start, end;
  start = clock();
  for(int i = 0; i < nops; i++){
    keyType k;
    valType v;
    k = (keyType)(rand() % data_size-1);
    v = (valType)(rand() % data_size-1);
    int r = update(&k, &v, tree);
  }
  end = clock();
  double time_elapsed = (double)end-start/CLOCKS_PER_SEC;
  printf("%f,", time_elapsed);
  return r;
}

int test_throughput(lsm* tree, int data_size, int buffer_size, bool sorted, int nops, float put_prob, float update_prob){
  int r = 0; 
  int ndata = 0; 
  clock_t start, end;
  start = clock();
  for(int i = 0; i < nops; i++){ 
    float rand_val = rand() % 99;
    if(rand_val <= put_prob){
      keyType k;
      valType v;
      k = (keyType)ndata;
      v = (valType)rand();
      put(&k, &v, tree);
      ndata++; 
    }else if(rand_val > put_prob && rand_val <= put_prob+update_prob){
      keyType k;
      valType v;
      k = (keyType)rand()%(ndata-1);
      v = (valType)rand();
      update(&k, &v, tree);
    } else {
      keyType k;
      k = (keyType)rand()%(ndata-1);
      get(k, tree);
    }
  }
  end = clock();
  double time_elapsed = (double)end-start/CLOCKS_PER_SEC;
  printf("%f,", time_elapsed);
  return 0; 
}

int main(int argc, char* args[]){

  assert(argc == 5); 
  clock_t start, end;

  int r;
  int data_size =  atoi(args[1]);
  int buffer_size =  atoi(args[2]);
  int nops = atoi(args[3]);
  bool sorted = false;
  lsm *tree;
  /* TEST PUT */ 
  tree = init_new_lsm(buffer_size, sorted);  
  r = test_put(tree, data_size, buffer_size, sorted);
  /* TEST GET */ 
  //  r = test_get(tree, data_size, nops);
  /* TEST UPDATE */ 
  r = test_update(tree, data_size, nops);
  /* TEST THROUGHPUT */;
  /*   float put_prob = 33.0; */
  /*   float update_prob = 33.0;  */
  /*   r = test_throughput(tree, data_size, buffer_size, sorted, nops, put_prob, update_prob);  */
  destruct_lsm(tree); 
  return r;
}
