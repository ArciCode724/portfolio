import numpy as np
from math import log

def strHash(text:str) -> int:
    retHash = 0
    for s in text:
        retHash *= 131
        retHash += ord(s)
    return retHash & ((1<<32) - 1)
    
def testDirtribution():
    with open("words.txt") as f:
        count = np.zeros(32)
        n = 0
        for l in f.readlines():
            n += 1
            l = l[:-1]
            lHash = strHash(l)
            for i in range(32):
                count[i] += (lHash >> i) & 1
            # print(f'{l}, {strHash(l)}')
        print(count / n)

if __name__ == "__main__":
    np.set_printoptions(precision=2, suppress=True)
    binBits = 16
    m = 1 << binBits
    bins = np.zeros(m, dtype=np.int8)
    # print(bins)
    with open("words.txt") as f:
        n = 0
        for l in f:
            l = l[:-1]
            lHash = strHash(l)
            # lHash += (1<<32)

            binI = lHash & ((1<<binBits) - 1)
            lHash >>= binBits

            c = 1
            while lHash & 1:
                lHash >>= 1
                c += 1
            bins[binI] = max(bins[binI], c)

        # print(bins)
        zeros_slot = (bins==0).sum()
        print(zeros_slot/m)
        if zeros_slot > 0.1*m: 
            estimate = -m * log(zeros_slot / m) 
            # estimate = log(zeros_slot/m) / log((m-1)/m)
        else: 
            harmSum = sum(2.0 ** (-int(i)) for i in bins)
            alpha = 0.7213 / (1 + 1.079/m)
            estimate = alpha * m * m / harmSum
        # print(estimate)
        print(estimate)