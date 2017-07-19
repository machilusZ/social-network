from collections import defaultdict
import networkx as nx
import matplotlib.pyplot as plt
import matplotlib.colors as col
import evaluation

'''
paper : <<Uncovering the overlapping community structure of complex networks in nature and society>>
'''

class CPM():
    
    def __init__(self,G,k=4):
        self._G = G
        self._k = k

    def execute(self):
        # find all cliques which size > k
        cliques = list(nx.find_cliques(self._G))
        vid_cid = defaultdict(lambda:set())
        for i,c in enumerate(cliques):
            if len(c) < self._k:
                continue
            for v in c:
                vid_cid[v].add(i)
        
        # build clique neighbor
        clique_neighbor = defaultdict(lambda:set())
        remained = set()
        for i,c1 in enumerate(cliques):
            #if i % 100 == 0:
                #print i
            if len(c1) < self._k:
                continue
            remained.add(i)
            s1 = set(c1)
            candidate_neighbors = set()
            for v in c1:
                candidate_neighbors.update(vid_cid[v])
            candidate_neighbors.remove(i)
            for j in candidate_neighbors:
                c2 = cliques[j]
                if len(c2) < self._k:
                    continue
                if j < i:
                    continue
                s2 = set(c2)
                if len(s1 & s2) >= min(len(s1),len(s2)) -1:
                    clique_neighbor[i].add(j)
                    clique_neighbor[j].add(i) 
        
        # depth first search clique neighbors for communities
        communities = []
        for i,c in enumerate(cliques):
            if i in remained and len(c) >= self._k:
                #print 'remained cliques', len(remained)
                communities.append(set(c))
                neighbors = list(clique_neighbor[i])
                while len(neighbors) != 0:
                    n = neighbors.pop()
                    if n in remained:
                        #if len(remained) % 100 == 0:
                            #print 'remained cliques', len(remained)
                        communities[len(communities)-1].update(cliques[n])
                        remained.remove(n)
                        for nn in clique_neighbor[n]:
                            if nn in remained:
                                neighbors.append(nn)
        return communities
        

def draw_graph(file):
    G = nx.Graph()
    with open(file) as f:
        lines = f.readlines()
        nodes = set()
        edges = [tuple(line.strip().split()) for line in lines]
    G.add_edges_from(edges)
    pos = nx.spring_layout(G)
    count=0
    colors=[]

    algorithm = CPM(G, 4)
    communities = algorithm.execute()

    for name,hex in col.cnames.items():
        colors.append(name)
    for community in communities:
        nx.draw_networkx_nodes(G, pos, node_color=colors[count], nodelist=list(community), node_size=10, alpha=0.8)
        count += 3
        print(community)
    nx.draw_networkx_edges(G, pos, width=1.0, alpha=1)
    plt.show()

def accuracy(edges,circles):
    G = nx.Graph()

    with open(edges) as f:
        lines = f.readlines()
        nodes = set()
        edges = [tuple(line.strip().split()) for line in lines]
        for edge in edges:
            nodes.add(edge[0])
            nodes.add(edge[1])
    G.add_edges_from(edges)

    algorithm = CPM(G, 4)
    communities = algorithm.execute()
    with open(circles) as f:
        lines = f.readlines()
        circles = []
        for line in lines:
            circle = line.strip().split()
            circle = set(circle[1:])
            circles.append(circle)

    overall = evaluation.evaluation(communities, circles, nodes)
    return overall




if __name__ == '__main__':
    G = nx.Graph()

    with open('/Users/feng/PycharmProjects/social_PJ/facebook/348.edges') as f:
        lines = f.readlines()
        nodes = set()
        edges = [tuple(line.strip().split()) for line in lines]
        for edge in edges:
            nodes.add(edge[0])
            nodes.add(edge[1])
    G.add_edges_from(edges)
    algorithm = CPM(G, 4)
    communities = algorithm.execute()
    '''
    with open('/Users/feng/PycharmProjects/social_PJ/facebook/348.circles') as f:
        lines = f.readlines()
        circles = []
        for line in lines:
            circle = line.strip().split()
            circle = set(circle[1:])
            circles.append(circle)

    overall = evaluation.evaluation(communities, circles, nodes)
    print(overall)
    '''
    nodes=list(G.node.keys())
    length=len(nodes)
    clength=0
    for community in communities:
        clength+=len(community)
        print('done')
    truth=length==clength
    print(truth)


    