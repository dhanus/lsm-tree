
import numpy as np 
import cPickle as pickle 

actions = ['put_sorted'] 
data_sizes = ['1000', '10000', '100000']
buffer_sizes = ['100', '1000', '10000', '100000']  

stats = {} 
ids = [] 

a = actions[0]
# ds = data_sizes[0]
# bs = buffer_sizes[0]
means = np.zeros([len(data_sizes), len(buffer_sizes)]) 
stds =  np.zeros([len(data_sizes), len(buffer_sizes)]) 
for a in range(len(actions)): 
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
 
pickle.dump(stats, open('stats.p','w')) 
pickle.dump(means, open('means.p','w')) 
pickle.dump(stds, open('stds.p','w')) 
print stats.keys() 
print means 
print stds 

