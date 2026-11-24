import numpy as np
import random

class neuralNet:
    def __init__(self, layerSize:list[int]):
        if(len(layerSize) < 2): raise ValueError("trzeba podać conajmnej 2 warstwy")
        self.regRate = 0.001
        self.weightList:list[_adamMatrix] = []
        self.biasList:list[_adamMatrix] = []
        self.gateList:list[_adamMatrix] = []
        self.update_div = 0

        for i in range(len(layerSize)-1):
            newWeight = _heWeights(layerSize[i+1], layerSize[i])
            self.weightList.append(_adamMatrix(newWeight))

            newBias = np.zeros(layerSize[i+1])
            self.biasList.append(_adamMatrix(newBias))

            self.gateList.append(_adamMatrix(0.9*np.ones(layerSize[i+1])))

    def predict(self, input:np.ndarray):
        output = input.copy()
        for i in range(len(self.weightList)):
            output = self.weightList[i].val @ output
            output += self.biasList[i].val
            _leakyRelu(output, self.gateList[i].val)
        return output
    
    def train(self, input:np.ndarray, output:np.ndarray):
        self.update_div += 1
        layerOutput = [input.copy()]
        layerDiff = []
        layerGateDiff = []
        modelOut = input.copy()
        # forword
        for i in range(len(self.weightList)):
            np.clip(modelOut, -100, 100, out=modelOut)
            modelOut = self.weightList[i].val @ modelOut
            modelOut += self.biasList[i].val
            layerDiff.append(_leakyReluDiff(modelOut, self.gateList[i].val)) # df(x,a)/dx
            layerGateDiff.append(_gateDiff(modelOut, self.gateList[i].val)) # df(x,a)/da
            _leakyRelu(modelOut, self.gateList[i].val)
            layerOutput.append(modelOut.copy())
        
        # back
        error:np.ndarray = output - modelOut
        # cost = np.linalg.norm(error)
        cost = np.sum(np.abs(error))
        for i in range(len(self.weightList)-1, -1, -1):
            self.gateList[i].upd += error*layerGateDiff[i]

            error *= layerDiff[i]
            np.clip(error, -16, 16, out=error)

            self.biasList[i].upd += error
            self.weightList[i].upd += np.outer(error, layerOutput[i])

            error = self.weightList[i].val.T @ error

        return cost
        # np.clip(error, -10, 10, out=error)

    def step(self):
        if self.update_div == 0: return

        learnRate = 0.01
        regRate = learnRate*self.regRate
        learnRate /= self.update_div

        for i in range(len(self.weightList)):
            self.weightList[i].val -= _regL1(self.weightList[i].val,regRate)
            self.weightList[i].val -= _regL2(self.weightList[i].val, 0.5*regRate)
            self.biasList[i].val -= _regL1(self.biasList[i].val, regRate)
            self.biasList[i].val -= _regL2(self.biasList[i].val, 0.5*regRate)
            self.gateList[i].val -= _regL1(self.gateList[i].val, 3*regRate)

            self.weightList[i].upd /= self.update_div
            self.biasList[i].upd /= self.update_div
            self.gateList[i].upd /= self.update_div

            self.weightList[i].adamStep(learnRate)
            self.biasList[i].adamStep(learnRate)
            self.gateList[i].adamStep(learnRate)
            np.clip(self.gateList[i].val, 0, 1, out=self.gateList[i].val)

        gradient = 0
        elemNum = 0
        for i in range(len(self.weightList)):
            gradient += np.sum(np.abs(self.weightList[i].upd))
            gradient += np.sum(np.abs(self.biasList[i].upd))
            gradient += np.sum(np.abs(self.gateList[i].upd))

            elemNum += self.weightList[i].val.size
            elemNum += self.biasList[i].val.size
            elemNum += self.gateList[i].val.size

            self.weightList[i].upd *= 0
            self.biasList[i].upd *= 0
            self.gateList[i].upd *= 0
        gradient /= self.update_div*elemNum
        self.update_div = 0

        return gradient

    def brainDexamage(self):
        for i in range(len(self.weightList)):
            self.weightList[i].val *= np.random.rand(*self.weightList[i].val.shape)*0.02 + 0.99
            self.biasList[i].val *= np.random.rand(*self.biasList[i].val.shape)*0.02 + 0.99
            self.gateList[i].val *= np.random.rand(*self.gateList[i].val.shape)*0.02 + 0.99

    def pruneWieghts(self):
        for l in self.weightList:
            l.val[np.abs(l.val) < 0.01] = 0

    def _delNeuron(self, layerI:int, neuronI:int):
        self.biasList[layerI].delete(neuronI)
        self.gateList[layerI].delete(neuronI)
        self.weightList[layerI].delete(neuronI, axis=0)
        self.weightList[layerI+1].delete(neuronI, axis=1)

    def _addNeuron(self, layerI:int):
        self.weightList[layerI].pad(0)
        self.weightList[layerI+1].pad(1)
        self.biasList[layerI].pad()
        self.gateList[layerI].pad()

    def _delLayer(self, layerI:int):
        self.weightList.pop(layerI)
        self.biasList.pop(layerI)
        self.gateList.pop(layerI)

    def pruneNeuron(self):
        layerI = -1
        neuronI = 0
        importance = float("inf")
        for i in range(len(self.weightList)-1):
            thisImportance = np.linalg.norm(self.weightList[i].val, axis=1) * np.linalg.norm(self.weightList[i+1].val, axis=0)
            thisImportance /= np.mean(thisImportance)
            
            testI = np.argmin(thisImportance)
            if(thisImportance[testI] < importance): 
                importance = thisImportance[testI]
                neuronI = testI
                layerI = i

        print("neuron")
        print(importance)
        if importance > 0.1: return False

        # print(layerI)
        # print(neuronI)

        self._delNeuron(layerI, neuronI)
        return True
    
    def pruneCopy(self):
        layerI = 0
        bestFit = 0
        neuronsIndex = 0
        scalar = 0
        a = 0
        for i in range(0, len(self.weightList)-1):
            similar:np.ndarray = np.column_stack((self.weightList[i].val, self.biasList[i].val))
            # similar[similar < 0.01] = 0
            magnitut = np.linalg.norm(similar, axis=1) + 1e-12

            similar /= magnitut[:,None]
            similar = similar @ similar.T
            
            np.fill_diagonal(similar, 0)
            
            scalarMatrix =  magnitut[:, None] / magnitut[None, :]
            similar[ np.logical_or(scalarMatrix < 1/2, scalarMatrix > 2) ] = 0

            gateDif = np.abs(self.gateList[i].val[:,None] - self.gateList[i].val[None, :])
            similar[ gateDif > 0.1 ] = 0

            index = np.unravel_index(np.argmax(similar), similar.shape)
            if(similar[index] > bestFit): 
                layerI = i
                bestFit = similar[index]
                neuronsIndex = index
                scalar = magnitut[index[0]] / magnitut[index[1]]
                # scalar = scalarMatrix[index]
                        
        if bestFit < 0.90: return False
        print("merge")
        print(bestFit)
        print(scalar)

        print(self.gateList[layerI].val[neuronsIndex[0]])
        print(self.gateList[layerI].val[neuronsIndex[1]])
        print(self.weightList[layerI].val[neuronsIndex[0],:])
        print(self.weightList[layerI].val[neuronsIndex[1],:])

        self.weightList[layerI].val[neuronsIndex[0],:] += self.weightList[layerI].val[neuronsIndex[1],:]*scalar
        self.weightList[layerI].val[neuronsIndex[0],:] /= 2
        self.biasList[layerI].val[neuronsIndex[0]] += self.biasList[layerI].val[neuronsIndex[1]]*scalar
        self.biasList[layerI].val[neuronsIndex[0]] /= 2

        # print(self.weightList[layerI][neuronsIndex[0],:])

        self.weightList[layerI+1].val[:,neuronsIndex[0]] += self.weightList[layerI+1].val[:,neuronsIndex[1]]/scalar

        self._delNeuron(layerI, neuronsIndex[1])
        return True
        
    def pruneLayer(self):
        layerI = -1
        for i in range(len(self.gateList)-1):
            if np.all(np.abs(self.gateList[i].val) < 0.1):
                layerI = i
                break
        
        if layerI == -1: return False

        self.biasList[layerI+1].val += self.weightList[layerI+1].val @ self.biasList[layerI].val
        self.weightList[layerI+1] = _adamMatrix(self.weightList[layerI+1].val @ self.weightList[layerI].val)        
        self._delLayer(layerI)

        return True
    
    def doubleSize(self):
        for layerI in range(len(self.biasList)-1):
            for splitI in range(len(self.biasList[layerI].val)):
                self._addNeuron(layerI)

                inW = self.weightList[layerI].val[splitI, :].copy()
                self.weightList[layerI].val[-1,:] = inW
                noise = np.random.randn(*inW.shape)
                noise /= np.linalg.norm(noise) + 1e-12
                noise *= 0.04 * np.linalg.norm(inW)
                self.weightList[layerI].val[splitI,:] += noise
                self.weightList[layerI].val[-1,:] -= noise
                
                self.weightList[layerI+1].val[:,-1] = self.weightList[layerI+1].val[:,splitI].copy()
                alpha = np.random.uniform(0.4, 0.6)
                self.weightList[layerI+1].val[:,splitI] *= alpha
                self.weightList[layerI+1].val[:,-1] *= 1-alpha

                noise = 0.01 * np.random.randn()
                self.biasList[layerI].val[-1] = self.biasList[layerI].val[splitI] 
                self.biasList[layerI].val[splitI] += noise 
                self.biasList[layerI].val[-1] -= noise

                noise = 0.01 * np.random.randn()
                self.gateList[layerI].val[-1] = self.gateList[layerI].val[splitI]
                self.gateList[layerI].val[splitI] += noise 
                self.gateList[layerI].val[-1] -= noise

        for layerI in range(len(self.weightList), 0, -1):
            neuronNum = len(self.biasList[layerI-1].val)
            self.weightList.insert(layerI, _adamMatrix(np.eye(neuronNum) + 0.01 * np.random.randn(neuronNum, neuronNum)))
            self.biasList.insert(layerI, _adamMatrix(0.01 * np.random.randn(neuronNum)))
            self.gateList.insert(layerI, _adamMatrix(0.15*np.ones(neuronNum)))

