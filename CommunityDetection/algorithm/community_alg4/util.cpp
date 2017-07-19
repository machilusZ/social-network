#include "util.hpp"
#include "munkres/src/munkres.h"
#include "math.h"
#include "stdlib.h"
#include "stdio.h"
#include "limits"
using namespace std;

// Inner product
Scalar inp(map<int,int>* x, Scalar* y, int D)
{
  Scalar res = 0;
  for (map<int,int>::iterator it = x->begin(); it != x->end(); it ++)
    res += it->second * y[it->first];
  return res;
}

// Make a feature vector sparse
map<int,int>* makeSparse(int* feat, int D)
{
  map<int,int>* res = new map<int,int>();
  for (int i = 0; i < D; i ++)
    if (feat[i]) res->insert(pair<int,int>(i,feat[i]));
  return res;
}

// Compute the loss between a groundtruth cluster l and a predicted cluster lhat
Scalar loss(set<int>& l, set<int>& lhat, int N, int which)
{
  if (l.size() == 0)
  {
    if (lhat.size() == 0) return 0;
    return 1.0;
  }
  if (lhat.size() == 0)
  {
    if (l.size() == 0) return 0;
    return 1.0;
  }
  int tp = 0;
  int fp = 0;
  int fn = 0;

  Scalar ll = 0;
  for (set<int>::iterator it = l.begin(); it != l.end(); it ++)
    if (lhat.find(*it) == lhat.end())
    {
      // false negative
      fn ++;
      if (which == ZEROONE)
        ll += 1.0/N;
      else if (which == SYMMETRICDIFF)
        ll += 0.5/l.size();
    }
  for (set<int>::iterator it = lhat.begin(); it != lhat.end(); it ++)
    if (l.find(*it) == l.end())
    {
      // false positive
      fp ++;
      if (which == ZEROONE)
        ll += 1.0/N;
      else if (which == SYMMETRICDIFF)
        ll += 0.5/(N - l.size());
    }
    else tp ++;

  if ((lhat.size() == 0 or tp == 0) and which == FSCORE) return 1.0;
  Scalar precision = (1.0*tp)/lhat.size();
  Scalar recall = (1.0*tp)/l.size();
  if (which == FSCORE)
    return 1 - 2 * (precision*recall) / (precision + recall);

  return ll;
}

// Compute the optimal loss via linear assignment
Scalar totalLoss(vector<set<int> >& clusters, vector<set<int> >& chat, int N, int which)
{
  Matrix<double> matrix(clusters.size(), chat.size());
  
  for (int i = 0; i < (int) clusters.size(); i ++)
    for (int j = 0; j < (int) chat.size(); j ++)
      matrix(i,j) = loss(clusters[i], chat[j], N, which);

  Munkres m;
  m.solve(matrix);

  Scalar l = 0;
  for (int i = 0; i < (int) clusters.size(); i ++)
    for (int j = 0; j < (int) chat.size(); j ++)
      if (matrix(i,j) == 0) l += loss(clusters[i], chat[j], N, which);

  return l / (clusters.size() < chat.size() ? clusters.size() : chat.size());
}

// Compress the features if we are using a compressed representation
void compress(vector<int> fol, int* feat, int* res)
{
  int where = 0;
  for (int i = 0; i < (int) fol.size(); i ++)
  {
    int sum = 0;
    for (int j = 0; j < fol[i]; j ++)
      sum += feat[where ++];
    res[i] = sum;
  }
}

// Elementwise absolute difference
void diff(int* f1, int* f2, int D, int* res)
{
  for (int i = 0; i < D; i ++)
    res[i] = abs(f1[i] - f2[i]);
}
