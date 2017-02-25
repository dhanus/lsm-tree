from subprocess import call


actions = ['put_sorted', 'put_unsorted'] 
buffer_sizes = ['100', '1000', '10000', '100000']
data_sizes = ['1000', '10000', '100000', '1000000', '1000000'] 

# collect times for each data and buffer size 
for ds in data_sizes: 
    for bs in buffer_sizes: 
        f = open("data/put_unsorted_%s_%s.txt"%(ds, bs), "w")
        for i in range(50):
            call(["./lsm", ds, bs], stdout=f)

