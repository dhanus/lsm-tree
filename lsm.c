#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>

typedef int keyType;
typedef int valType;

typedef struct _node{
  keyType key;
  valType val;
  // char *children;
} node;

typedef struct _lsm{
  size_t block_size; /*This is the number of nodes each block can hold.*/
  int k;   /*The LSM tree grows in dimension k.*/
  int node_size;
  size_t next_empty;
  node *block;
  FILE *disk_fp;
  size_t file_size;
} lsm;

lsm* initialize_lsm(){
  lsm* tree;
  tree = malloc(sizeof(lsm));
  if(!tree){
    perror("init_lsm: block is null \n");
    return NULL;
  }
  tree->block_size = 100; 
  tree->k = 2; 
  tree->next_empty = 0; 
  tree->node_size = sizeof(node);
  tree->block = malloc(tree->block_size*tree->node_size); 
  if(!tree->block){
    perror("init_lsm: block is null \n");
    return NULL;
  }
  tree->file_size = 0;
  printf("initialized lsm \n");
  return tree;
}

/* on exit, write partial buffer to disk on file and meta data */


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

  /* create and populate left and right subarrays*/
  left = (node*)malloc(mid*sizeof(node));
  right = (node*)malloc((n-mid)*sizeof(node));
  
  for(i = 0; i < mid; i++){
    left[i] = block[i];
  }
  for(i = mid;i<n;i++){
    right[i-mid] = block[i];
  }

  /* sort and merge the two arrays */
  merge_sort(left,mid);  // sort left subarray
  merge_sort(right,n-mid);  // sort right subarray
  merge(block,left,mid,right,n-mid);
  free(left);
  free(right);
}


node* get_node(keyType key, lsm* tree){
  /* search the buffer for this item */
  printf("searching level 1\n");
  for (int i = 0; i < tree->block_size; i++){
    if (tree->block[i].key == key){
      return &tree->block[i];
    }
  }
  /* search through the file on disk for this item */
  printf("opening file\n");
  tree->disk_fp  = fopen("disk_storage.txt", "rb+");
  if(tree->disk_fp == NULL){
    perror("could not read file\n");
    return NULL;
  }
  printf("opened file\n");
  node *file_data;
  size_t noe;
  printf("about to read file\n");
  fread(&noe, sizeof(size_t), 1, tree->disk_fp); 
  file_data = malloc(sizeof(node)*noe);
  fread(&file_data, sizeof(node), noe, tree->disk_fp);
  for(int i = 0; i < sizeof(file_data); i++){
    if (file_data[i].key == key){
      return &file_data[i];
      }
  }
  /* If it does not find the given key, it will return NULL */
  return NULL;
}

int put(keyType* key, valType* val, lsm* tree){
  if(tree->next_empty == tree->block_size){
    /* sort the block & write it to the next level */
    tree->disk_fp = fopen("disk_storage.txt", "rb");
    // if the file exists, init first byte to be 0
    if(tree->disk_fp == NULL){
      tree->disk_fp = fopen("disk_storage.txt", "rb+");
      merge_sort(tree->block, tree->next_empty);
      size_t noe = tree->next_empty-1;
      fwrite(&noe, sizeof(noe), 1, tree->disk_fp);
      fseek(tree->disk_fp, sizeof(noe), SEEK_SET);
      fwrite(&tree->block,  sizeof(node),(noe+tree->next_empty), tree->disk_fp);
      fclose(tree->disk_fp);
      return 0;
    } else {
    // LATER: if memory is too small, implement external sort
    // define parameter what the available memory is. (hardcode how much memory)
    // sort the buffer
    merge_sort(tree->block, tree->next_empty);
    // read from the file into memory into a sorted array
    // assume that it fits into memory
    // make sure that there is stuff there
    node *file_data;
    size_t noe = 0;
    fseek(tree->disk_fp, 0, SEEK_SET);
    int r;
    r = fread(&noe, sizeof(size_t), 1, tree->disk_fp);
    file_data = malloc(sizeof(node)*noe);
    assert(file_data);
    r = fread(&file_data, sizeof(node), noe, tree->disk_fp);
    // merge the sorted buffer and the sorted disk contents
    node *complete_data = malloc(sizeof(node)*(noe+tree->next_empty));
    merge(complete_data, file_data, noe, tree->block,tree->next_empty);
    // seek to the start of the file & write # of elements
    fseek(tree->disk_fp, 0, SEEK_SET);
    fwrite(&noe, sizeof(noe),1, tree->disk_fp);
    // seek to the first space after the number of elements
    fseek(tree->disk_fp, sizeof(noe), SEEK_SET);
    fwrite(&complete_data,  sizeof(node),(noe+tree->next_empty), tree->disk_fp);
    // reset next_empty to 0
    // Question: Do I still want to do this if I'm writing a partial buffer?
    tree->next_empty = 0;
    fclose(tree->disk_fp);
    free(file_data);
    free(complete_data);
    }
  } else{
    node n;
    n.key = *key;
    n.val = *val;
    tree->block[tree->next_empty] = n;
    tree->next_empty += 1;
  }
  return 0;
}


int update(keyType* key, valType* val, lsm* tree){
  /* search buffer, search disk, update value  */
  node* n = get_node(*key, tree);
  assert(n != NULL);
  n->val = *val;
  /* re-sort array */
  /* QUESTION: What if we updae on disk?
     How to tell which array this node is in? */
  merge_sort(tree->block, tree->next_empty);
  return 1;
}



void test_print_tree(lsm* tree){
  printf("starting print tree/n");
  if(tree->next_empty < tree->block_size){
    printf("data is not larger than the buffer\n");
    for(int i=0; i < tree->next_empty; i++){
      printf("key %i \n",tree->block[i].key);
      printf("value %i\n",tree->block[i].val);
    }
  }
}


int test_get(lsm* tree){
  printf("start get test_data\n");
  srand(0);
  for (int i = 0; i < 10; i++){
    keyType *test_k = malloc(sizeof(keyType));
    valType *test_v = malloc(sizeof(valType));
    node* n;
    printf("get node\n");
    n =  get_node(*test_k, tree);
    assert(n);
    printf("got node. about to assert.\n"); 
    printf("val is %i\n", n->val); 
    printf("test_v is %i \n",(valType)test_v);
    assert(n->val == (valType)test_v);
    }
  return 0; 
}


int test_put(lsm* tree, int data_size){
  /* QUESTION: Is there a smart way to ensure that the keys and values are unique? */
  assert(tree);
  srand(0);
  int r;
  printf("start: create_test_data\n");
  for(int i = 0; i < data_size; i++){
    keyType *k = malloc(sizeof(keyType));
    valType *v = malloc(sizeof(valType));
    //printf("about to define keyTypes %i \n", i);
    *k = (keyType)i;
    *v = (valType)i;
    r = put(k,v,tree);
    assert(r==0);
  }
  printf("test data created \n");
  return r;
}


int main() {
  int r;
  int data_size = 110;
  lsm* tree;
  tree = initialize_lsm();
  r = test_put(tree, data_size);
  // print_tree(tree);
  return r;
}

// TODO: things to test
// update to read ratios
// start uniformly distributed then try adding skey
// read-write ratio to throughput
// number of duplicates
// have read only / write only benchmarks

// write a single function per test
