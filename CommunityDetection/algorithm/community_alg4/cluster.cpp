#include "util.hpp"
#include "cluster.hpp"
#include "algorithm"
#include "math.h"
#include "qpbo/QPBO.h"
#include <sstream>
#include <string>
#include <fstream>
using namespace std;

/** check that files exist for fopen */
FILE* _fopen_(char* fname, const char* mode)
{
  FILE* f = fopen(fname, mode);
  if (f == NULL)
  {
    printf("Couldn't open %s\n", fname);
    exit(1);
  }
  return f;
}

/** Load a graph */
graphData::graphData(char* nodeFeatureFile,
                     char* selfFeatureFile,
                     char* clusterFile,
                     char* edgeFile,
                     int which, // What type of features are being used?
                     int directed) // Should edges be treated as directed?
                     : directed(directed)
{
  int nNodeFeatures;

  map<int, int*> nodeFeatures;
  map<int, int*> simFeatures;
  int* selfFeatures;

  // Read node features for the graph
  FILE* f = NULL;
  
  ifstream f2(nodeFeatureFile);
  if (not f2.is_open())
  {
    printf("Couldn't open %s\n", nodeFeatureFile);
    exit(1);
  }
  string line;
  int i = 0;
  while (getline(f2, line))
  {
    istringstream iss(line);
    string nodeID;
    iss >> nodeID;
    if (nodeIndex.find(nodeID) != nodeIndex.end())
    {
      printf("Got duplicate feature for %s\n", nodeID.c_str());
      delete [] nodeFeatures[i];
    }
    nodeIndex[nodeID] = i;
    indexNode[i] = nodeID;
    
    vector<int> featuresV;
    int f;
    while (iss >> f)
      featuresV.push_back(f);
    nNodeFeatures = featuresV.size();
    int* features = new int [nNodeFeatures];
    for (int f = 0; f < nNodeFeatures; f ++)
      features[f] = featuresV[f];
    nodeFeatures[i] = features;
    i ++;
  }
  nNodes = i;
  f2.close();
  
  if (nNodes > 1200)
  {
    printf("This code will probably run out of memory with more than 1000 nodes!\n");
    printf("Please see our arxiv paper for more scalable versions of the algorithm: http://arxiv.org/abs/1210.8182\n");
    printf("Delete this line (%s, line %d) to continue\n", "cluster.cpp", __LINE__ + 1);
    exit(0);
  }

  // Read the features of the user who created the graph
  f = _fopen_(selfFeatureFile, "r");
  selfFeatures = new int [nNodeFeatures];
  for (int x = 0; x < nNodeFeatures; x ++)
  {
    int z;
    fscanf(f, "%d", &z);
    selfFeatures[x] = z;
  }
  fclose(f);
  for (int i = 0; i < nNodes; i ++)
  {
    int* feature = new int [nNodeFeatures];
    diff(selfFeatures, nodeFeatures[i], nNodeFeatures, feature);
    simFeatures[i] = feature;
  }
  
  // Read the circles
  f = _fopen_(clusterFile, "r");
  char* circleName = new char [1000];
  while (fscanf(f, "%s", circleName) == 1)
  {
    set<int> circle;
    char* nid = new char [100];
    while (true)
    {
      fscanf(f, "%s", nid);
      string nodeID(nid);
      if (nodeIndex.find(nodeID) == nodeIndex.end())
        printf("Got unknown entry in label file: %s\n", nodeID.c_str());
      else
        circle.insert(nodeIndex[nodeID]);
      char c;
      while (true) {          
        c = fgetc(f);
        if (c == '\n') break;
        if (c >= '0' and c <= '9')
        {
          fseek(f, -1, SEEK_CUR);
          break;
        }
      }
      if (c == '\n') break;
    }
    delete [] nid;
    clusters.push_back(circle);
  }
  delete [] circleName;
  fclose(f);

  // Use the appropriate encoding scheme for different feature types
  nEdgeFeatures = 1 + nNodeFeatures;
  if (which == BOTH)
    nEdgeFeatures += nNodeFeatures;
  for (int i = 0; i < nNodes; i ++)
  {
    for (int j = (directed ? 0 : i+1); j < nNodes; j ++)
    {
      if (i == j) continue;
      int* d = new int [nEdgeFeatures];
      d[0] = 1;
      
      if (which == EGOFEATURES)
        diff(simFeatures[i], simFeatures[j], nNodeFeatures, d + 1);
      else if (which == FRIENDFEATURES)
        diff(nodeFeatures[i], nodeFeatures[j], nNodeFeatures, d + 1);
      else
      {
        diff(simFeatures[i], simFeatures[j], nNodeFeatures, d + 1);
        diff(nodeFeatures[i], nodeFeatures[j], nNodeFeatures, d + 1 + nNodeFeatures);
      }
      edgeFeatures[pair<int,int>(i,j)] = makeSparse(d, nEdgeFeatures);
      delete [] d;
    }
  }

  // Read the edges for the graph
  f = _fopen_(edgeFile, "r");
  char* nid1 = new char [100];
  char* nid2 = new char [100];
  while (fscanf(f, "%s %s\n", nid1, nid2) == 2)
  {
    string nID1(nid1);
    string nID2(nid2);
    edgeSet.insert(pair<int,int>(nodeIndex[nID1], nodeIndex[nID2]));
  }
  delete [] nid1;
  delete [] nid2;
  fclose(f);

  for (map<int, int*>::iterator it = nodeFeatures.begin(); it != nodeFeatures.end(); it ++)
    delete [] it->second;
  for (map<int, int*>::iterator it = simFeatures.begin(); it != simFeatures.end(); it ++)
    delete [] it->second;
  delete [] selfFeatures;
}

