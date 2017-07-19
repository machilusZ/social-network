import networkx as nx
import random
import math
import evaluation

'''
paper : <<SCAN:A Structural Clustering Algorithm for Networks>>
'''
def cal_similarity(G, node_i, node_j):
    '''
        paper : <<SCAN:A Structural Clustering Algorithm for Networks>>
    '''
    s1 = set(G.neighbors(node_i))
    s1.add(node_i)
    s2 = set(G.neighbors(node_j))
    s2.add(node_j)
    return len(s1 & s2) / math.sqrt(len(s1) * len(s2))






class SCAN():
    
    def __init__(self, G, epsilon=0.5, mu=3):
        self._G = G
        self._epsilon = epsilon
        self._mu = mu

    def get_epsilon_neighbor(self, node):
        return [neighbor for neighbor in self._G.neighbors(node) if cal_similarity(self._G,node, neighbor) >= self._epsilon]        

    def is_core(self, node):
        return len(self.get_epsilon_neighbor(node)) >= self._mu
    
    def get_hubs_outliers(self, communities):
        other_nodes = set(self._G.node.keys())
        node_community = {}
        for i,c in enumerate(communities):
            for node in c:
                other_nodes.discard(node)
                node_community[node] = i
        hubs = []
        outliers = []
        for node in other_nodes:
            neighbors = self._G.neighbors(node)
            neighbor_community = set()
            for neighbor in neighbors:
                neighbor_community.add(node_community[neighbor])
            if len(neighbor_community) > 1:
                hubs.append(node)
            else:
                outliers.append(node)
        return hubs,outliers

    def execute(self):
        # random scan nodes
        visit_sequence = self._G.node.keys()
        random.shuffle(visit_sequence)
        communities = []
        for node_name in visit_sequence:
            node = self._G.node[node_name]
            if(node.get("classified") == True):
                continue
            if(self.is_core(node_name)):  # a new community
                community = [node_name]
                communities.append(community)
                node["type"] = "core"
                node["classified"] = True
                queue = self.get_epsilon_neighbor(node_name)
                while(len(queue) != 0):
                    temp = queue.pop(0)
                    if(self._G.node[temp].get("classified") != True):
                        self._G.node[temp]["classified"] = True
                        community.append(temp)
                    if(not self.is_core(temp)):
                        continue
                    R = self.get_epsilon_neighbor(temp)
                    for r in R:
                        node_r = self._G.node[r]
                        is_classified = node_r.get("classified")
                        if(is_classified):
                            continue
                        node_r["classified"] = True
                        community.append(r)
                        if(node_r.get("type") != "non-member"):
                            queue.append(r)
                        else:
                            node["type"] = "non-member"
        return communities

def s_accuracy(edges_,circles):
    G = nx.Graph()
    with open(edges_) as f:
        lines = f.readlines()
        nodes = set()
        edges = [tuple(line.strip().split()) for line in lines]
        for edge in edges:
            nodes.add(edge[0])
            nodes.add(edge[1])
    G.add_edges_from(edges)

    # print G.node.keys()

    # G = nx.karate_club_graph()

    algorithm = SCAN(G, 0.7, 3)
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
    with open('/Users/feng/PycharmProjects/social_PJ/facebook/0.edges') as f:
        lines = f.readlines()
        nodes = set()
        edges = [tuple(line.strip().split()) for line in lines]
        for edge in edges:
            nodes.add(edge[0])
            nodes.add(edge[1])
    G.add_edges_from(edges)

    
    #print G.node.keys()
    
    #G = nx.karate_club_graph()
    
    algorithm = SCAN(G, 0.7, 3)
    communities = algorithm.execute()

    with open('/Users/feng/PycharmProjects/social_PJ/facebook/0.circles') as f:
        lines = f.readlines()
        circles = []
        for line in lines:
            circle = line.strip().split()
            circle = set(circle[1:])
            circles.append(circle)

    overall = evaluation.evaluation(communities, circles, nodes)
    print(overall,communities)

#0.761167180772
