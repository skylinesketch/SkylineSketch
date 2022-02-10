import numpy as np
from scipy.linalg import eigh, cholesky
from scipy.stats import norm
from pylab import plot, show, axis, subplot, xlabel, ylabel, grid
import random

attribute_num = 10
id_num_candidate = 100000000
id_num = 1000000
max_increment = 200
max_threshold = 16000000

def record_sum(file, i, dest):
    file.write(str(i))
    for d in dest:
        file.write(' ' + str(d))
    file.write('\n')


# f_sum = open("p_uniform_sum.txt", 'w')
samples_sum = np.random.zipf(1.5, id_num_candidate) # powerlaw distribution of sums

result = []
cnt = 0
id_cnt = 0
for i in range(id_num):
    if i % 10000 == 0:
        print(i)
    while samples_sum[id_cnt] > max_threshold and id_cnt < id_num_candidate:
        id_cnt += 1
    if id_cnt == id_num_candidate:
        print('terminate at ' + str(i))
        break
    dist = np.random.uniform(0, 1, attribute_num)
    dist /= dist.sum()
    dest = [ int(d * samples_sum[id_cnt]) for d in dist ]
    id_cnt += 1
    # record_sum(f_sum, i, dest)
    while (True):
        item = []
        item.append(i)
        tmp = list(filter(lambda s: s >= max_increment, dest))
        if len(tmp) > 0:
            cnt += 1
            for t in range(len(dest)):
                k = random.randint(min(1, dest[t]), min(max_increment, dest[t]))
                dest[t] -= k
                item.append(k)
            result.append(item)
        else:
            cnt += 1
            for t in dest:
                item.append(t)
            result.append(item)
            break

# f_sum.close()
random.shuffle(result)
print(cnt)

with open('p_uniform_' + str(attribute_num) + '_' + str(max_threshold) + '.txt', 'w') as f:
    for i, r in enumerate(result):
        f.write(str(r[0]))
        for attr in r[1:]:
            f.write(' ' + str(attr))
        f.write('\n')

