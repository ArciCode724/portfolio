class Tendency:
    def __init__(self, memLen):
        self.memLen = memLen
        self.queue = [0 for i in range(memLen)]
        self.queueI = 0
        self.sum = 0
        self.wightSum = 0
        self.xSquer = 0
        i = (self.memLen-1)/2
        while(i > 0): 
            self.xSquer += i**2
            i-=1
        self.xSquer *= 2

    def add(self, newPoint):
        toRemove = self.queue[self.queueI]
        self.queue[self.queueI] = newPoint
        self.queueI = (self.queueI+1)%self.memLen

        # self.wightSum -= self.sum
        # self.wightSum += ((self.memLen-1)/2+1)*toRemove
        # self.wightSum += ((self.memLen-1)/2)*newPoint
        # self.sum -= toRemove
        # self.sum += newPoint

        self.sum -= toRemove
        self.wightSum -= self.sum
        self.wightSum += ((self.memLen-1)/2)*(newPoint+toRemove)
        self.sum += newPoint

        # print(self.sum)
        # print(self.wightSum)
        # print(self.queue)
        return self.wightSum/self.xSquer
    
# trend = Tendency(4)

# for i in [1,2,3,2,1,1,1,2,2,4,6]:
#     print(i)
#     a = trend.add(i)
#     print(a)
#     print()