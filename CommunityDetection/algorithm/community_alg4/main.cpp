#include "cluster.hpp"
#include "util.hpp"
#include "string.h"
#include <time.h>

using namespace std;

/** Code to run the experiment and save the output in a nice readable format. */
void experiment(graphData* gd,
                int K, // number of circles
                int lambda, // regularization parameter
                int reps, // number of iterations of training
                int gradientReps, // number of iterations of gradient ascent
                int improveReps, // number of iterations using by QPBO
                char* resName // Where to save the results
               )
{
  long starttime = clock();
  printf("Results will be saved to %s\n", resName);

  Cluster c(gd);
  Scalar bestll = 0;
  int bestseed = 0;
  vector<set<int> > bestClusters;
  Scalar* bestTheta = new Scalar [K*c.gd->nEdgeFeatures];
  Scalar* bestAlpha = new Scalar [K*c.gd->nEdgeFeatures];

  // Number of random restarts
  int nseeds = 5;

  for (int seed = 0; seed < nseeds; seed ++)
  {
    printf("Random restart %d of %d\n", seed + 1, nseeds);
    c.train(K, reps, gradientReps, improveReps, lambda, seed, SYMMETRICDIFF);
    Scalar ll = c.loglikelihood(c.theta, c.alpha, c.chat);
    if (ll > bestll or bestll == 0)
    {
      bestll = ll;
      bestseed = seed;
      bestClusters = c.chat;
      memcpy(bestTheta, c.theta, K*c.gd->nEdgeFeatures*sizeof(Scalar));
      memcpy(bestAlpha, c.alpha, K*sizeof(Scalar));
    }
  }

  FILE* f = fopen(resName, "w");
  
  long endtime = clock();
  
  fprintf(f, "seed = %d\n", bestseed);
  fprintf(f, "ll = %f\n", bestll);
  fprintf(f, "loss_zeroone = %f\n", totalLoss(c.gd->clusters, bestClusters, c.gd->nNodes, ZEROONE));
  fprintf(f, "loss_symmetric = %f\n", totalLoss(c.gd->clusters, bestClusters, c.gd->nNodes, SYMMETRICDIFF));
  fprintf(f, "fscore = %f\n", 1 - totalLoss(c.gd->clusters, bestClusters, c.gd->nNodes, FSCORE));
  fprintf(f, "clusters = [");
  for (vector<set<int> >::iterator it = bestClusters.begin(); it != bestClusters.end(); it ++)
  {
    if (it != bestClusters.begin()) fprintf(f, ",");
    fprintf(f, "[");
    for (set<int>::iterator it2 = it->begin(); it2 != it->end(); it2 ++)
    {
      if (it2 != it->begin()) fprintf(f, ",");
      fprintf(f, "%s", gd->indexNode[*it2].c_str());
    }
    fprintf(f, "]");
  }
  fprintf(f, "]\n");

  fprintf(f, "theta = [");
  for (int k = 0; k < K; k ++)
  {
    if (k != 0) fprintf(f, ",");
    fprintf(f, "[");
    for (int i = 0; i < c.gd->nEdgeFeatures; i ++)
    {
      if (i != 0) fprintf(f, ",");
      fprintf(f, "%f", bestTheta[k*c.gd->nEdgeFeatures + i]);
    }
    fprintf(f, "]");
  }
  fprintf(f, "]\n");

  fprintf(f, "alpha = [");
  for (int k = 0; k < K; k ++)
  {
    if (k != 0) fprintf(f, ",");
    fprintf(f, "%f", bestAlpha[k]);
  }
  fprintf(f, "]\n");
  
  fprintf(f, "runtime = %f\n", ((float) (endtime - starttime)) / CLOCKS_PER_SEC);

  fclose(f);
  
  delete [] bestTheta;
  delete [] bestAlpha;
}

int main(int argc, char** argv)
{
  if (argc < 2)
  {
    printf("Expected 2 arguments (userid, output path), e.g.\n");
    printf("%s facebook/698 results.out\n", argv[0]);
    exit(1);
  }

  char* nodeFeatureFile = new char [1000];
  char* selfFeatureFile = new char [1000];
  char* clusterFile = new char [1000];
  char* edgeFile = new char [1000];
  
  sprintf(nodeFeatureFile, "%s.feat", argv[1]);
  sprintf(selfFeatureFile, "%s.egofeat", argv[1]);
  sprintf(clusterFile, "%s.circles", argv[1]);
  sprintf(edgeFile, "%s.edges", argv[1]);
  
  graphData gd(nodeFeatureFile, selfFeatureFile, clusterFile, edgeFile, FRIENDFEATURES, 0);
  experiment(&gd,
             3, // K
             1, // lambda
             25, // training iterations
             50, // gradient reps
             5, // QPBO reps
             argv[2]);

  delete [] nodeFeatureFile;
  delete [] selfFeatureFile;
  delete [] clusterFile;
  delete [] edgeFile;
}
