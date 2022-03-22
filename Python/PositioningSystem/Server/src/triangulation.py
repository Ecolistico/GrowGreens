#!/usr/bin/env python3

import numpy as np
from scipy import optimize

def clean_data(pos, dist):
    count = 0
    for i in range(len(dist)):
        if np.isnan(dist[i-count]) or dist[i-count] == 0: 
            pos = np.delete(pos, i-count, 0)
            dist = np.delete(dist, i-count, 0)
            count += 1
    return pos, dist
    
def min_max(pos, dist):
    l = len(pos[0])
    min_pos = pos-dist
    max_pos = pos+dist
    min_values = []
    max_values = []
    average_values = []
    for i in range(l):
        min_values.append(max([(p[i]) for p in min_pos]))
        max_values.append(min([(p[i]) for p in max_pos]))
        average_values.append((min_values[i]+max_values[i])/2)
    return np.array(average_values)

def err_dis(p0, pos, dist):
    dist_inv = 1/(np.sqrt(dist)) # Weight values with the inverse of the dist 
    #return sum([(np.linalg.norm(pos[i]-p0)*dist_inv[i]) for i in range(len(pos))])
    return sum([(abs(np.linalg.norm(pos[i]-p0)-dist[i])) for i in range(len(pos))]) # Things are strange if dist is < 1

def triangulation(pos, dist):
    pos = np.array(pos).reshape(len(dist), 3)
    dist = np.array(dist).reshape(len(dist), 1)
    p, d = clean_data(pos, dist)
    if len(p) < 3: 
        print("ERROR: not enough points")
        return np.array([0,0,0])
    p0 = min_max(p, d)
    res = optimize.minimize(err_dis, p0, args=(p, d), method='Powell')
    #print(res)
    return res.x.round(3)

def triangulation1(pos, dist):
    p, d = clean_data(pos, dist)
    if len(p) < 3:
        print("ERROR: not enough points") 
        return np.array([0,0,0])
    p0 = min_max(p, d)
    res = optimize.least_squares(err_dis, p0, args=(p, d))
    #print(res)
    return res.x.round(3)