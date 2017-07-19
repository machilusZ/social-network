from networkx import all_pairs_shortest_path_length
from networkx import Graph
import numpy as np
import networkx as nx

G=nx.Graph()
with open('/Users/feng/PycharmProjects/social_PJ/facebook/0.edges') as f:
    lines = f.readlines()
    edges = [tuple(line.strip().split()) for line in lines]
    G.add_edges_from(edges)
    length_dict = all_pairs_shortest_path_length(G)
d=[]
for source in list(length_dict.keys()):
    for target in list(length_dict[source]):
        if length_dict[source][target]==11:
            d.append((source,target))
#[('255', '198'), ('198', '255'), ('198', '241'), ('241', '198')]
path1=nx.shortest_path(G,source='255',target='198')
path2=nx.shortest_path(G,source='198',target='241')
path=[path1,path2]
print(path)

length_list=[]
for source in length_dict:
    for target in length_dict[source]:
        if source==target:
            continue
        l=length_dict[source][target]
        length_list.append(l)
avg_length=sum(length_list)/(float(len(length_list)))
print((avg_length))
#avg_length=3.75
#according to facebook's report in 2016 , the value can be lowered to 3.57 ;
#since we are using ego network as our data ,while facebook authority used all user's data
diameter=max(length_list)
print(diameter)
#diameter=11 in our ego network