/** Train the model to predict K clusters */
void Cluster::train(int K, int reps, int gradientReps, int improveReps, Scalar lambda, int seed, int whichLoss)
{
  nTheta = K*gd->nEdgeFeatures;
  if (theta) delete [] theta;
  if (alpha) delete [] alpha;
  theta = new Scalar [nTheta];
  alpha = new Scalar [K];
  
  unsigned int seed_ = seed;
  unsigned int* sptr = &seed_;
  
  // Learning rate
  Scalar increment = 1.0/(1.0*gd->nNodes*gd->nNodes);
  if (gd->directed) increment *= 0.5;

  // Clusters are initially empty
  chat.clear();
  for (int k = 0; k < K; k ++)
    chat.push_back(set<int>());

  for (int rep = 0; rep < reps; rep ++)
  {
    // If it's the first iteration or the solution is degenerate, randomly initialize the weights
    for (int k = 0; k < K; k ++)
      if (rep == 0 or (int) chat[k].size() == 0 or (int) chat[k].size() == gd->nNodes)
      {
        chat[k].clear();
        for (int i = 0; i < gd->nNodes; i ++)
          if (rand_r(sptr) % 2 == 0) chat[k].insert(i);
        for (int i = 0; i < gd->nEdgeFeatures; i ++)
          theta[k*gd->nEdgeFeatures + i] = 0;
        // Just set a single feature to 1 as a random initialization.
        theta[k*gd->nEdgeFeatures + rand_r(sptr)%gd->nEdgeFeatures] = 1.0;
        theta[k*gd->nEdgeFeatures] = 1;
        alpha[k] = 1;
      }

    // Update the latent variables (cluster assignments) in a random order.
    vector<int> order;
    for (int k = 0; k < K; k ++) 
    {
      order.push_back(k);
    }
    for (int k = 0; k < K; k ++)
    {
      for (int o = 0; o < K; o ++)
      {
        int x1 = o;
        int x2 = rand_r(sptr) % K;
        order[x1] ^= order[x2];
        order[x2] ^= order[x1];
        order[x1] ^= order[x2];
      }
    }
    int changed = 0;
    for (vector<int>::iterator k = order.begin(); k != order.end(); k ++)
      chat[*k] = minimize_graphcuts(*k, improveReps, changed);
    printf("loss = %f\n", totalLoss(gd->clusters, chat, gd->nNodes, whichLoss));
    Scalar ll_prev = loglikelihood(theta, alpha, chat);
    if (not changed) break;

    // Perform gradient ascent
    Scalar ll = 0;
    Scalar* dlda = new Scalar [K];
    Scalar* dldt = new Scalar [nTheta];
    for (int iteration = 0; iteration < gradientReps; iteration ++)
    {
      dl(dldt, dlda, K, lambda);
      for (int i = 0; i < nTheta; i ++)
        theta[i] += increment*dldt[i];
      for (int k = 0; k < K; k ++)
        alpha[k] += increment*dlda[k];
      printf(".");
      fflush(stdout);
      ll = loglikelihood(theta, alpha, chat);

      // If we reduced the objective, undo the update and stop.
      if (ll < ll_prev)
      {
        for (int i = 0; i < nTheta; i ++)
          theta[i] -= increment*dldt[i];
        for (int k = 0; k < K; k ++)
          alpha[k] -= increment*dlda[k];
        ll = ll_prev;
        break;
      }
      ll_prev = ll;
    }
    delete [] dlda;
    delete [] dldt;
    printf("\nll = %f\n", ll);
  }
}

