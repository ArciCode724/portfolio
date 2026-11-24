import numpy as np

with open("wordVec.txt") as f:
    wordList = []
    vecList = []
    # print(vecList)

    for l in f.readlines():
        word, vec = l.split(":")
        # print(word)
        # print(np.fromstring(vec, sep=' '))
        wordList.append(word)
        vecList.append(np.fromstring(vec, sep=' '))

    vecList = np.array(vecList)
# print(vecList.shape)
# print(wordList[10])
# print(vecList[10])

vecMagnitue = np.linalg.norm(vecList, axis=1)
similarity:np.ndarray = vecList / vecMagnitue[:,None]
similarity = similarity @ similarity.T

# print(np.max(similarity))
pairs = np.array(np.tril_indices(similarity.shape[0], k=-1))
order = np.argsort(similarity[pairs[0], pairs[1]], axis=None)[::-1]
pairs = pairs.T[order]

# print(pairs)

for p in pairs[:100000]:
    # print(p)
    print(wordList[p[0]], end=' - ')
    print(wordList[p[1]], end=" : ")
    print(similarity[p[0], p[1]])
# print()

# for i in np.argsort(vecMagnitue)[:-100:-1]:
#     print(wordList[i], end=" : ")
#     print(vecMagnitue[i])