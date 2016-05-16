
void print_buffer_data(lsm* tree);

void print_disk_data(lsm* tree); 

void test_print_tree(lsm* tree);

int test_get_one(lsm* tree, int data_size);

int test_put_one(lsm* tree, int data_size);

int test_get(int* data_sizes, int num_data_sizes,  int* buffer_sizes, bool sorted);

int test_put(int* data_sizes, int num_data_sizes,  int* buffer_sizes, bool sorted);

int test_delete(lsm* tree, int data_size);

int test_update(lsm* tree, int data_size);

int test_throughput(lsm* tree, int data_size);
