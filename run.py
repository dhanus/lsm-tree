from subprocess import call

buffer_sizes = [100, 1000, 10000, 100000]
data_sizes = [1000, 10000, 100000, 1000000, 10000000] 

bs = str(buffer_sizes[0])
ds = str(data_sizes[0])

f = open("put_%s_%s.txt"%(ds, bs), "w")
for i in range(10):
    call(["./lsm", ds, bs], stdout=f)

