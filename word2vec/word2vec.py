import numpy as np

class Word2vec:
    def __init__(self, wordList:set[str]):
        self.learnRate = 0.03
        self.rng = np.random.default_rng()
        
        self.wordIndex = {}
        # for i in range(len(wordList)):
        #     self.wordIndex[wordList[i]] = i
        for i, word in enumerate(wordList):
            self.wordIndex[word] = i

        # neuronNum = len(wordList) // 10 + 1
        neuronNum = 200
        self.weights:list[np.ndarray] = [0,0]
        
        # self.weights[0] = self.rng.standard_normal((neuronNum, len(wordList)))
        self.weights[0] = self.rng.uniform(-1, 1,(neuronNum, len(wordList)))
        self.weights[0] /= np.sqrt(neuronNum)
        # self.weights[1] = self.rng.standard_normal((len(wordList), neuronNum))
        self.weights[1] = self.rng.uniform(-1, 1,(len(wordList), neuronNum))
        self.weights[1] /= np.sqrt(neuronNum)

    def train(self, wordBag:list[str], target:str) -> int:
        inIndex = []
        for w in wordBag: 
            i = self.wordIndex.get(w, -1)
            if i == -1: continue
            inIndex.append(i)
        # inIndex = np.array(inIndex)
        if len(inIndex) == 0: return -1

        i = self.wordIndex.get(target, -1)
        if i == -1: return -1
        wordLen = len(self.wordIndex)
        outIndex = self.rng.choice(
            wordLen,
            min(11, wordLen), 
            replace=False
        )
        outIndex = np.concatenate([[i], outIndex[outIndex != i][:10]])

        # oneVec = np.ones(len(inIndex))

        # neuronActivation = self.weights[0][:,inIndex] @ oneVec
        # neuronActivation:np.ndarray = self.weights[0][:,inIndex].sum(axis=1)
        neuronActivation:np.ndarray = self.weights[0][:,inIndex].mean(axis=1)
        anser = self.weights[1][outIndex,:] @ neuronActivation
        # -ln(q(x)) - ln(1-q(x))
        anser = 1 / (1 + np.exp(-anser))
        # loss = np.log(anser[0]) + np.log(1-anser[1:]).sum()
        # loss *= -1

        error = anser.copy()
        error[0] -= 1
        loss = np.abs(error).sum()
        neuronError = self.weights[1][outIndex,:].T @ error / len(inIndex)

        learnRate = self.learnRate
        # self.weights[1][outIndex,:] -= _regL1(self.weights[1][outIndex,:], learnRate*0.001)
        self.weights[1][outIndex,:] -= learnRate * np.outer(error, neuronActivation)
        # self.weights[0][:,inIndex] -= learnRate * np.outer(neuronError, oneVec)
        for i in inIndex: 
            # self.weights[0][:,i] -= _regL1(self.weights[0][:,i], learnRate*0.001)
            self.weights[0][:,i] -= learnRate * neuronError

        return loss

    def pruneNeuron(self):
        importance = np.linalg.norm(self.weights[0], axis=1) * np.linalg.norm(self.weights[1], axis=0)
        importance /= np.mean(importance)
        neuronI = np.argmin(importance)

        if importance[neuronI] > 0.2: return False
        print("neuron")
        print(importance[neuronI])

        self.weights[0] = np.delete(self.weights[0], neuronI, axis=0)
        self.weights[1] = np.delete(self.weights[1], neuronI, axis=1)

        # self._delNeuron(layerI, neuronI)
        return True

    def pruneCopy(self):
        magnitut = np.linalg.norm(self.weights[0], axis=1) + 1e-12
        similar:np.ndarray = self.weights[0] / magnitut[:,None]
        similar = similar @ similar.T
            
        np.fill_diagonal(similar, 0)
            
        #     scalarMatrix =  magnitut[:, None] / magnitut[None, :]
        #     similar[ np.logical_or(scalarMatrix < 1/2, scalarMatrix > 2) ] = 0

        index = np.unravel_index(np.argmax(similar), similar.shape)
        bestFit = similar[index]
        scalar = magnitut[index[0]] / magnitut[index[1]]
                        
        if bestFit < 0.9: return False
        print("merge")
        print(bestFit)
        print(scalar)

        self.weights[0][index[0],:] += self.weights[0][index[1],:]*scalar
        self.weights[0][index[0],:] /= 2

        self.weights[1][:,index[0]] += self.weights[1][:,index[1]]/scalar

        self.weights[0] = np.delete(self.weights[0], index[1], axis=0)
        self.weights[1] = np.delete(self.weights[1], index[1], axis=1)
        
        # print(self.weights[0].shape[0])
        return True

    def showVec(self):
        for w, i in self.wordIndex.items():
            print(w, end=": ")
            print(self.weights[0][:,i])

    def getVec(self, word:str):
        i = self.wordIndex.get(word, -1)
        if i == -1: return None
        return self.weights[0][:,i]

def _regL1(X:np.ndarray, rate:float):
    ret = X.copy()
    ret[X > rate] = rate
    ret[X < -rate] = -rate
    return ret