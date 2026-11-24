import numpy as np

dataNum = 10000000
data = np.random.uniform(size=dataNum)
data = np.sort(data)

binNum = 400
bin_edges = np.linspace(0, 1, binNum+1) 
# print(bin_edges[:10])
idx = np.searchsorted(data, bin_edges, side="right")
# print(idx[:10])

minSum = 0
for i in range(binNum):
    if idx[i] == idx[i+1]: continue
    m = np.min(data[idx[i]:idx[i+1]])
    minSum += m-bin_edges[i]
    # print(1/binNum/(m-bin_edges[i]))
minSum /= binNum

print(1/minSum)
print((dataNum - 1/minSum)/dataNum*100)