class _adamMatrix:
    def __init__(self, val:np.ndarray):
        self.val = val
        self.upd = np.zeros(val.shape)
        self._sum = np.zeros(val.shape)
        self._sqsm = np.zeros(val.shape)

    def __len__(self):
        return len(self.val)

    def __str__(self):
        return self.val.__str__()

    # upd trzeba samemu ustawiac
    def adamStep(self, learnRate):
        sumRate = 0.9 # movment
        sqrtRate = 0.99 # RMSProp

        # print(self.upd)

        self._sum = sumRate*self._sum + (1-sumRate)*self.upd
        self._sqsm = sqrtRate*self._sqsm + (1-sqrtRate)*(self.upd**2)
        self.val += learnRate*self._sum / (np.sqrt(self._sqsm) + 0.0001)

    def delete(self, intex:int, axis:int=None):
        self.val = np.delete(self.val, intex, axis)
        self.upd = np.delete(self.upd, intex, axis)
        self._sum = np.delete(self._sum, intex, axis)
        self._sqsm = np.delete(self._sqsm, intex, axis)

    def pad(self, axis:int=0):
        pad_shape = list(self.val.shape)
        pad_shape[axis] = 1
        zeros = np.zeros(pad_shape)

        self.val = np.append(self.val, zeros, axis)
        self.upd = np.append(self.upd, zeros, axis)
        self._sum = np.append(self._sum, zeros, axis)
        self._sqsm = np.append(self._sqsm, zeros, axis)

