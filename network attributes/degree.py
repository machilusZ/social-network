import numpy as np
import matplotlib.pyplot as plt
from math import log
with open('/Users/feng/PycharmProjects/social_PJ/facebook/0.edges') as f:
    edges = {}
    lines = f.readlines()
    connection = [line.strip().split() for line in lines]
    for edge in connection:
        head = edge[0]
        tail = edge[1]
        try:
            edges[head].append(tail)
        except KeyError:
            edges[head] = [tail]
        try:
            edges[tail].append(head)
        except KeyError:
            edges[tail] = [head]

nodes = list(edges.keys())
nodes.sort()

size = len(nodes)
adjency_mat = np.zeros((size, size))
diagDegree_mat = np.zeros((size, size))


for i in range(size):
    for j in range(size):
        head = nodes[i]
        tail = nodes[j]
        if tail in edges[head]:
            adjency_mat[i, j] = 1

distribution=[]
for i in range(size):
    node = nodes[i]
    degree = len(edges[node])
    diagDegree_mat[i, i] = degree
    distribution.append(degree)

#for d in range(len(distribution)):
#    distribution[d]=log(distribution[d],2)
distribution.sort()
distribution.reverse()
length=len(distribution)
x_axis=np.array(list(range(1,length+1)))
fig,ax=plt.subplots()
ax.scatter(x_axis,distribution,alpha=0.5)
ax.grid(True)
fig.tight_layout()
ax.set_xlabel('number')
ax.set_ylabel('degree')
ax.set_title(r'degree distribution')

plt.show()


