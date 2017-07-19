import matplotlib.pyplot as plt
import numpy as np
'''
from sklearn.externals import joblib
clf = joblib.load( 'rf_classifier.pkl')
importances = clf.feature_importances_
print(importances)
'''
importance=[ 0.05664096 , 0.05678849 , 0.27375491,  0.14556384  ,0.10785706  ,0.06162928, 0.01012452,  0.00881127 , 0.22298582 , 0.05584384]
pr=importance[0]+importance[1]
importance.pop(0)
importance.pop(0)
importance.insert(0,pr)

fig,ax=plt.subplots()
features=('pageRank','resource allocation','jaccard','admic_adar','preferential_attachment','sameCommunity','hopcroft','index_hop','within_inter_cluster')
y_pos = np.arange(len(features))

ax.barh(y_pos, importance,align='center',color='green', ecolor='black')
ax.set_yticks(y_pos)
ax.set_yticklabels(features)
ax.invert_yaxis()  # labels read top-to-bottom
ax.set_xlabel('Importance')
ax.set_title('feature importance')

plt.show()