/** Optimize the cluster assignments for the k'th cluster */
set<int> Cluster::minimize_graphcuts(int k, int improveReps, int& changed)
{
  int E = gd->edgeFeatures.size();
  int K = chat.size();
  int largestCompleteGraph = 500;
  if (E > largestCompleteGraph*largestCompleteGraph) E = largestCompleteGraph*largestCompleteGraph;

  QPBO<Scalar> q(gd->nNodes,E);
  q.AddNode(gd->nNodes);

  map<pair<int,int>, Scalar> mc00;
  map<pair<int,int>, Scalar> mc11;
  vector<pair<Scalar, pair<int,int> > > diff_c00_c11;

  for (map<pair<int,int>, map<int,int>*>::iterator it = gd->edgeFeatures.begin(); it != gd->edgeFeatures.end(); it ++)
  {
    pair<int,int> e = it->first;
    int e1 = e.first;
    int e2 = e.second;
    int exists = gd->edgeSet.find(e) != gd->edgeSet.end() ? 1 : 0;
    Scalar inp_ = inp(it->second, theta + k*gd->nEdgeFeatures, gd->nEdgeFeatures);
    Scalar other_ = 0;
    for (int l = 0; l < K; l ++)
    {
      if (l == k) continue;
      Scalar d = (chat[l].find(e1) != chat[l].end() and chat[l].find(e2) != chat[l].end()) ? 1 : -alpha[l];
      other_ += d*inp(it->second, theta + l*gd->nEdgeFeatures, gd->nEdgeFeatures);
    }

    Scalar c00;
    Scalar c01;
    Scalar c10;
    Scalar c11;

    if (exists)
    {
      c00 = -other_ + alpha[k]*inp_ + log(1 + exp(other_ - alpha[k]*inp_));
      c01 = c00;
      c10 = c00;
      c11 = -other_ - inp_ + log(1 + exp(other_ + inp_));
    }
    else
    {
      c00 = log(1 + exp(other_ - alpha[k]*inp_));
      c01 = c00;
      c10 = c00;
      c11 = log(1 + exp(other_ + inp_));
    }
    
    mc00[it->first] = c00;
    mc11[it->first] = c11;
    
    if (gd->nNodes <= largestCompleteGraph or exists) q.AddPairwiseTerm(it->first.first, it->first.second, c00, c01, c10, c11);
    else diff_c00_c11.push_back(pair<Scalar, pair<int,int> >(-fabs(c00 - c11), it->first));
  }
  
  if (gd->nNodes > largestCompleteGraph)
  {
    int nEdgesToInclude = largestCompleteGraph*largestCompleteGraph;
    if (nEdgesToInclude > (int) diff_c00_c11.size()) nEdgesToInclude = diff_c00_c11.size();
    
    sort(diff_c00_c11.begin(), diff_c00_c11.end());
    for (int i = 0; i < nEdgesToInclude; i ++)
    {
      pair<int,int> edge = diff_c00_c11[i].second;
      Scalar c00 = mc00[edge];
      Scalar c01 = c00;
      Scalar c10 = c00;
      Scalar c11 = mc11[edge];
      q.AddPairwiseTerm(edge.first, edge.second, c00, c01, c10, c11);
    }
  }

  for (int i = 0; i < gd->nNodes; i ++)
  {
    if (chat[k].find(i) == chat[k].end()) q.SetLabel(i, 0);
    else q.SetLabel(i, 1);
  }

  // See the QPBO readme for documentation.
  q.MergeParallelEdges();
  q.Solve();
  q.ComputeWeakPersistencies();
  if (gd->nNodes > largestCompleteGraph) improveReps = 1;
  for (int it = 0; it < improveReps; it ++)
    q.Improve();

  int* oldLabel = new int [gd->nNodes];
  int* newLabel = new int [gd->nNodes];

  set<int> res;
  for (int i = 0; i < gd->nNodes; i ++)
  {
    newLabel[i] = 0;
    if (q.GetLabel(i) == 1)
    {
      res.insert(i);
      newLabel[i] = 1;
    }
    else if (q.GetLabel(i) < 0 and chat[k].find(i) != chat[k].end())
    {
      res.insert(i);
      newLabel[i] = 1;
    }
    if (chat[k].find(i) == chat[k].end())
      oldLabel[i] = 0;
    else
      oldLabel[i] = 1;
  }
  
  double oldEnergy = 0;
  double newEnergy = 0;
  
  for (map<pair<int,int>, map<int,int>*>::iterator it = gd->edgeFeatures.begin(); it != gd->edgeFeatures.end(); it ++)
  {
    pair<int,int> e = it->first;
    int e1 = e.first;
    int e2 = e.second;
    
    int old_l1 = oldLabel[e1];
    int old_l2 = oldLabel[e2];
    int new_l1 = newLabel[e1];
    int new_l2 = newLabel[e2];
    
    if (old_l1 and old_l2) oldEnergy += mc11[e];
    else oldEnergy += mc00[e];
    
    if (new_l1 and new_l2) newEnergy += mc11[e];
    else newEnergy += mc00[e];
  }
  
  if (newEnergy > oldEnergy or res.size() == 0)
  {
    res.clear();
    res = chat[k];
  }
  else
  {
    for (set<int>::iterator it = chat[k].begin(); it != chat[k].end(); it ++)
      if (res.find(*it) == res.end()) changed = 1;
    for (set<int>::iterator it = res.begin(); it != res.end(); it ++)
      if (chat[k].find(*it) == chat[k].end()) changed = 1;
  }

  delete [] oldLabel;
  delete [] newLabel;

  return res;
}

