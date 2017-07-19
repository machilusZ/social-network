#include <bits/stdc++.h>
#define FOR(i, l, r) for (int i = int(l); i < int(r); i++)
#define FST first
#define SCD second
#define modp 1000000007
#define MAXN 5013
#define MAX_CYCLE 1000000
#define MAX_ANTS 30
#define DIS 0
using namespace std;

const float alpha = 1;
const float rho = 0.99;
const float tauMin = 0.01, tauMax = 6;

int N, M, Es[MAXN][MAXN], nEs[MAXN][MAXN], En[MAXN];
int Best;
bool connect[MAXN][MAXN];
float tau[MAXN];
clock_t startTime = clock();

void initG(char *fileName)
{
    freopen(fileName, "r", stdin);
    scanf("p edge %d %d", &N, &M);
    FOR(i, 0, M) {
        int u, v;
        scanf(" e %d %d", &u, &v);
        u--;
        v--;
        connect[u][v] = connect[v][u] = true;
        Es[u][En[u]++] = v;
        Es[v][En[v]++] = u;
    }
    FOR(i, 0, N) {
        tau[i] = tauMax;
        int tmp = 0;
        FOR(j, 0, N) {
            if (!connect[i][j]) {
                nEs[i][tmp++] = j;
            }
        }
    }
}
float mpow(float x, int k)
{
    float res = 1;
    while (k) {
        res *= x;
        if (k & 1) {
            x *= x;
        }
        k >>= 1;
    }
    return res;
}
int choose(float *p, int n)
{
    float ran = rand() / (double)RAND_MAX * p[n - 1];
    int l = 0, r = n - 1;
    while (l < r) {
        int m = (l + r) / 2;
        if (p[m] < ran) {
            l = m + 1;
        }
        else {
            r = m;
        }
    }
    return l;
}
void walk(int startV, int *clique)
{
    int candidates[MAXN], candNum = En[startV];
    float p[MAXN], total = 0.;
    clique[clique[0] = 1] = startV;
    FOR(i, 0, candNum) {
        candidates[i] = Es[startV][i];
        p[i] = mpow(tau[candidates[i]], alpha) + total;
        total = p[i];
    }
    while (candNum) {
        int id = choose(p, candNum);
        int v = candidates[id];
        clique[++clique[0]] = v;
        candidates[id] = candidates[--candNum];
        total = 0;
        FOR(i, 0, candNum) {
            if (connect[v][candidates[i]]) {
                p[i] = mpow(tau[candidates[i]], alpha) + total;
                total = p[i];
            }
            else {
                candidates[i--] = candidates[--candNum];
            }
        }
    }
}
int optimize(int* clique) {
	int vi, i, stop, found;
	int delta = 0;
	i = rand() % clique[0];
	stop = i;
	while (1){
		vi = clique[i + 1];
		found = 0;
		int num = N - En[vi] - 1;
		FOR(j, 0, num) {
			int vj = nEs[vi][j];
			FOR(k, j + 1, num) {
				int vk = nEs[vi][k];
				if (connect[vj][vk]) {
                    int l = 0;
					while ((l < clique[0]) && ((clique[l + 1] == vi) || (connect[vj][clique[l + 1]] && connect[vk][clique[l + 1]]))) {
                        l++;
					}
					if (l == clique[0]){
                        clique[i + 1] = vj;
                        clique[++clique[0]] = vk;
                        found = 1;
					}
					else if (!connect[vj][clique[l + 1]]) {
                        break;
					}
				}
			}
			if (found) {
                break;
			}
		}
		if (found){
			stop = i;
		}
		else{
			i++;
			if (i == clique[0]) {
                i=0;
			}
			if (i == stop) {
                return delta;
			}
		}
	}
}

void evaporate()
{
    FOR(i, 0, N) {
        tau[i] = max(tau[i] * rho, tauMin);
    }
}
void reinforce(int *clique, float delta)
{
    FOR(i, 0, clique[0]) {
        tau[clique[i + 1]] = min(tau[clique[i + 1]] + delta, tauMax);
    }
}
int solve(char *fileName)
{
    int bestResult[MAXN], bestCycleResult[MAXN], tmpResult[MAXN];
    clock_t lastTime = clock();
    int lastCycle = 0;
    initG(fileName);
    bestResult[0] = 0;
    for (int cycle = 0; bestResult[0] != Best && cycle < MAX_CYCLE; cycle++) {
        bestCycleResult[0] = 0;
        FOR(ant, 0, MAX_ANTS) {
            walk(rand() % N, tmpResult);
            if (tmpResult[0] > bestCycleResult[0]) {
                memcpy(bestCycleResult, tmpResult, sizeof(int) * (tmpResult[0] + 1));
            }
        }
        optimize(bestCycleResult);
        if (bestResult[0] < bestCycleResult[0]) {
            memcpy(bestResult, bestCycleResult, sizeof(int) * (bestCycleResult[0] + 1));
            printf("Current best result: %d, at cycle %d (%fs)\n", bestResult[0], cycle, (float)(clock() - startTime) / CLOCKS_PER_SEC);
            FOR(i, 0, bestResult[0]) {
                printf("%d%c", bestResult[i + 1] + 1, " \n"[i == bestResult[0] - 1]);
            }
            lastTime = clock();
            lastCycle = cycle;
            FOR(i, 0, bestResult[0]) {
                FOR(j, 0, i) {
                    if (!connect[bestResult[i + 1]][bestResult[j + 1]]) {
                        printf("!!!!!!!!!!%d %d\n", bestResult[i + 1], bestResult[j + 1]);
                        while (1);
                    }
                }
            }
        }
        evaporate();
		float delta = 1.0/(float)(bestResult[0] + 1 - bestCycleResult[0]);
		reinforce(bestCycleResult, delta);
    }
    sort(bestResult + 1, bestResult + bestResult[0] + 1);
    printf("Get current result at cycle %d (%fs)\n", lastCycle, (float)(lastTime - startTime) / CLOCKS_PER_SEC);
    FOR(i, 0, bestResult[0]) {
        printf("%d%c", bestResult[i + 1] + 1, " \n"[i == bestResult[0] - 1]);
    }
    return bestResult[0];
}
int read(char *s)
{
    int res = 0;
    while (*s >= '0' && *s <= '9') {
        res *= 10;
        res += (*s) - '0';
        s++;
    }
    return res;
}
int main(int argc, char *argv[])
{
    char *fileName = "..\\dat\\frb100-40.clq";
//    fileName = "..\\dat\\frb53-24-clq\\frb53-24-1.clq";
    int l = 1;
    int seed = time(0);
    Best = -1;
    while (l < argc) {
        string mod = string(argv[l++]);
        if (mod == string("-i")) {
            fileName = argv[l++];
        }
        if (mod == string("-s")) {
            seed = read(argv[l++]);
        }
        if (mod == string("-b")) {
            Best = read(argv[l++]);
        }
    }
    srand(seed);
    int res = solve(fileName);
    printf("End at %d (%fs)\n\n", res, (float)(clock() - startTime) / CLOCKS_PER_SEC);
    //freeG(N);
    return 0;
}
