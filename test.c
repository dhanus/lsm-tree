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
  /*Given a pointer to a lsm object, prints tree.*/
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
}


int test_put(lsm* tree, int data_size, int buffer_size, bool sorted, bool timing){
  /* Puts `data size` items into the lsm tree object.
     Args:
     tree: pointer to lsm object.
     data_size: int specifying how much data to add.
     buffer_size: int specifying in-memory buffer size.
     sorted: bool if true, data will be sorted on disk.*/
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
  if(timing){
    double time_elapsed = (double)end-start/CLOCKS_PER_SEC;
    printf("%f,", time_elapsed);
  }
  return r;
}

int test_delete(lsm* tree, int data_size, int nops, bool timing){
  /* Deletes a single item in range (0, `data_size`).
     Args:
     tree: pointer to lsm object.
     data_size: int specifying size of data stored.
     nops: specifies number of times to run operation.*/
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
  if(timing){
    double time_elapsed = (double)end-start/CLOCKS_PER_SEC;
    printf("%f,", time_elapsed);
  }
  return r; 
}


int test_get(lsm* tree, int data_size, int nops, bool timing){
  /* Retrieves an item from the LSM in range (0, `data_size`).
     Args:
     tree: pointer to lsm object.
     data_size: int specifying size of data stored.
     nops: specifies number of times to run operation.*/
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
  if(timing){
    double time_elapsed = (double)end-start/CLOCKS_PER_SEC;
    printf("%f,", time_elapsed);
  }
  return r; 
};


int test_update(lsm* tree, int data_size, int nops, bool timing){
  /* Tests the update function.
     Args:
     tree: pointer to lsm object.
     data_size: int specifying size of data stored.
     nops: specifies number of times to run operation.*/
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
  if(timing){
    double time_elapsed = (double)end-start/CLOCKS_PER_SEC;
    printf("%f,", time_elapsed);
  }
  return r;
}

int test_throughput(lsm* tree, int data_size, int buffer_size, bool sorted, int nops, float put_prob, float update_prob, bool timing){
  /*Tests LSM's throughput by trying many operations of different probabilities. 
    Args: 
    tree: pointer to lsm object. 
    data_size: int specifying size of data stored. 
    nops: specifies number of times to run operation.*/
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
  if(timing){
    double time_elapsed = (double)end-start/CLOCKS_PER_SEC;
    printf("%f,", time_elapsed);
  }
  return 0; 
}

int main(int argc, char* args[]){

  assert(argc >= 5); 
  clock_t start, end;

  int r;
  int data_size = atoi(args[1]);
  int buffer_size = atoi(args[2]);
  int nops = atoi(args[3]);
  char testing[4];
  strcpy(testing, args[4]);
  bool sorted = false;
  if(argc == 6){
    sorted = args[5];
  }
  // set probabilities for throughput. 
  float put_prob = 33.0; 
  float update_prob = 33.0; 
  if(argc == 8){
    put_prob = atoi(args[6]);
    update_prob = atoi(args[7]); 
  }
  lsm *tree;
  tree = init_new_lsm(buffer_size, sorted);
  
  if(strcmp(testing, "put") == 0){
    /* TEST PUT */ 
    r = test_put(tree, data_size, buffer_size, sorted, true);
  }
  if(strcmp(testing, "get") == 0){
    /* TEST GET */ 
    r = test_put(tree, data_size, buffer_size, sorted, false);
    r = test_get(tree, data_size, nops, true);
  }
  if(strcmp(testing, "upd") == 0){
  /* TEST UPDATE */
    r = test_put(tree, data_size, buffer_size, sorted, false);
    r = test_update(tree, data_size, nops, true);
  }
  if(strcmp(testing, "thr") == 0){
  /* TEST THROUGHPUT */;
  r = test_put(tree, data_size, buffer_size, sorted, false);
  r = test_throughput(tree, data_size, buffer_size, sorted, nops, put_prob, update_prob, true);
  }
  destruct_lsm(tree); 
  return r;
}
