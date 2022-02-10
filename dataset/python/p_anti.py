import numpy as np
import openturns as ot
import random

attribute_num = 10
id_num_candidate = 100000000
id_num = 1000000
max_increment = 200
max_threshold = 16000000
anti_file = '../distribute/anti_10_1000000.txt'
output_file = 'p_anti_' + str(attribute_num) + '_' + str(max_threshold) + '.txt'

def read_distribution(file):
    y = []
    with open(file) as f:
        while True:
            line = f.readline()
            if line:
                x = list(map(float, line.split()))
                y.append(x[1:])
            else:
                break
    return np.array(y)


samples_sum = np.random.zipf(1.5, id_num_candidate) # powerlaw distribution of sums
y = read_distribution(anti_file)
print("finish reading distribution")
Total_increments = 0

result = []
id_cnt = 0
for i in range(id_num):
    while samples_sum[id_cnt] > max_threshold and id_cnt < id_num_candidate:
        id_cnt += 1
    if id_cnt == id_num_candidate:
        print('terminate at ' + str(i))
        break
    dist = y[i] / y[i].sum()
    dest = [ int(d * samples_sum[id_cnt]) for d in dist ]
    id_cnt += 1

    while (True):
        item = []
        item.append(i)
        tmp = list(filter(lambda s: s >= max_increment, dest))
        if len(tmp) > 0:
            Total_increments += 1
            for t in range(len(dest)):
                k = random.randint(min(1, dest[t]), min(max_increment, dest[t]))
                dest[t] -= k
                item.append(k)
            result.append(item)
        else:
            Total_increments += 1
            for t in dest:
                item.append(t)
            result.append(item)
            break

random.shuffle(result)
print(Total_increments)

with open(output_file, 'w') as f:
    for i, r in enumerate(result):
        f.write(str(r[0]))
        for attr in r[1:]:
            f.write(' ' + str(attr))
        f.write('\n')
