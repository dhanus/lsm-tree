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

int test_get(lsm* tree, int data_size){
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

int test_put(lsm* tree, int data_size){
  /* QUESTION: Is there a smart way to ensure that the keys and values are unique? */
  assert(tree);
  srand(0);
  int r;
  printf("start: create_test_data\n");
  for(int i = data_size; i >= 0; i--){
    keyType k;
    valType v;
    k = (keyType)i;
    v = (valType)i;
    r = put(&k,&v,tree);
    assert(r==0);
  }
  printf("test data created \n");
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
    k = (keyType)rand() %(i-1);
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
  int r;
  int data_size = 25;
  clock_t start, end;
  lsm* tree;
  start = clock();
  tree = init_new_lsm();
  r = test_put(tree, data_size);
  test_print_tree(tree);
  //r = test_delete(tree, data_size); 
  //test_print_tree(tree);
  //r = test_get(tree, data_size);
  r = test_update(tree, data_size);
  /*   r = test_throughput(tree, data_size);  */
  end = clock();
  printf("%ldms\n", end-start);
  return r;
}
