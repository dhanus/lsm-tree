#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

typedef int keyType;
typedef int valType;

typedef struct _node{
  keyType key;
  valType val;
} node;

typedef struct _lsm{
  size_t block_size; // This is the number of nodes each block can hold.
  int k; // The LSM tree grows in dimension k.
  int node_size;
  size_t next_empty;
  node *block;
  char* disk1;
  bool sorted;
} lsm;

typedef struct _nodei{
  node *node;
  int index;
} nodei;

int file_exist(char *fname){
  return ( access( fname, F_OK ) != -1 );
}

lsm* init_new_lsm(){
  lsm* tree;
  tree = malloc(sizeof(lsm));
  if(!tree){
    perror("init_lsm: block is null \n");
    return NULL;
  }
  tree->block_size = 10; 
  tree->k = 2; 
  tree->next_empty = 0; 
  tree->node_size = sizeof(node);
  tree->block = malloc(tree->block_size*tree->node_size); 
  if(!tree->block){
    perror("init_lsm: block is null \n");
    return NULL;
  }
  tree->disk1 = "disk_storage.txt";
  tree->sorted = true;
  printf("init_lsm: initialized lsm \n");
  return tree;
}


void destruct_lsm(lsm* tree){
  free(tree->block);
  free(tree);
}


size_t get_file_size(FILE *f){
  errno=0;
  if (fseek(f, 0, SEEK_END)<0) // seek to end of file
  {
	printf("Error: %d\n",errno);  
    	perror("fseek \n");
  } 
 size_t s = ftell(f); // get current file pointer
  if (fseek(f, 0, SEEK_SET)<0) // seek to end of file
  {
	printf("Error: %d\n",errno);  
    	perror("fseek \n");
  } 
  return s; 
}

void merge(node *whole, node *left,int left_size,node *right,int right_size){
  int l, r, i;
  l = 0; r = 0; i = 0;

  while(l < left_size && r < right_size) {
    if(left[l].key  < right[r].key){
      whole[i++] = left[l++];
    } else{
      whole[i++] = right[r++];
    }
  }
  while(l < left_size){
    whole[i++] = left[l++];
  }
  while(r < right_size){
    whole[i++] = right[r++];
  }
}


void merge_sort(node *block, int n){
  /* Given an unsorted char array containing nores , this returns
     a char array sorted by key using merge sort */
  /* Assumption: block will only be sorted when full */
  assert(block != NULL);
  if(n < 2){
    return;
  }
  int mid, i;
  mid = n/2;
  node *left;
  node *right;

  /* create and populate left and right subarrays */
  left = (node*)malloc(mid*sizeof(node));
  right = (node*)malloc((n-mid)*sizeof(node));

  memcpy(left, block, sizeof(node) * mid);
  memcpy(right, &block[mid], sizeof(node) * (n - mid));

  /* sort and merge the two arrays */
  merge_sort(left,mid);  // sort left subarray
  merge_sort(right,n-mid);  // sort right subarray
  merge(block,left,mid,right,n-mid);
  free(left);
  free(right);
}


nodei* search_buffer(const keyType* key, lsm* tree){
  for (int i = 0; i < tree->block_size; i++){
    if (tree->block[i].key == *key){
      nodei* nodei = malloc(sizeof(nodei));
      nodei->node = &tree->block[i];
      nodei->index = i;
      return nodei;
    }
  }  
  return NULL;
}

nodei* search_disk(const keyType* key, lsm* tree){
  printf("opening file\n");
  int r; 
  FILE* f = fopen("disk_storage.txt", "r");
  if(f == NULL){
    perror("search_disk: open 1: \n");
    return NULL;
  }
  node *file_data;
  size_t num_elements;
  r = fread(&num_elements, sizeof(size_t), 1, f);
  if(r == 0){ 
    if(ferror(f)){
      perror("ferror\n");
    }
    else if(feof(f)){
      perror("EOF found\n");
    }
  }
  file_data = malloc(sizeof(node)*num_elements);
  r = fread(file_data, sizeof(node), num_elements, f);
  if(r == 0){ 
    if(ferror(f)){
      perror("ferror\n");
    }
    else if(feof(f)){
      perror("EOF found\n");
    }
  }
  for(int i = 0; i < sizeof(file_data); i++){
    if (file_data[i].key == *key){
      nodei* nodei = malloc(sizeof(nodei));
      nodei->node = &file_data[i];
      nodei->index = i;
      return nodei;
    }
  }
  if(fclose(f)){
    perror("search_disk: fclose: ");
  }
  return NULL; 
}

