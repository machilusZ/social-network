import matplotlib.pyplot as plt
from CPM import accuracy
from  Louvain import l_accuracy
from SCAN import s_accuracy
from numpy import average
import numpy as np

ego=['0','107','348','414','686','698','1684','1912','3437','3980']
prefix='/Users/feng/Desktop/social_network_project/facebook/'
edges=[prefix + i+ '.edges' for i in ego]
circles=[prefix+ i +'.circles' for i in ego]

cpm_score,louvain_socre,scan_score=0,0,0

cpm_accuracy=[]
for i in range(len(edges)):
    edge=edges[i]
    circle=circles[i]
    rate=accuracy(edge,circle)
    print(rate)
    cpm_accuracy.append(rate)
cpm_score=average(cpm_accuracy)


louvain_accuracy=[]
for i in range(len(edges)):
    edge=edges[i]
    circle=circles[i]
    rate=l_accuracy(edge,circle)
    print(rate)
    louvain_accuracy.append(rate)
louvain_score=average(cpm_accuracy)

scan_accuracy=[]
for i in range(len(edges)):
    edge=edges[i]
    circle=circles[i]
    rate=s_accuracy(edge,circle)
    print(rate)
    louvain_accuracy.append(rate)
scan_score=average(cpm_accuracy)

McAuley_score=0.84

fig, ax = plt.subplots()

methods=('CPM','Louvain','SCAN','McAuley')
y_pos = np.arange(len(methods))
performance =[0.737614724384,0.5,0.761167180772,0.84]
ax.barh(y_pos, performance,align='center',
        color='green', ecolor='black')
ax.set_yticks(y_pos)
ax.set_yticklabels(methods)
ax.invert_yaxis()  # labels read top-to-bottom
ax.set_xlabel('Performance')
ax.set_title('Accuracy on detected communities (1 - Balanced Error Rate, higher is better)')

plt.show()





