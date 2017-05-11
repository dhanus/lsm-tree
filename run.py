from subprocess import call


actions = ['put_sorted', 'put_unsorted', 'get_sorted'] 
#buffer_sizes = ['100', '1000', '10000', '100000']
#data_sizes = ['1000', '10000', '100000', '1000000']# '1000000'] 

buffer_sizes =['100']
data_sizes = ['1000']
a = actions[2]
# collect times for each data and buffer size 
for ds in data_sizes: 
    for bs in buffer_sizes: 
        f = open("data/%s/%s_%s_%s.txt"%(a,a,ds, bs), "w")
        for i in range(1):
            call(["./lsm", ds, bs], stdout=f)

