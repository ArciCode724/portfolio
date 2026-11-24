import numpy as np

a = np.array([[1, 2, 3], [4, 5, 6]])
print(a)
# b = np.array([7, 8])
# print(b[None,:])
print(np.append(a, 0, axis=0))