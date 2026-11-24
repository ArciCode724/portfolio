import numpy as np
import random

def genCornerData():
    n = 7
    x = np.linspace(-5, 5, n)
    y = np.linspace(-5, 5, n)
    xv, yv = np.meshgrid(x, y)
    xv = xv.ravel()
    yv = yv.ravel()
    points = np.stack([xv,yv], axis=1)

    # mask_labers = np.zeros(n*n)

    # xor
    mask_labers = (points[:,0] < 0) * (points[:,1] < 0) + (points[:,0] > 0) * (points[:,1] > 0)
    
    # corner
    # mask_labers = points < 0
    # mask_labers = np.all(mask_labers, axis=1)

    # circel
    # mask_del = ((points[:,0]**2 + points[:,1]**2) > 3**2) * ((points[:,0]**2 + points[:,1]**2) < 5**2)
    # points = np.delete(points, mask_del, axis=0)
    # mask_labers = (points[:,0]**2 + points[:,1]**2) < 4**2
    # mask_labers[16] = 0

    # side
    # mask_labers = points[:,0] < 0
    # mask_labers[:,:] = 0

    # mask_labers[] = 0

    labers = np.zeros(np.shape(mask_labers))
    labers[mask_labers] = 1

    # labers[:] = 0

    # labers[0] = 1
    # labers[7] = 1
    # labers[14] = 1
    # labers[15] = 1
    # labers[10] = 1
    # labers[5] = 1
     
    return points, labers
