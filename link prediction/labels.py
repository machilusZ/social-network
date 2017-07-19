import networkx as nx

def build_graph(file_e):
    G = nx.Graph()
    with open(file_e) as f:
        lines = f.readlines()
        nodes = set()
        edges = [tuple(line.strip().split()) for line in lines]
        for edge in edges:
            nodes.add(edge[0])
            nodes.add(edge[1])
    nodes = list(nodes)
    G.add_edges_from(edges)
    pairs = []
    for i in range(len(nodes) - 1):
        for k in range(1, len(nodes) - i - 1):
            pair = (nodes[i], nodes[i + k])
            pairs.append(pair)
    return G,pairs

def community_information(file_c):
    with open(file_c) as f:

        lines = f.readlines()
        communities={}
        circles=[]
        for line in lines:
            circle = line.strip().split()
            label=circle[0]
            circle = set(circle[1:])
            communities[label]=circle
            circles.append(circle)
    return circles,communities


def hopcroft(file_e,file_c):
    circles,communities=community_information(file_c)
    G,pairs=build_graph(file_e)
    for node in list(G.node.keys()):

        for name,community in communities.items():
            count=0
            if node in community:
                G.node[node]['community']=name
                count+=1
        if count==0:
            G.node[node]['community']='outlier'

    preds = nx.cn_soundarajan_hopcroft(G, pairs)
    return preds

def index_hop(file_e,file_c):
    circles, communities = community_information(file_c)
    G, pairs = build_graph(file_e)
    for node in list(G.node.keys()):

        for name, community in communities.items():
            count = 0
            if node in community:
                G.node[node]['community']=name
                count += 1
        if count == 0:
            G.node[node]['community']='outlier'
    preds = nx.ra_index_soundarajan_hopcroft(G, pairs)
    return preds

def within_inter_cluster(file_e,file_c):
    circles, communities = community_information(file_c)
    G, pairs = build_graph(file_e)
    for node in list(G.node.keys()):

        for name, community in communities.items():
            count = 0
            if node in community:
                G.node[node]['community']=name
                count += 1
        if count == 0:
            G.node[node]['community']='outlier'
    preds = nx.within_inter_cluster(G, pairs)
    return preds





p=within_inter_cluster('/Users/feng/PycharmProjects/social_PJ/facebook/348.edges','/Users/feng/PycharmProjects/social_PJ/facebook/348.circles')

print(list(p))


