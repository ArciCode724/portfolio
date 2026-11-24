from random import sample
import json
import heapq
import yaml

# https://en.wikipedia.org/wiki/BK-tree

class kbTree:
    def __init__(self):
        self.nodes:dict[str, dict[int, str]] = {}
        self.root:str = None

    def insertWord(self, word:str):
        if self.root == None:
            self.root = word
            # self.nodes[word] = {}
            return

        parent:str = self.root
        while True:
            wordDist:int = wordDistance(word, parent)
            if wordDist == 0: break

            paths:dict[int, str] = self.nodes.get(parent)
            if paths == None:
                paths = {}
                self.nodes[parent] = paths

            parent = paths.get(wordDist)
            if parent != None: continue

            paths[wordDist] = word
            # self.nodes[word] = {}
            break

    def deleteWord(self, word:str):
        if self.root == None: return

        if self.root == word:
            wordList = self._removeAllNodes(self.root)
            self.root = self._createTree(wordList)
            return

        parent = self.root
        wordDyst = 0
        while True:
            path = self.nodes.get(parent)
            if path == None: return
            wordDyst = wordDistance(parent, word)
            child = path.get(wordDyst)
            if child == None: return
            if child == word: break
            parent = child
        
        if self.nodes.get(word) == None:
            self.nodes[parent].pop(wordDyst)
        else:
            wordList = self._removeAllNodes(word)
            self.nodes[parent][wordDyst] = self._createTree(wordList)

    def wordExist(self, word:str) -> bool:
        if self.root == None: return False
        patent = self.root

        while patent != None:
            testDyst = wordDistance(patent, word)
            if testDyst == 0: return True

            path = self.nodes.get(patent)
            if path == None: return False
            patent = path.get(testDyst)
        return False

    def findClose(self, word:str, k:int=3) -> list[str]:
        if self.root == None: return []
        matchHeap:list[tuple[int, str]] = [(float("inf"), '')]*k
        checkStack = [self.root]

        while checkStack:
            testWord = checkStack.pop()
            testDyst = wordDistance(testWord, word)

            heapq.heappushpop_max(matchHeap, (testDyst, testWord))
            matchScore = matchHeap[0][0]

            path = self.nodes.get(testWord)
            if path == None: continue
            for k, child in path.items():
                if abs(k - testDyst) <= matchScore:
                    checkStack.append(child)

        matchHeap = sorted(matchHeap)
        return [s for v, s in matchHeap]

    # reqursion, tworzy drzewo na bazie słów i zwraca nazwe roota
    def _createTree(self, wordList:list[str]) -> str:
        if len(wordList) == 1: return wordList[0]
        newRoot = _findBestRoot(wordList)

        subList:dict[int, list[str]] = {}
        for w in wordList:
            dyst = wordDistance(newRoot,w)
            l:list[str] = subList.get(dyst)
            if l == None: subList[dyst] = [w]
            else: l.append(w)
        subList.pop(0)

        newPath:dict[int, str] = {}
        for k, v in subList.items():
            newPath[k] = self._createTree(v)
        self.nodes[newRoot] = newPath
        return newRoot

    # zwraca liste wszystkich usuniętych złów z drzewa. bez roota
    def _removeAllNodes(self, root:str) -> list[str]:
        wordList:list[str] = []
        wordToCheck:list[str] = [root]

        while wordToCheck:
            paths = self.nodes.pop(wordToCheck.pop(), None)
            if paths == None: continue
            val = paths.values()
            wordList.extend(val)
            wordToCheck.extend(val)

        return wordList

    def rebuild(self):
        wordList = self._removeAllNodes(self.root)
        wordList.append(self.root)
        self.root = self._createTree(wordList)

    def save(self, fileName:str):
        loaded = {}
        loaded["root"] = self.root
        loaded["nodes"] = self.nodes
        with open(fileName, "w", encoding="utf-8") as f:
            # json.dump(loaded, f, indent=2, ensure_ascii=False)
            yaml.dump(loaded, f, allow_unicode=True)

    def load(self, fileName:str):
        try:
            with open(fileName, "r", encoding="utf-8") as f:
                # loaded:dict = json.load(f)
                loaded:dict = yaml.safe_load(f)
        except FileNotFoundError:
            return

        # print(loaded)
        self.root = loaded["root"]
        self.nodes = loaded["nodes"]
        # for k1, v1 in self.nodes.items():
        #     self.nodes[k1] = {int(k2):v2 for k2,v2 in v1.items()}

def _findBestRoot(wordList:list[str]) -> str:
    rootBach = sample(wordList, min(20, len(wordList)))
    # bestGini = -1 # gini
    bestGini = float("inf") # gini
    bestRoot = rootBach[0]

    for r in rootBach:
        counter = {}
        for w in wordList:
            dyst = wordDistance(r,w)
            i = counter.get(dyst, 0)
            counter[dyst] = i + 1
        counter.pop(0)

        gini = 0.0
        for i in counter.values(): gini += i**2
        # gini = 1 - gini/(len(wordList)-1)**2
        if gini < bestGini:
            bestGini = gini
            bestRoot = r
    return bestRoot        

def wordDistance(word1:str, word2:str) -> int:
    # https://en.wikipedia.org/wiki/Damerau%E2%80%93Levenshtein_distance
    w1Len = len(word1)+1
    w2Len = len(word2)+1

    # [word1][word2]
    subDyst = [[0 for j in range(w2Len)] for i in range(w1Len)]
    for i in range(w1Len): subDyst[i][0] = i
    for i in range(w2Len): subDyst[0][i] = i
    for w1 in range(1,w1Len):
        for w2 in range(1,w2Len):
            thisDyst = float('inf')
            sameChar = word1[w1-1] == word2[w2-1]

            thisDyst = min(thisDyst, subDyst[w1][w2-1]+1) # delet
            thisDyst = min(thisDyst, subDyst[w1-1][w2]+1) # inset

            testDyst = subDyst[w1-1][w2-1]
            if not sameChar: testDyst += 1
            thisDyst = min(thisDyst, testDyst) # replace

            # if w1 >= 2 and w2 >= 2 and word1[w1-1] == word2[w2-2] and word1[w1-2] == word2[w2-1]:
            #     testDyst = subDyst[w1-2][w2-2]
            #     if not sameChar: testDyst += 1
            #     thisDyst = min(thisDyst, testDyst) # swap

            subDyst[w1][w2] = thisDyst
    # print(subDyst)
    return subDyst[-1][-1]

if __name__ == "__main__":
    # wordDistance("ca", 'abc')
    model = kbTree()
    model.insertWord("book")
    model.insertWord("books")
    model.insertWord("boo")
    model.insertWord("boon")
    model.insertWord("cook")
    model.insertWord("cake")
    model.insertWord("cape")
    model.insertWord("cart")
    model.deleteWord("boon")
    print(model.nodes)
    