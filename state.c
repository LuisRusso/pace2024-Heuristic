#include <stdlib.h>
#include <stdio.h>
#include <bsd/stdlib.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include "state.h"
#include "graph.h"
#include "splayTree.h"
#include "sigterm.h"
extern uint n0;
extern uint n1;
typedef struct crossings crossings;
struct crossings{
  uint v;
};
static crossings *B;
static uint *BI;
static uint *S;
static uint *SI;
static uint R = 32;
static uint M = 0xFFFFFFFF;
static uint *Bscore;
static uint *Sscore;
static uint *ops;
static uint *BestScore;
void
gdbBreak(void)
{}
static void
setupTree(void)
{
  R = 32 - __builtin_clz (n1-1);
  M = (1<<R)-1;
}
static uint
lca(uint a, uint b)
{
  int s = 32 - __builtin_clz (a ^ b);
  return (a|(1<<R)) >> s;
}
static uint
itop(uint i)
{
  uint s = i << (R + __builtin_clz (i) - 31);
  return s & M;
}
static int
intcmp(const void *p1, const void *p2)
{
  int A = *(const uint *) p1;
  int B = *(const uint *) p2;
  return A - B;
}
static uint
levelSize(uint l)
{
  uint m = itop(l);
  uint n = itop(l+1);
  if((0 == n) || (n > n1)) n = n1;
  return n - m;
}
static void
setBest(uint v)
{
  BestScore[v] = -1;
  if(2*v < (uint)(1<<R))
    BestScore[v] = BestScore[2*v] + BestScore[2*v+1] + Sscore[v] - Sscore[2*v] - Sscore[2*v+1];
  if(BestScore[v] > Bscore[v])
    BestScore[v] = Bscore[v];
}
void
assertConsistency(void)
#ifdef NDEBUG
{}
#else
{
  for(uint i = 0; i < n1; i++){
    assert(SI[S[i]] == i && "Failed saved consistency");
  }
  for(uint i = 0; i < n1; i++){
    assert(BI[B[i].v] == i && "Failed current consistency");
  }
  uint lB[n1];
  for(uint l = 1; l < (uint)(1<<R); l++){
    if(itop(l) < n1){
      for(uint i = 0; i < levelSize(l); i++)
 lB[i] = BI[S[itop(l)+i]];
      qsort(lB, levelSize(l), sizeof(uint), intcmp);
      for(uint i = 0; i < levelSize(l); i++)
 lB[i] = B[lB[i]].v;
      assert(Bscore[l] == totalCross(levelSize(l), lB) && "Score failure");
      assert(Sscore[l] == totalCross(levelSize(l), &S[itop(l)]) && "Score failure");
    }
  }
}
#endif
static void
storeInterval(uint v)
{
  uint l = levelSize(v);
  uint t = itop(v);
  for(uint i = 0; i < l; i++)
    S[t+i] = BI[S[t+i]];
  qsort(&S[t], l, sizeof(uint), intcmp);
  for(uint i = 0; i < l; i++){
    S[t+i] = B[S[t+i]].v;
    SI[S[t+i]] = t+i;
  }
  uint d = Sscore[v];
  uint j = 1;
  while(lca(t,t+j) >= v){
    for(uint i = 0; i < l; i+= 2*j){
      uint len = 2*j;
      if(t+i+len > n1) len = n1 - i - t;
      BestScore[lca(t+i,t+i+j)] = \
 Bscore[lca(t+i,t+i+j)] = \
 Sscore[lca(t+i,t+i+j)] = \
 totalCross(len, &S[t+i]);
      ops[lca(t+i,t+i+j)] = 0;
    }
    j *= 2;
  }
  d -= Sscore[v];
  v /= 2;
  while(0 < v){
    Sscore[v] -= d;
    setBest(v);
    v /= 2;
  }
}
static void
recursiveStore(uint v)
{
  if(v >= (uint)(1<<R))
    return;
  if(Sscore[v] <= BestScore[v]) return;
  if(Bscore[v] <= BestScore[v]){
    storeInterval(v);
    return;
  }
  recursiveStore(2*v);
  recursiveStore(2*v+1);
}
static uint *F = NULL;
static uint
queryF(uint i)
{
  uint s = 0;
  while(0 < i){
    s += F[i];
    i -= i&-i;
  }
  return s;
}
static void
updateF(uint i, uint v)
{
  while(i <= n0){
    F[i] += v;
    i += i&-i;
  }
}
static uint
totalCrossingsR(void)
{
  uint res = 0;
  F = (uint *)calloc(n0+2, sizeof(uint));
  for(uint j = n1; j > 0;){
    j--;
    uint *A = Adj(B[j].v);
    int d = (int)deg(B[j].v);
    A += d-1;
    for(int i = 0; i < d; i++){
      res += queryF(A[-i]-1);
      updateF(A[-i], 1);
    }
  }
  free(F);
  return res;
}
static uint
totalCrossingsL(void)
{
  uint res = 0;
  F = (uint *)calloc(n0+2, sizeof(uint));
  for(uint j = 0; j < n1; j++){
    uint *A = Adj(B[j].v);
    uint d = deg(B[j].v);
    for(uint i = 0; i < d; i++){
      res += queryF(n0-A[i]);
      updateF(1+n0-A[i], 1);
    }
  }
  free(F);
  return res;
}
#define miniM(M,V) if((V) < (M)) (M) = (V);
#define swapM(T,A,B) {T _mt = A; A = B; B = _mt;}
static void
vertexSwap(uint li)
{
  assert("Out of Bounds Swap" && li < n1-1);
  uint l = lca(SI[B[li].v], SI[B[li+1].v]);
  if(0 < vertexEnergy(li) &&
     BestScore[l] < Sscore[l] &&
     ops[l] >= levelSize(l)
     ){
    while(0 < l/2){
      if(ops[l/2] < levelSize(l/2))
 break;
      if(BestScore[l/2] >= Sscore[l/2])
 break;
      if(BestScore[2*(l/2)] >= Sscore[2*(l/2)])
 break;
      if(BestScore[2*(l/2)+1] >= Sscore[2*(l/2)+1])
 break;
      l /= 2;
    }
    recursiveStore(l);
  }
  l = lca(SI[B[li].v], SI[B[li+1].v]);
  while(0 < l){
    ops[l]++;
    Bscore[l] += vertexEnergy(li);
    setBest(l);
    l /= 2;
  }
  swapM(crossings, B[li], B[li+1]);
  BI[B[li].v] = li;
  BI[B[li+1].v] = li+1;
  removeSp(li);
  if(0 < li) removeSp(li-1);
  if(li+1 < n1-1) removeSp(li+1);
  insertSp(li);
  if(0 < li) insertSp(li-1);
  if(li+1 < n1-1) insertSp(li+1);
}
int
vertexEnergy(uint i)
{
  return (int)cjj(B[i+1].v, B[i].v) - (int)cjj(B[i].v, B[i+1].v);
}
static int
cmpAvg(const void *p1, const void *p2)
{
  uint j = *(const uint *) p1;
  uint jj = *(const uint *) p2;
  if(0 == deg(j)) return -1;
  if(0 == deg(jj)) return 1;
  double a1 = getAvg(*(const uint *) p1);
  double a2 = getAvg(*(const uint *) p2);
  int r = 0;
  if(a1 != a2){
    if(a1 > a2)
      r = 1;
    else
      r = -1;
  }
  return r;
}
uint
setInitialState(void)
{
  S = (uint *)malloc(n1*sizeof(uint));
  for(uint i = 0; i < n1; i++) S[i] = n0+1+i;
  qsort(S, n1, sizeof(uint), cmpAvg);
  freeAvg();
  SI = (uint *)malloc(n1*sizeof(uint));
  SI = &SI[(int)-n0-1];
  for(uint i = 0; i < n1; i++) SI[S[i]] = i;
  B = (crossings *)calloc(n1, sizeof(struct crossings));
  for(uint i = 0; i < n1; i++) B[i].v = S[i];
  BI = (uint *)calloc(n1, sizeof(uint));
  BI = &BI[(int)-n0-1];
  for(uint i = 0; i < n1; i++) BI[B[i].v] = i;
  allocSplay(n1);
  uint max[n1];
  uint min[n1];
  if(Adj(S[0]+1) != Adj(S[0]))
    max[0] = Adj(S[0]+1)[-1];
  else
    max[0] = 0;
  for(uint i = 1; i < n1; i++){
    max[i] = max[i-1];
    if(Adj(S[i]+1) != Adj(S[i]) && Adj(S[i]+1)[-1] > max[i])
      max[i] = Adj(S[i]+1)[-1];
  }
  if(Adj(S[n1-1]+1) != Adj(S[n1-1]))
    min[n1-1] = Adj(S[n1-1])[0];
  else
    min[n1-1] = n0 + 1;
  for(int i = n1-2; i >= 0; i--){
    min[i] = min[i+1];
    if(Adj(S[i]+1) != Adj(S[i]) && Adj(S[i])[0] < min[i])
      min[i] = Adj(S[i])[0];
  }
  for(uint i = 0; i+1 < n1; i++){
    if(max[i] <= min[i+1]){
    } else
      insertSp(i);
  }
  setupTree();
  uint al = 1<<R;
  Bscore = (uint *) malloc(al*sizeof(uint));
  BestScore = (uint *) calloc(al, sizeof(uint));
  Sscore = (uint *) calloc(al, sizeof(uint));
  ops = (uint *) calloc(al, sizeof(uint));
  storeInterval(1);
  return Bscore[1];
}
void
freeState(void
   )
{
  freeSplay();
  free(S);
  free(&SI[n0+1]);
  free(B);
  free(&BI[n0+1]);
  free(Bscore);
  free(BestScore);
  free(Sscore);
  free(ops);
}
void
printState(void)
{
  uint i = 0;
  for(; i+1 < n1; i++){
    printf("%d-(%d|%d\t%d-%d = %d)  ",
    B[i].v,
    cjj(B[i+1].v, B[i].v),
    cjj(B[i].v, B[i+1].v),
    cjj(B[i+1].v, B[i].v),
    cjj(B[i].v, B[i+1].v),
    cjj(B[i+1].v, B[i].v) - cjj(B[i].v, B[i+1].v)
    );
    putchar('\n');
  }
}
static void
saveState(uint j)
{
  uint min;
  uint max;
}
void
transitionBound(int ell)
{
  uint li;
  uint p;
               {
    uint pool = getSizeByD(ell);
    if(0 == pool)
      p = 0;
    else
      p = arc4random_uniform(pool);
    li = getNodeOnSplay(p);
  }
  int d = vertexEnergy(li);
  if(0 <= d){
    vertexSwap(li);
  } else {
    int k = 0;
    while(canContinue() &&
   li + k < n1 - 1 &&
   containsSp(li+k) &&
   0 >= vertexEnergy(li+k)
   ){
      vertexSwap(li+k);
      k++;
    }
    k = 1;
    while(canContinue() &&
   k <= (int)li &&
   containsSp(li-k) &&
   0 >= vertexEnergy(li-k)
   ){
      vertexSwap(li-k);
      k++;
    }
  }
}
void
saveMin(void)
{
  recursiveStore(1);
}
void
showMin(void)
{
  for(uint i = 0; i < n1; i++)
    printf("%d\n", S[i]);
}
void
resetState(void)
{
  saveMin();
  for(uint i = 0; i < n1; i++){
    B[i].v = S[i];
    BI[B[i].v] = i;
  }
  for(uint i = 0; i < (uint)(1<<R); i++)
    BestScore[i] = Bscore[i] = Sscore[i];
}
uint
getScore(void)
{
  return Sscore[1];
}