/** Partial derivatives of log-likelihood */
void Cluster::dl(Scalar* dldt, Scalar* dlda, int K, Scalar lambda)
{
  for (int i = 0; i < nTheta; i ++) dldt[i] = -lambda*sgn<Scalar>(theta[i]);
  for (int k = 0; k < K; k ++) dlda[k] = 0;
  double* inps = new double [K];
  bool** chatFlat = new bool* [K];
  for (int k = 0; k < K; k ++)
  {
    chatFlat[k] = new bool [gd->nNodes];
    for (int n = 0; n < gd->nNodes; n ++)
    {
      chatFlat[k][n] = false;
      if (chat[k].find(n) != chat[k].end()) chatFlat[k][n] = true;
    }
  }

  for (map<pair<int,int>, map<int,int>*>::iterator it = gd->edgeFeatures.begin(); it != gd->edgeFeatures.end(); it ++)
  {
    Scalar inp_ = 0;
    pair<int,int> e = it->first;
    int e1 = e.first;
    int e2 = e.second;
    int exists = gd->edgeSet.find(it->first) != gd->edgeSet.end() ? 1 : 0;
    for (int k = 0; k < K; k ++)
    {
      inps[k] = inp(it->second, theta + k*gd->nEdgeFeatures, gd->nEdgeFeatures);
      Scalar d = chatFlat[k][e1] and chatFlat[k][e2] ? 1 : -alpha[k];
      inp_ += d * inps[k];
    }
    Scalar expinp = exp(inp_);
    Scalar q = expinp / (1 + expinp);
    if (q != q) q = 1; // Avoids isnan in the case of overflow.

    for (int k = 0; k < K; k ++)
    {
      bool d_ = chatFlat[k][e1] and chatFlat[k][e2];
      Scalar d = d_ ? 1 : -alpha[k];
      for (map<int,int>::iterator itf = it->second->begin(); itf != it->second->end(); itf ++)
      {
        int i = itf->first;
        int f = itf->second;
        if (exists) dldt[k*gd->nEdgeFeatures + i] += d*f;
        dldt[k*gd->nEdgeFeatures + i] += -d*f*q;
      }
      if (not d_)
      {
        if (exists) dlda[k] += -inps[k];
        dlda[k] += inps[k]*q;
      }
    }
  }

  for (int k = 0; k < K; k ++)
    delete [] chatFlat[k];
  delete [] chatFlat;
  delete [] inps;
}

/** Compute the log-likelihood of a parameter vector and cluster assignments */
Scalar Cluster::loglikelihood(Scalar* theta, Scalar* alpha, vector<set<int> >& chat)
{
  int K = chat.size();
  bool** chatFlat = new bool* [K];
  for (int k = 0; k < K; k ++)
  {
    chatFlat[k] = new bool [gd->nNodes];
    for (int n = 0; n < gd->nNodes; n ++)
    {
      chatFlat[k][n] = false;
      if (chat[k].find(n) != chat[k].end()) chatFlat[k][n] = true;
    }
  }

  Scalar ll = 0;
  for (map<pair<int,int>, map<int,int>*>::iterator it = gd->edgeFeatures.begin(); it != gd->edgeFeatures.end(); it ++)
  {
    Scalar inp_ = 0;
    pair<int,int> e = it->first;
    int e1 = e.first;
    int e2 = e.second;
    int exists = gd->edgeSet.find(it->first) != gd->edgeSet.end() ? 1 : 0;
    for (int k = 0; k < K; k ++)
    {
      Scalar d = chatFlat[k][e1] and chatFlat[k][e2] ? 1 : -alpha[k];
      inp_ += d * inp(it->second, theta + k*gd->nEdgeFeatures, gd->nEdgeFeatures);
    }
    if (exists) ll += inp_;
    double ll_ = log(1 + exp(inp_));
    ll += -ll_;
  }

  if (ll != ll)
  {
    printf("ll isnan for user\n");
    exit(1);
  }

  for (int k = 0; k < K; k ++)
    delete [] chatFlat[k];
  delete [] chatFlat;
  return ll;
}
