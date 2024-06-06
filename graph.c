#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stddef.h>
#include "graph.h"
#include "LRUHash.h"
typedef struct edge edge;
struct edge{
  uint a;
  uint b;
};
uint n0;
uint n1;
static uint m;
static uint *C;
static uint *A;
static double *avg;
static uint *F = NULL;
static uint *Fv = NULL;
static uint stampF = 0;
void
loadGraph(
          )
{
  FILE *f = stdin;
  char *line = NULL;
  size_t len = 0;
  char *tok;
  do getline(&line, &len, f); while('c' == line[0]);
  tok = strtok(line," ");
  tok = strtok(NULL," ");
  tok = strtok(NULL," ");
  sscanf(tok, "%ud", &n0);
  tok = strtok(NULL," ");
  sscanf(tok, "%ud", &n1);
  tok = strtok(NULL," ");
  sscanf(tok, "%ud", &m);
  edge *E;
  E = (edge *) malloc(m*sizeof(edge));
  for(uint i = 0; i < m; i++){
    do getline(&line, &len, f); while('c' == line[0]);
    tok = strtok(line," ");
    sscanf(tok, "%ud", &E[i].a);
    tok = strtok(NULL," ");
    sscanf(tok, "%ud", &E[i].b);
  }
  avg = (double *) calloc(n1, sizeof(double));
  avg -= n0+1;
  edge *Ex;
  Ex = (edge *) malloc(m*sizeof(edge));
  C = (uint *) calloc(2+n0, sizeof(uint));
  for(uint i = 0; i < m; i++) C[1+E[i].a]++;
  for(uint j = 1; j < 2+n0; j++) C[j] += C[j-1];
  for(uint i = 0; i < m; i++) Ex[C[E[i].a]++] = E[i];
  free(C);
  free(E);
  E = Ex;
  A = (uint *) malloc(m*sizeof(uint));
  C = (uint *) calloc(3+n1, sizeof(uint));
  C -= n0;
  for(uint i = 0; i < m; i++) C[2+E[i].b]++;
  for(uint j = 1+n0; j < 3+n1+n0; j++) C[j] += C[j-1];
  for(uint i = 0; i < m; i++){
    avg[E[i].b] += E[i].a;
    A[C[1+E[i].b]++] = E[i].a;
  }
  free(E);
  for(uint j = 1+n0; j <= n1+n0; j++)
    if(0 != deg(j))
      avg[j] /= deg(j);
  free(line);
  F = (uint *)calloc(n0+2, sizeof(uint));
  Fv = (uint *)calloc(n0+2, sizeof(uint));
  stampF = 0;
  uint a = 10*n1;
  if(a < m) a = m;
  hashInit(a);
}
uint
deg(uint j)
{
  return C[j+1]-C[j];
}
uint *
Adj(uint j)
{
  return &A[C[j]];
}
double
getAvg(uint j)
{
  return avg[j];
}
void
freeAvg(void)
{
  free(&avg[n0+1]);
}
void
freeGraph(void)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfree-nonheap-object"
  free(&C[n0]);
#pragma GCC diagnostic pop
  free(A);
  free(F);
  free(Fv);
  hashFree();
}
void
printGraph(void)
{
  printf("B=  ");
  for(uint j = n0+1; j <= n0+n1; j++)
    printf("%2d ", j);
  putchar('\n');
  printf("A=  ");
  for(uint i = 1; i <= n0; i++)
    printf("%2d ", i);
  putchar('\n');
  for(uint j = n0+1; j <= n0+n1; j++){
    printf("%2d: ", j);
    uint di = 0;
    for(uint i = 1; di < deg(j) && i <= n0; i++){
      if(Adj(j)[di] == i){
 printf("%2d ", i);
 di++;
      } else printf("   ");
    }
    printf("\n");
  }
}
uint
cjj(uint j, uint jj)
{
  uint res = search(j, jj);
  if((uint)-1 == res){
    res = 0;
    uint *la = Adj(j);
    uint nl = deg(j);
    uint *ra = Adj(jj);
    uint nr = deg(jj);
    uint i = 0;
    uint jjj = j;
    j = 0;
    while(i < nl && j < nr){
      if(la[i] == ra[j]){
 i++;
 res+=j;
 j++;
      } else {
 if(la[i] < ra[j]){
   uint p2 = 1;
   uint a = i;
   while(i+p2 < nl && la[i+p2] < ra[j]) p2 <<= 1;
   uint b = i+p2;
   if(b >= nl) b = nl;
   while(a+1 < b){
     uint m = a + (b - a)/2;
     if(la[m] < ra[j]) a = m;
     else b = m;
   }
   res += j*(b-i);
   i = b;
 } else {
   uint p2 = 1;
   uint a = j;
   while(j+p2 < nr && la[i] > ra[j+p2]) p2 <<= 1;
   uint b = j+p2;
   if(b >= nr) b = nr;
   while(a+1 < b){
     uint m = a + (b - a)/2;
     if(la[i] > ra[m]) a = m;
     else b = m;
   }
   j = b;
 }
      }
    }
    res += j*(nl-i);
    insert(jjj, jj, res);
  }
  return res;
}
static uint
queryF(uint i)
{
  uint s = 0;
  while(0 < i){
    if(stampF != Fv[i]){
      Fv[i] = stampF;
      F[i] = 0;
    }
    s += F[i];
    i -= i&-i;
  }
  return s;
}
static void
updateF(uint i, uint v)
{
  while(i <= n0){
    if(stampF != Fv[i]){
      Fv[i] = stampF;
      F[i] = 0;
    }
    F[i] += v;
    i += i&-i;
  }
}
uint
totalCross(uint m, uint *L)
{
  stampF++;
  if(0 == stampF){
    bzero(Fv, (n0+2)*sizeof(uint));
    stampF++;
  }
  uint res = 0;
  for(uint j = 0; j < m; j++){
    uint *A = Adj(L[j]);
    uint d = deg(L[j]);
    for(uint i = 0; i < d; i++){
      res += queryF(n0-A[i]);
      updateF(1+n0-A[i], 1);
    }
  }
  return res;
}
uint
getSize(void)
{
  return n0+n1;
}
Fnv64_t
getFNV(void)
{
  Fnv64_t h = 0;
  h = fnv_64_buf(&n0, sizeof(uint), h);
  h = fnv_64_buf(&n1, sizeof(uint), h);
  h = fnv_64_buf(&m, sizeof(uint), h);
  h = fnv_64_buf(&C[1], n1*sizeof(uint), h);
  h = fnv_64_buf(A, m*sizeof(uint), h);
  return h;
}
