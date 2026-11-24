import re

def showOcurance(lines:list[str], pater:str):
    for l in lines:
        if l.find(pater) == -1: continue
        l = l.replace(pater, f"\033[91m{pater}\033[0m")
        print(l, end="")

def contexImportance(lines:list[str], globalWordCound:dict[str:int], pater:str) -> dict[str, float]:
    windowsSize = 5 # distance from centre
    counter = {}

    for l in lines:
        tokens = l.split()
        numberOfToken = len(tokens)
        indices = [i for i, x in enumerate(tokens) if x == pater]
        for i in indices:
            context = (
                tokens[max(0, i-windowsSize):i] +
                tokens[i+1:min(numberOfToken, i+windowsSize+1)]
            )
            for w in context:
                ii = counter.get(w, 0)
                counter[w] = ii+1

    importance = {k:float(i)/globalWordCound[k] for k,i in counter.items()}
    return importance

def showImportance(importance:dict[str, float]):
    sortedList = list(importance.items())
    sortedList = sorted(sortedList, key=lambda x:x[1], reverse=True)
    for k,i in sortedList[:10]:
        print(k, end=" : ")
        print(i)

with open("procesText.txt", "r") as f:
    wordCount = f.readline()
    wordCount = wordCount.split(',')
    wordCount = [x.split(':') for x in wordCount]
    wordCount = {k:int(i) for k, i in wordCount}
    # print(wordCount)

    lines = f.readlines()
    # showOcurance(lines, "deceive")
    # print()
    # showOcurance(lines, "acknowledge")
    counter1 = contexImportance(lines, wordCount, "gold")
    showImportance(counter1)
    print()
    counter1 = contexImportance(lines, wordCount, "silver")
    showImportance(counter1)
    # counter2 = 