node* get(const keyType* key, lsm* tree){
  // search the buffer for this item
  nodei* ni = search_buffer(key, tree);
  if(ni != NULL){
    return ni->node;
  } else{
    // search through the file on disk for this item
    ni = search_disk(key, tree);
  }
  // If it does not find the given key, it will return NULL
  return ni->node;
}

int write_to_disk(lsm* tree){
  node *complete_data;
  node *file_data;
  size_t num_elements;
  int r;
  //sort the buffer 
  if(tree->sorted){
    merge_sort(tree->block, tree->next_empty);
  } 
  struct stat s; 
  int file_doesnt_exist = stat(tree->disk1, &s); 
  if(!file_doesnt_exist){
    FILE* fr  = fopen(tree->disk1, "r");
    // read number of elements 
    r = fread(&num_elements, sizeof(size_t), 1, fr);
    if(r == 0){ 
      if(ferror(fr)){
	perror("put: fread 1: ferror\n");
      }
      else if(feof(fr)){
	perror("put: fread 1: EOF found\n");
      }
    }
    // allocate memory for nodes on disk
    file_data = malloc(sizeof(node)*num_elements);
    assert(file_data);
    // read nodes on disk into memory
    r = fread(file_data, sizeof(node), num_elements, fr);
    if(r == 0){ 
      if(ferror(fr)){
	perror("put fread 2: ferror\n");
      }
	else if(feof(fr)){
	  perror("put fread 2: EOF found\n");
	}
    }
    if(fclose(fr)){
      perror("put: close 2: \n");
    }
    // merge the sorted buffer and the sorted disk contents
    complete_data = malloc(sizeof(node)*(num_elements+tree->next_empty));
    merge(complete_data, file_data, num_elements, tree->block,tree->next_empty);
    free(file_data);
  }
  FILE* fw  = fopen(tree->disk1, "w");
  if(complete_data == NULL){
    complete_data = tree->block;
    num_elements = tree->next_empty;
  }
  // seek to the start of the file & write # of elements
  if(fseek(fw, 0, SEEK_SET)){
    perror("put: fseek 4: \n");
  }
  if(!fwrite(&num_elements, sizeof(num_elements),1, fw)){
    perror("put: fwrite 4: \n");
  }
  // seek to the first space after the number of elements
  if(fseek(fw, sizeof(num_elements), SEEK_SET)){
    perror("put: fseek 5: \n");
  }
  if(!fwrite(complete_data,  sizeof(node),(num_elements+tree->next_empty), fw)){
    perror("put: fwrite 5: \n");
    }
  // reset next_empty to 0
  // Question: Do I still want to do this if I'm writing a partial buffer?
  tree->next_empty = 0;
  if(fclose(fw)){
    perror("put: close 2: \n");
  }
  free(complete_data);
  return 0; 
}


int put(const keyType* key, const valType* val, lsm* tree){
  int r; 
  if(tree->next_empty == tree->block_size){
    // buffer is full and must be written to disk
    r = write_to_disk(tree); 
  }
  node n;
  n.key = *key;
  n.val = *val;
  tree->block[tree->next_empty] = n;
  tree->next_empty += 1;
  return 0;
}


int update(const keyType* key, const valType* val, lsm* tree){
  /* search buffer, search disk, update value  */
  printf("updating things\n");
  nodei* ni = search_buffer(key, tree);
  if(ni != NULL){
    node n;
    n.key = *key;
    n.val = *val;
    tree->block[ni->index] = n;
  } else {
    ni = search_disk(key, tree);
    if(ni != NULL){
      int i = ni->index;
      node *file_data;
      size_t num_elements = 0;
      int r;
      FILE* fr = fopen(tree->disk1, "r");
      r = fread(&num_elements, sizeof(size_t), 1, fr);
      if(r == 0){ 
	if(ferror(fr)){
	  perror("ferror\n");
	}
	else if(feof(fr)){
	  perror("EOF found\n");
	}
      }
      file_data = malloc(sizeof(node)*num_elements);
      assert(file_data);
      r = fread(file_data, sizeof(node), num_elements, fr);
      if(r == 0){ 
	if(ferror(fr)){
	  perror("ferror\n");
	}
	else if(feof(fr)){
	  perror("EOF found\n");
	}
      }
      file_data[i].key = *key;
      file_data[i].val = *val;
      if(fclose(fr)){
	perror("update: flcose 1: \n");
      }
      // seek to the first space after the number of elements
      FILE* fw = fopen(tree->disk1, "w");
      if(fw == NULL){
	perror("update: fopen 2: \n");
      }
      if(fseek(fw, sizeof(node)*(i+1), SEEK_SET)){
	perror("update: fseek \n");
      }
      if(!fwrite(file_data,  sizeof(node),num_elements, fw)){
	perror("update: fwrite: \n");
      }
      // reset next_empty to 0
      // Question: Do I still want to do this if I'm writing a partial buffer?
      tree->next_empty = 0;
      if(fclose(fw)){
	perror("update: flcose 2: \n");
      }
      free(file_data);
    }
  }
  printf("update finished\n");
  return 0;
}

