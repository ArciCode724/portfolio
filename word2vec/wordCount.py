import matplotlib.pyplot as plt

with open("Pan Tadeusz.txt", "r", encoding="utf-8") as f:
    text = f.read()

text = text.lower()

# usuanie zbiednych
for znak in [',', '.', ':', ';', '!', '?','"', "'", '(', ')', '\n', '…', '-', "—", '–', '»', '«', '*']:
    text = text.replace(znak, " ")
# print(text)
# tokens = text.split(" ")
# tokens = list(filter(lambda x: x != '', tokens))
tokens = text.split()

for i in range(len(tokens)):
    if tokens[i] == "pan":
        print(f"{tokens[i-1]} {tokens[i]} {tokens[i+1]}")

# tokens = set(filter(lambda x: x[-1] == '.', tokens))
# print(tokens)

counter = {}
for word in tokens:
    i = counter.get(word, 0)
    counter[word] = i+1

print(len(counter))

order_counter = sorted(counter.items(), key=lambda x: x[1], reverse=True)
# order_counter = order_counter[:50]
# labels = [x[0] for x in order_counter]
# values = [x[1] for x in order_counter]
# print(labels)
# print(values)

# plt.bar(labels, values)
# plt.xticks(rotation=45)
# plt.show()

with open("d.txt", "w", encoding="utf-8") as f:
    for s, i in order_counter:
        f.write(s + ": " + str(i) + "\n")

# text = ' '.join(tokens)
# with open("out.txt", "w", encoding="utf-8") as f:
#     f.write(text)