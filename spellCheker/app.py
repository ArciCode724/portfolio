import tkinter as tk
from kbTree import kbTree
import re

def findAllApearnce(word:str) -> list[tuple[int,int]]:
    start = '1.0'
    retVal:list[tuple[int,int]] = []

    while True:
        start = textBox.search(word, start, stopindex="end")
        if not start: break
        end = textBox.index(f"{start} wordend")
        retVal.append((start, end))
        start = end
    return retVal

def addToWordList(word:str):
    wordFinder.insertWord(word)
    for start, end in findAllApearnce(word):
        textBox.tag_remove("errorWord",start,end)

def delFromWordList(word:str):
    wordFinder.deleteWord(word)
    for start, end in findAllApearnce(word):
        textBox.tag_add("errorWord",start,end)


def fixWord(event:tk.Event):
    index = textBox.index(f"@{event.x},{event.y}")
    startIdx = textBox.index(f"{index} wordstart")
    endIdx = textBox.index(f"{index} wordend")

    word = textBox.get(startIdx, endIdx).strip()
    if word == '': return
    fixMenu = tk.Menu(root, tearoff=0)

    wordList = wordFinder.findClose(word)
    if wordList[0] == word: 
        fixMenu.add_command(label='usuń ze słownika', command=lambda: delFromWordList(word))
    else:
        delMenu = tk.Menu(fixMenu, tearoff=0)
        for w in wordList:
            fixMenu.add_command(label=w, command=lambda a=w: textBox.replace(startIdx, endIdx, a))
            delMenu.add_command(label=w, command=lambda a=w: wordFinder.deleteWord(a))
        fixMenu.add_separator()
        fixMenu.add_command(label='dodaj do słownika', command=lambda: addToWordList(word))
        fixMenu.add_cascade(label='usuń słowo', menu=delMenu)

    fixMenu.post(event.x_root, event.y_root)

def on_key(event:tk.Event):
    if event.keysym in ("Left", "Right", "Up", "Down"): return

    rangeToCheck:list[tuple[int,int]] = []

    # print(event.keysym)
    if event.keysym in ['space', 'comma', 'period']:
        startIdx = textBox.index("insert")
        endIdx = textBox.index("insert wordend")
        rangeToCheck.append((startIdx, endIdx))

        startIdx = textBox.index("insert -2 chars wordstart")
        endIdx = textBox.index("insert -1 chars")
        rangeToCheck.append((startIdx, endIdx))

        startIdx = textBox.index("insert -1 chars")
        endIdx = textBox.index("insert")
        textBox.tag_remove("errorWord",startIdx,endIdx)
    else: 
        if event.keysym != "BackSpace": 
            startIdx = textBox.index("insert -1 chars wordstart")
            endIdx = textBox.index("insert -1 chars wordend")
        else:
            startIdx = textBox.index("insert wordstart")
            endIdx = textBox.index("insert wordend")
        rangeToCheck.append((startIdx, endIdx))

    # print(rangeToCheck)

    for startIdx, endIdx in rangeToCheck:
        
        word = textBox.get(startIdx, endIdx)
        if word in [' ', '.', ',']: 
            textBox.tag_remove("errorWord",startIdx,endIdx)
            continue
        if word == '': continue

        if wordFinder.wordExist(word):
            textBox.tag_remove("errorWord",startIdx,endIdx)
        else:
            textBox.tag_add("errorWord",startIdx,endIdx)

root = tk.Tk()
root.geometry('400x400+100+100')
root.title("spell cheker")

wordFinder = kbTree()
wordFileName = "words.yaml"
wordFinder.load(wordFileName)

textBox = tk.Text(root, font=("Courier", 16))
textBox.pack()
textBox.tag_config("errorWord", underline=True, underlinefg="red")

textBox.bind("<Button-3>", fixWord)
textBox.bind("<KeyRelease>", on_key)

root.mainloop()

wordFinder.save(wordFileName)
