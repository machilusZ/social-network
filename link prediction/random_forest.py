import matplotlib.pyplot as plt
import re
from sklearn.ensemble import RandomForestClassifier
import numpy as np
from sklearn.externals import joblib

############################
#STEP1:READ IN TRAINING DATA
############################




with open("/Users/feng/Desktop/social_network_project/data.csv") as f:
    lines=f.readlines()
    labels=[]
    features=[]
    count=0
    for line in lines:

        line = re.split(r',', line.strip())
        pr1 = eval(line[0][2:-1])
        pr2 = eval(line[1][1:-2])
        line.pop(0)
        line.pop(0)
        line =list(map(float, line))

        line.insert(0, pr1)
        line.insert(1,pr2)
        feature = line[0:-1]
        label = line[-1]
        if count==3:
            print(features)
        labels.append(label)
        features.append(feature)
        count+=1
    print('feature done')

features=np.array(features,dtype=object)
labels=np.array(labels,dtype=float)
################################
#STEP1.1 Devide training&test data
#################################
examples=[]
prediction=[]
length=len(labels)
test_index=[]
answers=[]
count=0
while True:
    index=np.random.randint(0,length)
    test_index.append(index)
    count+=1
    if count>138000:
        break
for index in test_index:
    examples.append(features[index])
    answers.append(labels[index])

print('testing set done')
print('traing set begin')

traing_set=[]
traing_labels=[]
count=0
for index in range(len(features)):
    if index in test_index:
        continue
    traing_set.append(features[index])

    traing_labels.append(labels[index])
    count+=1
    if count%10000==0:
        print(str(count)+'training data done')

print("training set done")



################################
#STEP2:TRINING A CLASSFIER
################################
clf = RandomForestClassifier(n_estimators = 500, bootstrap = True, oob_score = True)
clf = clf.fit(traing_set,traing_labels)
print('classifier done')
joblib.dump(clf, 'rf_classifier.pkl')

##################
#STEP3:EVALUATION
##################

score=clf.score(examples,answers)
print(score)


