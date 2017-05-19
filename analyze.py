
import numpy as np 
import cPickle as pickle 

actions = ['put_unsorted', 'put_sorted', 'get_sorted', 'update_sorted', 'update_unsorted', 'get_unsorted', 'throughput_unsorted_303030', 'throughput_sorted_303030'] 
data_sizes = ['1000', '10000', '100000']
buffer_sizes = ['100', '1000', '10000', '100000']  

stats = {} 
ids = [] 

#########
## Specify the index of the action that you want to test 
#########
# a = -1
means = np.zeros([len(data_sizes), len(buffer_sizes)]) 
stds =  np.zeros([len(data_sizes), len(buffer_sizes)]) 

for d in range(len(data_sizes)):
    bs_means =[]
    bs_stds =[]
    for b in range(len(buffer_sizes)): 
        id = '%s_%s_%s'%(actions[a], data_sizes[d], buffer_sizes[b])
        ids.append(id)
        f = open('data/%s/%s.txt'%(actions[a], id), 'r')
        data = f.readline().split(',')[:-1] 
        
        for i in range(len(data)): 
            data[i] = float(data[i])
            
            data = np.array(data)
            mean = np.average(data)
            std = np.std(data)
            means[d,b] = mean 
            stds[d,b] = std 
            stats[id] = {'mean': mean, 'std': std, 'data': data} 
            print id, mean 
 
pickle.dump(stats, open('data/processed/%s_stats.p'%(actions[a]),'w')) 
pickle.dump(means, open('data/processed/%s_means.p'%(actions[a]),'w')) 
pickle.dump(stds, open('data/processed/%s_stds.p'%(actions[a]),'w')) 
print stats.keys() 
print means 
print stds 

