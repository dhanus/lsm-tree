from subprocess import call


actions = ['put_sorted', 'put_unsorted', 'get_sorted', 'get_unsorted', 'update_sorted', 'update_unsorted', 'throughput_unsorted_303030', 'throughput_sorted_303030'] 
buffer_sizes = ['100', '1000', '10000', '100000']
data_sizes = ['1000', '10000', '100000', '1000000', '1000000'] 

## Select the action that you would like to test 
a = actions[5]
num_ops = '100'
# collect times for each data and buffer size 
for ds in data_sizes: 
    for bs in buffer_sizes: 
        for i in range(5):
            f = open("data/%s/%s_%s_%s.txt"%(a,a,ds, bs), "w")
            call(["./lsm", ds, bs, num_ops], stdout=f)

