from word2vec import Word2vec
import numpy as np
import random

def sentece2Paris(sentec:str) -> list[tuple[list[str], str]]:
    retVal = []
    tokens = sentec.split()
    
    windowsSize = 5 # distance from centre
    
    numberOfToken = len(tokens)
    if numberOfToken < 2: return []

    for i in range(numberOfToken):
        context = (
            tokens[max(0, i-windowsSize):i] +
            tokens[i+1:min(numberOfToken, i+windowsSize+1)]
        )
        retVal.append( (context, tokens[i]) )

    return retVal

with open("procesText.txt", "r") as f:
    wordDic = f.readline()
    wordDic = wordDic.split(',')
    wordDic = [x.split(':') for x in wordDic]
    wordDic = [x[0] for x in wordDic if int(x[1]) >= 1]
    # print(len(wordDic))

    model = Word2vec(wordDic)

    data = []
    for l in f.readlines():
        data += sentece2Paris(l)

chunkSize = 8000
dataChunks = [data[i:i + chunkSize] for i in range(0, len(data), chunkSize)]
random.shuffle(dataChunks)

# print(data)
# print(len(data))
smoothError = 10.0
pastError = 100
patience = 4
while model.learnRate > 0.009:
# for _ in range(100000):
    # print("reset")
    print(model.weights[0].shape[0])
    for chunk in dataChunks:
        error = 0
        i = 0
        for contex, target in chunk:
            e = model.train(contex, target)
            if e == -1: continue
            error += e
            i += 1
        a = 0.95
        smoothError = error/i*(1-a) + smoothError*a
        # print(error/len(chunk))
        print(smoothError)
        print(model.learnRate)
        
        if abs(pastError - smoothError) < 0.002: patience -= 1
        else: patience = 4

        if patience == 0:
            patience = 4
            smoothError *= 1.2
            # if model.pruneNeuron(): pass #model.learnRate /= 0.9
            # elif model.pruneCopy(): pass #model.learnRate /= 0.9
            # else: model.learnRate *= 0.9
            model.learnRate *= 0.9
            if model.learnRate <= 0.009: break
        
        pastError = smoothError 
        # print()
    # if model.pruneCopy():
    #     smoothError += 1
    #     model.learnRate /= 0.9
    if smoothError < 0: break
    # if smoothError < 1: break

# model.showVec()
print(model.weights[0].var(axis=1))

with open("wordVec.txt", "w") as f:
    # f.write(str(model.weights[0].shape[0]) + "\n")
    for w in wordDic:
        f.write(w + ": ")
        np.savetxt(f, model.getVec(w), fmt="%f", newline=' ')
        f.write('\n')