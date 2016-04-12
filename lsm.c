#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


typedef int keyType;
typedef int valType;

typedef struct _node{
  keyType key;
  valType val;
  /* char *children; */
} node;

typedef struct _lsm{
  size_t block_size; /*This is the number of nodes each block can hold.*/
  int k;   /*The LSM tree grows in dimension k.*/
  size_t next_empty;
  size_t node_size;
  node *block;
  FILE *disk_fp;
} lsm;

lsm* initialize_lsm(){
  lsm* tree;
  tree->block_size = 100; 
  tree->k = 2; 
  tree->next_empty = 0; 
  tree->node_size = sizeof(node);
  tree->block = malloc(tree->block_size*tree->node_size); 
  return tree;
}

/* on exit, write partial buffer to disk on file and meta data */


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
  /* QUESTION: How does the void pointer in fread work? */
  node *file_data;
  size_t noe;
  fread(&noe, sizeof(size_t), 1, tree->disk_fp);
  file_data = malloc(sizeof(node)*noe);
  fread(file_data, sizeof(node), noe, tree->disk_fp);

  printf("searching level 2\n");
  for(int i = 0; i < sizeof(file_data); i++){
    if (file_data[i].key == key){
      return &file_data[i];
      }
  }
  /* If it does not find the given key, it will return NULL */
  return NULL;
}

int put(keyType key, valType val, lsm* tree){
  if(tree->next_empty == tree->block_size){
    /* sort the block & write it to the next level */
    /* QUESTION: Implement the sort algorithm */
  } else{
    node n;
    n.key = key;
    n.val = val;
    tree->block[tree->next_empty] = n;
    tree->next_empty += 1;
  }
  return 0;
}


int update(keyType key, valType val, lsm* tree){
  /* search buffer, search disk, update value  */
  node* n = get_node(key, tree);
  assert(n != NULL);
  n->val = val;
  /* re-sort array */
  /* TODO: Implement search */
  return 1;
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

int create_test_data(int data_size){
  /* QUESTION: Is there a smart way to ensure that the keys and values are unique? */
  srand(0);
  int r;
  lsm * l;
  l = initialize_lsm();
  printf("\n");
  for(int i = 0; i < data_size; i++){
    int k = rand();
    int v = rand();
    r = put(k,v,l);
    assert(r==0);
  }
  return r;
}


int main() {
  printf( "I am alive!  Beware.\n" );
  getchar();
  return 0;
}

// TODO: things to test
// update to read ratios
// start uniformly distributed then try adding skey
// read-write ratio to throughput
// number of duplicates
// have read only / write only benchmarks

// write a single function per test
