import numpy as np
import matplotlib.pyplot as plt
import genData
import neuralNet as nn
import tendency

def plotAi(ai:nn.neuralNet, border, ax:plt.Axes):
    n = 30
    x = np.linspace(border[0], border[1], n)
    y = np.linspace(border[0], border[1], n)
    xv:np.ndarray
    yv:np.ndarray
    xv, yv = np.meshgrid(x, y)
    points = np.stack([xv.ravel(),yv.ravel()], axis=1)
    
    z = np.ndarray(len(points))
    for i in range(len(points)):
        pred = ai.predict(points[i])
        z[i] = 0.5*(pred[1] - pred[0])+0.5
        if(z[i] < 0): z[i] = 0
        if(z[i] > 1): z[i] = 1
    z = z.reshape([n,n])
    # print(z)
    ax.contourf(x, y, z, levels = 4, alpha=0.4)

ax:plt.Axes
fig, ax = plt.subplots()

[points, label] = genData.genCornerData()
pointAnser = np.zeros([len(points), 2])
pointAnser[label == 0, 0] = 1
pointAnser[label == 1, 1] = 1 

# indexes = np.arange(len(points))
# np.random.shuffle(indexes)
# batchSize = 30
# while len(indexes)%batchSize != 0: batchSize -= 1
# batchNum = len(indexes)//batchSize
# indexes = indexes.reshape([batchNum, batchSize])

# ai = nn.neuralNet([2,32,32,32,32,2])
ai = nn.neuralNet([2,4,4,4,4,2])

np.set_printoptions(precision=2, suppress=True)
# print(ai.weightList)
# print(ai.biasList)

regRateChange = 0.0001
# for i in range(50):
while True:
    patience = 10
    pastError = 2000
    error = 1000
    # while (cost > 0.04 and oldCost - cost > 0.00001) or a > 0:
    while patience > 0:
        if pastError - error < 0.0001: patience -= 1
        # else: patience = 10

        pastError = error
        error = 0
        for b in range(30):
            for i in range(len(points)):
                error += ai.train(points[i], pointAnser[i])
            grad = ai.step()
        error /= 30*len(points)
        print(error)
        # print(grad)

        # ai.regRate = max(regRateChange, ai.regRate-regRateChange/100)

        ax.clear()
        plotAi(ai, [-6,6], ax)
        ax.scatter(points[:,0], points[:,1], c=label)
        fig.show()
        plt.pause(0.01)

    # for i in range(len(points)):
    #     ai.train(points[i], pointAnser[i])
    # for i in range(len(ai.activation_sum)):
    #     ai.activation_sum[i] /= ai.update_div
    #     ai.activation_sqsm[i] /= ai.update_div
    #     ai.activation_sqsm[i] -= ai.activation_sum[i]**2
    # print(ai.activation_sum)
    # print(ai.activation_sqsm)
    # ai.step()

    # print(ai.weightList)
    # print(ai.biasList)
    # print(ai.gateList)
    for w in ai.gateList:
        print(w.val)

    if error > 0.1: 
        # print("grow")
        # ai.doubleSize()
        ai.regRate = regRateChange
    elif ai.pruneLayer(): 
        print("layer")
        ai.regRate -= 4*regRateChange
    elif ai.pruneNeuron(): 
        print("neuron")
        ai.regRate -= 4*regRateChange
    elif ai.pruneCopy(): 
        print("merge")
        ai.regRate -= 4*regRateChange
    else:
        print("reg")
        # print(ai.regRate)
        ai.regRate += regRateChange
        # print(ai.regRate)
    ai.regRate = max(regRateChange, ai.regRate)

    for w in ai.biasList:
        print(len(w.val), end=" ")
    print()
    print(ai.regRate)
    print("next")

# ai.pruneWieghts()

ax.clear()
plotAi(ai, [-6,6], ax)
ax.scatter(points[:,0], points[:,1], c=label)
fig.show()
plt.pause(1000)