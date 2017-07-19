#########
#feature meausres
#########
import networkx as nx
import matplotlib.pyplot as plt

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

def pagerank(file_e):
    G,pairs=build_graph(file_e)
    pr=nx.pagerank(G,alpha=0.85,max_iter=1000)
    pr_list=[]
    for pair in pairs:
        source=pair[0]
        target=pair[1]

        pr_list.append((source,target,[pr[source],pr[target]]))
    return pr_list

def resource_allocation_index(file_e):
    G = nx.Graph()
    with open(file_e) as f:
        lines = f.readlines()
        nodes = set()
        edges = [tuple(line.strip().split()) for line in lines]
        for edge in edges:
            nodes.add(edge[0])
            nodes.add(edge[1])
    nodes=list(nodes)
    G.add_edges_from(edges)
    pairs=[]
    for i in range(len(nodes)-1):
        for k in range(1,len(nodes)-i-1):
            pair=(nodes[i],nodes[i+k])
            pairs.append(pair)

    preds = nx.resource_allocation_index(G, pairs)
    return preds

def jaccard_coefficient(file_e):
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

    preds = nx.jaccard_coefficient(G, pairs)
    return preds

def adamic_adar_index(file_e):
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

    preds = nx.adamic_adar_index(G, pairs)
    return preds

def preferential_attachment(file_e):
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

    preds = nx.preferential_attachment(G, pairs)
    return preds

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

def sameCommunity(file_c,file_e):
    G,pairs=build_graph(file_e)
    circles,communities=community_information(file_c)
    outcome=[[pair[0],pair[1],0]for pair in pairs]

    for p in range(len(pairs)):
        pair=pairs[p]
        source=pair[0]
        target=pair[1]
        for community in circles:
            if source in community and target in community:
                outcome[p][2]+=1
    return outcome


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
'''
def bonus(G,common,communities):
    count=0
    for node in common:
        for community in communities:
            if G.node[node][community]==True:
                count+=1
    return count


def community_belonging(file_e,file_c):
    communities=community_information(file_c)
    G,pairs=build_graph(file_e)
    nodes=nx.nodes(G)
    for node in nodes:
        for community in communities:
            if node in communities[community]:
                G.node[node][community]=node in communities[community]
    cs=[]
    ct=[]
    for pair in pairs:
        community_source=G.node[pair[0]]
        for c in community_source:
            if community_source[c]==True:
                cs.append(c)
        community_target = G.node[pair[1]]
        for c in community_target:
            if community_target[c] == True:
                ct.append(c)
        pra_communities=cs+ct
        neighbour_source=set(G.neighbors(pair[0]))
        neighbour_target=set(G.neighbors(pair[1]))
        common_neighbour=neighbour_source &neighbour_target
        count=bonus(G,common_neighbour,pra_communities)
        value=count+len(common_neighbour)
    return value




cb=community_belonging('/Users/feng/PycharmProjects/social_PJ/facebook/348.edges','/Users/feng/PycharmProjects/social_PJ/facebook/348.circles')
print(list(cb))
'''
'''
count=sameCommunity('/Users/feng/PycharmProjects/social_PJ/facebook/348.circles','/Users/feng/PycharmProjects/social_PJ/facebook/348.edges')
num=[]
belonging_distribution=[]
for key in list(count.keys()):
    c=count[key]
    belonging_distribution.append(c)
num_bins =5

fig, ax = plt.subplots()

# the histogram of the data
n, bins, patches = ax.hist(belonging_distribution, num_bins, normed=1)

# add a 'best fit' line



# Tweak spacing to prevent clipping of ylabel
fig.tight_layout()
plt.show()
'''
'''
r=resource_allocation_index('/Users/feng/PycharmProjects/social_PJ/facebook/348.edges')
re=[(ri[0],ri[1])for ri in r ]
pr=pagerank('/Users/feng/PycharmProjects/social_PJ/facebook/348.edges')
count=0
for key in list(pr.keys()):
    if key not in re:
        print(key)
#None~

'''
#G,pairs=build_graph('/Users/feng/PycharmProjects/social_PJ/facebook/348.edges')
#print(pairs[0:10])
'''
pr=pagerank('/Users/feng/PycharmProjects/social_PJ/facebook/348.edges')
pr_data=[data[2] for data in pr]
print(pr_data[0:10])
'''

