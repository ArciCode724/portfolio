import re

# imputFile = "Pan Tadeusz.txt"
# imputFile = "chat.txt"
imputFile = "../bible.txt"
with open(imputFile, "r", encoding="utf-8") as f:
    text = f.read()

text = text.lower()

text = re.sub(r"[,:\'\"()…\-—–»«*]", " ", text)
text = re.sub(r"\n(?!\n)", " ", text)

sentenc = re.split(r"[;\n.?!]", text)
for i in range(len(sentenc)):
    sentenc[i] = " ".join(sentenc[i].split())

sentenc = list(filter(lambda x: x != '', sentenc))

# counter = {}
# for w in (' '.join(sentenc)).split():
#     i = counter.get(w, 0)
#     counter[w] = i+1

text = '\n'.join(sentenc)
with open("procesText.txt", "w", encoding="utf-8") as f:
    # counterStr = ','.join([k + ':' + str(i) for k,i in counter.items()])
    # f.write(counterStr + '\n')
    f.write(text)