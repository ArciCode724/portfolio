import numpy
import math
import time
from math import pi
from math import cos
from math import sin
from numpy.fft import fft
from numpy import round as roundc



numData:int = 10000000
data:list[float] = []

for i in range(numData):
    # data.append( cos(2*pi*i/numData) )
    data.append( cos(2*pi*1*i/numData) + 3*cos(2*pi*2*i/numData) - sin(2*pi*3*i/numData + 0.5) )

start = time.time()

out:numpy.ndarray[numpy.complex128] = fft(data)

end = time.time()
print(end - start)


# for i in data:
#     print(round(i, 6))
# print()
# for i in out:
#     print(roundc(i, 6))