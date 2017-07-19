from sklearn.externals import joblib
from sklearn import svm
from sklearn import datasets


clf = joblib.load( 'rf_classifier.pkl')
importances = clf.feature_importances_
print(importances)