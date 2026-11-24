import numpy as np
import matplotlib.pyplot as plt
import neuralNet as nn
# import neuralNetAdam as nn
import matplotlib.image as mpimg
import tendency

ax:plt.Axes
fig, ax = plt.subplots()

img_array = mpimg.imread("img.png")
# img_array = mpimg.imread("imgBlack.png")
img_array = np.array(img_array)
img_array = img_array[:,:,0]
img_shape = img_array.shape
# print(img_array[0,0])

# x = np.arange(0, img_array.shape[0])
# y = np.arange(0, img_array.shape[1])
x = np.arange(-16, 16)
y = np.arange(-16, 16)
xv, yv = np.meshgrid(x, y)
xv:np.ndarray = xv.ravel()
yv:np.ndarray = yv.ravel()
points = np.stack([xv,yv], axis=1)
label = img_array.ravel()

indexes = np.arange(len(points))
np.random.shuffle(indexes)
batchSize = 64
while len(indexes)%batchSize != 0: batchSize -= 1
batchNum = len(indexes)//batchSize
indexes = indexes.reshape([batchNum, batchSize])

# ai = nn.neuralNet([2,64,64,64,64,64,64,64,64,64,1])
ai = nn.neuralNet([2,40,40,40,40,40,40,40,1])
regRateChange = 0.0001
ai.regRate = regRateChange

errorTreshold = 0.1

a = 10
pastErrorMean = 2000
errorMean = 100
# for t in range(1000):
while 1:
    patience = 5
    # while pastErrorMean - errorMean > 0:
    while patience > 0:
        if pastErrorMean - errorMean < 0.0005: patience -= 1
        # else: patience = 5
        # pastError = error
        # a -= 1
        error = 0
        for b in range(batchNum):
            for i in indexes[b]:
                error += ai.train(points[i], label[i])
            ai.step()
        error /= batchSize*batchNum
        pastErrorMean = errorMean
        errorMean = errorMean*0.95 + error*0.05
        print(errorMean)

        prediction = np.ndarray(label.shape)
        for i in range(len(points)):
            prediction[i] = ai.predict(points[i])[0]

        ax.clear()
        ax.imshow(prediction.reshape(img_shape), cmap="gray")
        fig.show()
        plt.pause(0.01)
    errorMean *= 1.2
    pastErrorMean = errorMean + 10

    prediction = np.ndarray(label.shape)
    for i in range(len(points)):
        prediction[i] = ai.predict(points[i])[0]
    ax.clear()
    ax.imshow(prediction.reshape(img_shape), cmap="gray")
    fig.show()
    plt.pause(0.1)

    if error > errorTreshold:
        # print("grow")
        # ai.doubleSize()
        input()
        ai.regRate = regRateChange
    elif ai.pruneLayer(): 
        print("layer")
    elif ai.pruneNeuron(): 
        print("neuron")
        ai.regRate -= 4*regRateChange
    elif ai.pruneCopy(): 
        print("merge")
        ai.regRate -= 4*regRateChange
    else: 
        ai.regRate += regRateChange
    ai.regRate = max(regRateChange, ai.regRate)


    for w in ai.biasList:
        print(len(w), end=" ")
    print()
    print(ai.regRate)
    print("next")



# np.clip(prediction, 0, 1, out=prediction)