def _leakyRelu(x_vec:np.ndarray, gate:np.ndarray): 
    mask = x_vec < 0
    x_vec[mask] *= 1-gate[mask]

def _leakyReluDiff(x_vec:np.ndarray, gate:np.ndarray):
    diff = np.ones(x_vec.shape)
    mask = x_vec < 0
    diff[mask] = 1-gate[mask]
    return diff

def _gateDiff(x_vec:np.ndarray, gate:np.ndarray):
    diff = np.zeros(x_vec.shape)
    mask = x_vec < 0
    diff[mask] = -x_vec[mask]
    return diff

def _regL1(X:np.ndarray, rate:float):
    # ret = np.zeros(X.shape)
    ret = X.copy()
    ret[X > rate] = rate
    ret[X < -rate] = -rate
    return ret

def _regL2(X:np.ndarray, rate:float):
    return X*rate

def _heWeights(*arg):
    newWeight = np.random.randn(arg[0], arg[1])
    newWeight *= np.sqrt(2.0 / arg[1])
    return newWeight

if __name__ == "__main__":
    a = neuralNet([2,2])

    # print(a.weightList)
    # print()

    # a.train(np.array([1,0]), np.array([1,0]))
    # a.step()

    # for w in a.weightList:
    #     print(w.val)
    # for w in a.biasList:
    #     print(w.val)
    # for w in a.gateList:
    #     print(w.val)