void print_buffer_data(lsm* tree){
  printf("printing buffer data\n");
  for(int i = 0; i < tree->next_empty; i++){
    printf("key %i \n",tree->block[i].key);
    printf("value %i\n",tree->block[i].val);
  }
}

void print_disk_data(lsm* tree){
  printf("printing disk data\n");
  FILE* f = fopen(tree->disk1, "r"); 
  node *file_data;
  size_t num_elements = 0;
  int r;
  r = fread(&num_elements, sizeof(size_t), 1, f);
  if(r == 0){ 
    if(ferror(f)){
	perror("ferror\n");
    }
    else if(feof(f)){
      perror("EOF found\n");
    }
  }
  file_data = malloc(sizeof(node)*num_elements);
  if(file_data == NULL){
    perror("print_disk_data: unsuccessful allocation \n");
  }
  r = fread(file_data, sizeof(node), num_elements, f);
  if(r == 0){ 
    if(ferror(f)){
      perror("ferror\n");
    }
    else if(feof(f)){
      perror("EOF found\n");
    }
  }
  for(int i = 0; i < sizeof(file_data); i++){
    printf("key %d \n",file_data[i].key);
    printf("value %d\n",file_data[i].val);
  }
}

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


int test_get(lsm* tree){
  printf("start get test_data\n");
  srand(0);
  for (int i = 0; i < 10; i++){
    keyType test_k;
    node* n;
    printf("get node\n");
    n =  get(&test_k, tree);
    assert(n);
    printf("got node. about to assert.\n"); 
    printf("val is %d\n", n->val); 
    assert(n->val == (valType)test_k);
    }
  printf("successfully tested get\n");
  return 0; 
}


int test_put(lsm* tree, int data_size){
  /* QUESTION: Is there a smart way to ensure that the keys and values are unique? */
  assert(tree);
  srand(0);
  int r;
  printf("start: create_test_data\n");
  for(int i = 0; i < data_size; i++){
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

int test_update(lsm* tree){
  printf("testing update\n");
  keyType k;
  valType v;
  k = (keyType)rand();
  v = (valType)rand();
  int r = update(&k, &v, tree);
  printf("tested update\n");
  return r;
}

int test_throughput(lsm* tree){
  printf("testing throughtput\n");
  srand(0); 
  int rand_val = rand() % 99;
  if(rand_val <= 33){
    keyType k;
    valType v;
    k = (keyType)rand();
    v = (valType)rand();
    put(&k,&v, tree);
  }else if(rand_val > 33 && rand_val <= 66){
    keyType k;
    valType v;
    k = (keyType)rand();
    v = (valType)rand();
    update(&k, &v, tree);
  } else {
    keyType k;
    k = (keyType)rand();
    get(&k, tree);
  }
  printf("testedthroughtput\n");
  return 0; 
}


int main() {
  int r;
  int data_size = 15;
  clock_t start, end;
  lsm* tree;
  start = clock();
  tree = init_new_lsm();
  r = test_put(tree, data_size);
  test_print_tree(tree);
  //r = test_get(tree);
  //r = test_update(tree);
  //r = test_throughput(tree);
  end = clock();
  printf("%ldms\n", end-start);
  return r;
}

// TODO: things to test
// update to read ratios
// start uniformly distributed then try adding skey
// read-write ratio to throughput
// number of duplicates
// have read only / write only benchmarks

// write a single function per test
/* on exit, write partial buffer to disk on file and meta data */
