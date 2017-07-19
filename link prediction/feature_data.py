import feature
import csv
import numpy as np



ego=['0','107','348','414','686','698','1684','1912','3437','3980']
prefix='/Users/feng/Desktop/social_network_project/facebook/'
edges=[prefix + i+ '.edges' for i in ego]


pageRank_list=[]
resource_allocation_list=[]
jaccard_coefficient_list=[]
adamic_adar_list=[]
preferential_attachment_list=[]
same_community_list=[]
hopcroft_list=[]
index_hop_list=[]
within_inter_cluster_list=[]

#Construct Features
count=1
for edge in edges:
    circle=edge[:-5]+'circles'
    pr_data=feature.pagerank(edge)
    pr_data=[data[2] for data in pr_data]
    pageRank_list+=pr_data

    print('pr'+str(count)+'done')




    resource_allocation=list(feature.resource_allocation_index(edge))
    resorce_data=[re[2] for re in resource_allocation]
    resource_allocation_list+=resorce_data
    print('resource_allocation'+str(count)+'done')


    jaccard_coefficient=list(feature.jaccard_coefficient(edge))
    jaccard_data=[jaccard[2] for jaccard in jaccard_coefficient]
    jaccard_coefficient_list+=jaccard_data
    print('jaccard' + str(count) + 'done')


    adamic_adar_index=list(feature.adamic_adar_index(edge))
    adamic_data=[adamic[2] for adamic in adamic_adar_index]
    adamic_adar_list+=adamic_data
    print('admic' + str(count) + 'done')


    preferential_attachment=list(feature.preferential_attachment(edge))
    preferential_data=[pre[2] for pre in preferential_attachment]
    preferential_attachment_list+=preferential_data
    print('prefer'+str(count)+'done')


    sameCommunity=feature.sameCommunity(circle,edge)
    same_data=[same[2] for same in sameCommunity]
    same_community_list+=same_data
    print('same'+str(count)+'done')


    hopcroft = feature.hopcroft(edge,circle)
    hopcroft_data = [hop[2] for hop in hopcroft]
    hopcroft_list+=same_data
    print('hop' + str(count) + 'done')


    index_hop = feature.index_hop(edge,circle)
    index_hop_data = [hop[2] for hop in index_hop]
    index_hop_list+=index_hop_data
    print('index_hop' + str(count) + 'done')


    within_inter_cluster = feature.within_inter_cluster(edge, circle)
    within_inter_cluster_data = [hop[2] for hop in within_inter_cluster]
    within_inter_cluster_list+=within_inter_cluster_data
    print('within_inter_cluster' + str(count) + 'done')


    if len(pageRank_list)==len(same_community_list):
        print(str(count)+'data ready')
    else:
        print('something goes wrong')

    count+=1



data=[]
'''
resource_allocation_list=list(np.array(resource_allocation_list).flat)
jaccard_coefficient_list=list(np.array(jaccard_coefficient_list).flat)
adamic_adar_list=list(np.array(adamic_adar_list).flat)
preferential_attachment_list=list(np.array(preferential_attachment_list).flat)
same_community_list=list(np.array(same_community_list).flat)
hopcroft_list=list(np.array(hopcroft_list).flat)
index_hop_list=list(np.array(index_hop_list).flat)
within_inter_cluster_list=list(np.array(within_inter_cluster_list).flat)
'''


data.append(pageRank_list)
data.append(resource_allocation_list)
data.append(jaccard_coefficient_list)
data.append(adamic_adar_list)
data.append(preferential_attachment_list)
data.append(same_community_list)
data.append(hopcroft_list)
data.append(index_hop_list)
data.append(within_inter_cluster_list)



#6 features , 2 more left to commute

#truths
truth_list=[]
count=0
for edge in edges:
    with open(edge) as f:

        G,pairs=feature.build_graph(edge)
        truths=[int(G.has_edge(pair[0],pair[1])) for pair in pairs]

        print('truth'+str(count)+' Done')
        count+=1

        truth_list+=truths

if len(truth_list)==len(within_inter_cluster_list):
    print('truth_list ready')
data.append(truth_list)

data=np.array(data).T

print(data.shape)

with open('/Users/feng/Desktop/social_network_project/data.csv','wb') as f:
    writer = csv.writer(f)
    writer.writerows(data)








