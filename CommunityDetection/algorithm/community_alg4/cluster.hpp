#ifndef __CLUSTER_HPP__
#define __CLUSTER_HPP__

#include "vector"
#include "set"
#include "map"
#include "utility"
#include "string"
#include "stdio.h"
#include "stdlib.h"
#include "util.hpp"

/** An object representing a graph and its features */
class graphData
{
public:
  graphData(char* nodeFeatureFile,
            char* selfFeatureFile,
            char* clusterFile,
            char* edgeFile, int which, int directed);
  ~graphData()
  {
    for (std::map<std::pair<int,int>, std::map<int,int>*>::iterator it = edgeFeatures.begin(); it != edgeFeatures.end(); it ++)
      delete it->second;
  }
  
  std::map<std::string, int> nodeIndex;
  std::map<int, std::string> indexNode;
  
  std::vector<std::set<int> > clusters;
  int nEdgeFeatures;
  int nNodes;
  std::map<std::pair<int,int>, std::map<int,int>*> edgeFeatures;
  std::set<std::pair<int,int> > edgeSet;
  int directed;
};

/** This class contains methods to run the clustering algorithm on a graphData object */
class Cluster
{
public:
  Cluster(graphData* gd) : gd(gd)
  {
    theta = NULL;
    alpha = NULL;
  }

  ~Cluster()
  {
    if (theta) delete [] theta;
    if (alpha) delete [] alpha;
  }

  int nTheta;
  Scalar* theta;
  Scalar* alpha;
  std::vector<std::set<int> > chat;
  graphData* gd;

  void train(int K, int reps, int gradientReps, int improveReps, Scalar lambda, int seed, int whichLoss);
  Scalar loglikelihood(Scalar* theta, Scalar* alpha, std::vector<std::set<int> >& chat);
private:
  std::set<int> minimize_graphcuts(int k, int improveReps, int& changed);
  void dl(Scalar* dldt, Scalar* dlda, int K, Scalar lambda);
};

#endif
