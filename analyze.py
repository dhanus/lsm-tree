import numpy as np 

actions = ['put'] 
data_sizes = ['1000', '10000', '100000', '1000000', '10000000']
buffer_sizes = ['100', '1000', '10000', '100000']  

stats = {} 

a = actions[0]
ds = data_sizes[0]
bs = buffer_sizes[0]

id = '%s_%s_%s'%(a, ds, bs)
f = open('data/%s.txt'%(id), 'r')
data = f.readline().split(',')[:-1] 

for i in range(len(data)): 
    data[i] = float(data[i])

stats[id] = {'max':max(data), 'min':min(data), 'mean':np.average(data), 'std':np.std(data), 'data': data} 

print stats 
