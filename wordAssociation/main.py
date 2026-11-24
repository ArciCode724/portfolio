from itertools import combinations
from typing import Iterator
import numpy as np

def sentece2Pair(sentec:str) -> Iterator[tuple[frozenset[str], str]]:
    tokens = sentec.split()
    
    numberOfToken = len(tokens)
    if numberOfToken < 2: return

    windowsSize = 3
    
    for i in range(numberOfToken):
        context = (
            tokens[max(0, i-windowsSize):i] +
            tokens[i+1:min(numberOfToken, i+windowsSize+1)]
        )
        
        yield (frozenset(context), tokens[i])

def countCoOcurance(data:list[tuple[frozenset[str], str]]) -> dict[(str,str), int]:
# contex size 1
    counter = {}
    for contex, target in data:
        for conxSize in range(1,3):
            for conx in combinations(contex, conxSize):
                key = (conx, target)
                i = counter.get(key,0)
                counter[key] = i+1    
        # for conx in contex:
        #     key = (conx, target)
        #     i = counter.get(key,0)
        #     counter[key] = i+1
    return counter

def craeteKeyIntexTranslator(itemList:list) -> tuple[dict , list]:
    retList = []
    retDict = {}
    i = 0
    for item in set(itemList):
        retDict[item] = i
        retList.append(item)
        i += 1
    return (retDict, retList)

def cosSimilarity(assoc:dict[tuple[str], int], wordCount):
    keys = assoc.keys()
    contexTrans = craeteKeyIntexTranslator({x[0] for x in keys})
    targetTrans = craeteKeyIntexTranslator({x[1] for x in keys})
    # print(contexTrans[1])
    # print(targetTrans[1])
    similarity:np.ndarray = np.zeros((len(contexTrans[1]),len(targetTrans[1])))
    for k, v in assoc.items():
        similarity[ contexTrans[0][k[0]] , targetTrans[0][k[1]] ] = v

    similarity /= np.linalg.norm(similarity, axis=0)
    similarity = similarity.T @ similarity

    pairs = np.array(np.tril_indices(similarity.shape[0], k=-1))
    order = np.argsort(similarity[pairs[0], pairs[1]])[::-1]
    pairs = pairs.T[order]

    for p in pairs[:50000]:
        w0 = targetTrans[1][p[0]]
        w1 = targetTrans[1][p[1]]
        # if wordCount[w0] <= 5 or wordCount[w1] <= 5: continue
        # print(p)
        print(w0, end=' - ')
        print(w1, end=" : ")
        print(similarity[p[0], p[1]])

    # print(similarity)

with open("procesText.txt", "r") as f:

    data = []
    for l in f.readlines():
        for pair in sentece2Pair(l):
            data.append(pair)
    
    wordCount = {}
    for k in data:
        k = k[1]
        i = wordCount.get(k,0)
        wordCount[k] = i+1

    # data = [i for i in data if wordCount[i[1]] >= 3]

# print(wordCount)
# print(data)


counterCoOcur = countCoOcurance(data)
# print(counterCoOcur)

# sumAll = sum(counterCoOcur.values())
sumLeft = {} # contex
sumRight = {} # target
for k, v in counterCoOcur.items():
    k0, k1 = k
    i = sumLeft.get(k0,0)
    sumLeft[k0] = i+v
    i = sumRight.get(k1,0)
    sumRight[k1] = i+v

# print(sumLeft)
# print(sumRight)

assoc = {k:i/sumLeft[k[0]] for k, i in counterCoOcur.items()}
print(assoc)
# print()
lift = {k:i/sumLeft[k[0]]/wordCount[k[1]]*len(data) for k, i in counterCoOcur.items()}
print(lift)
# for i in lift.items():
# for i in sorted(lift.items(), key=lambda x: -x[1]):
#     k = i[0]
#     if wordCount[k[0]] < 3 or wordCount[k[1]] < 3: continue
#     print(i)

# cosSimilarity(assoc, wordCount)