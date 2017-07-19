'''
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cbook as cbook
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

for i in range(size):
    node = nodes[i]
    degree = len(edges[node])
    diagDegree_mat[i, i] = degree

def pageRank(adjency,degree,alpha,beta):
    n=adjency.shape[0]
    inverse_a=np.linalg.inv(degree)
    trans_a=adjency.T
    I=np.eye(n)
    temp=[[1]]*n
    one=np.array(temp)
    centra=np.dot(beta*(I-alpha*np.dot(trans_a,inverse_a)),one)
    return centra

centra=pageRank(adjency_mat,diagDegree_mat,0.95,0.1)
n=len(centra)
x=np.array(list(range(1,n+1)))
fig,ax=plt.subplots()
ax.scatter(x,centra,alpha=0.5)
ax.grid(True)
fig.tight_layout()
plt.show()
'''
import networkx as nx
import matplotlib.pyplot as plt
import numpy as np

G=nx.Graph()
with open('/Users/feng/PycharmProjects/social_PJ/facebook/0.edges') as f:
    lines = f.readlines()
    edges = [tuple(line.strip().split()) for line in lines]
G.add_edges_from(edges)
rank=nx.pagerank(G,max_iter=1000)

top_list=sorted(rank.items(),key=lambda item:item[1])
top_list.reverse()
print(top_list)
x=[i[1] for i in top_list]


'''

length=len(top_list)
x_axis=np.array(list(range(length)))
rank_value=[item[1] for item in top_list]
fig,ax=plt.subplots()
ax.scatter(x_axis,rank_value,alpha=0.5)
ax.grid(True)
fig.tight_layout()

plt.show()

'''
num_bins =20

fig, ax = plt.subplots()

# the histogram of the data
n, bins, patches = ax.hist(x, num_bins, normed=1)

# add a 'best fit' line

ax.set_xlabel('pageRank value')
ax.set_ylabel('probability distribution')
ax.set_title(r'pageRank distribution')

# Tweak spacing to prevent clipping of ylabel
fig.tight_layout()
plt.show()



