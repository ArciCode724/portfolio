import numpy as np

A = np.array([
    [5, 2, 9],
    [1, 8, 3],
    [4, 7, 6]
])

# Get lower triangle indices (including diagonal)
index = np.tril_indices(A.shape[0])

# index = np.array(index)
print(index)
# Sort indices by matrix values
# print(A[index[0],index[1]])
order = np.argsort(A[index[0],index[1]])

# print(type(index))
# index[0,:] = index[0,order]
# index[1,:] = index[1,order]

# print(order)
# print(A[index])
# sorted_rows = rows[order]
# sorted_cols = cols[order]

# Values in sorted order
# sorted_values = A[sorted_rows, sorted_cols]

# print(list(zip(sorted_rows, sorted_cols)))
# print(sorted_values)