#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "LRUHash.h"
typedef struct entry *entry;
struct entry{
  uint l;
  uint r;
  uint i;
  uint c;
  entry lp;
  entry rp;
};
static entry *T;
static entry mark;
static uint alloc;
void
hashInit(uint n)
{
  assert(1<n && "Too small hash table.");
  alloc = 3*n;
  T = (entry *)calloc(alloc, sizeof(entry));
  mark = (entry)malloc((n+1)*sizeof(struct entry));
  mark->rp = &mark[1];
  mark->lp = &mark[n];
  for(uint i = 1; i < n; i++){
    mark[i].i = 0;
    mark[i].lp = &mark[i-1];
    mark[i].rp = &mark[i+1];
  }
  mark[n].i = 0;
  mark[n].lp = &mark[n-1];
  mark[n].rp = mark;
}
void
hashFree(void)
{
  free(T);
  free(mark);
}
static uint
hash(uint l, uint r)
{
  uint res = 0;
  if(l > r){
    res = 1;
    uint t = l;
    l = r;
    r = t;
  }
  res += 2*((r-1)*r+2*l);
  return res;
}
static void
insertPrim(entry e)
{
  uint i = hash(e->l, e->r) % alloc;
  while(NULL != T[i]){
    i++;
    i%=alloc;
  }
  T[i] = e;
  e->i = i;
}
static void
deletePrim(entry e)
{
  uint i = e->i;
  T[i] = NULL;
  i++;
  i%=alloc;
  while(NULL != T[i]){
    e = T[i];
    T[i] = NULL;
    insertPrim(e);
    i++;
    i%=alloc;
  }
}
static void
dllDelete(entry e)
{
  e->lp->rp = e->rp;
  e->rp->lp = e->lp;
}
static void
dllMTF(entry e)
{
  e->lp = mark;
  e->rp = mark->rp;
  e->lp->rp = e;
  e->rp->lp = e;
}
uint
search(uint l, uint r)
{
  uint i = hash(l, r) % alloc;
  while(NULL != T[i] && !(T[i]->l == l && T[i]->r == r)){
    i++;
    i%=alloc;
  }
  if(NULL != T[i]){
    dllDelete(T[i]);
    dllMTF(T[i]);
    return T[i]->c;
  }
  return -1;
}
void
insert(uint l, uint r, uint c)
{
  entry e = mark->lp;
  dllDelete(e);
  if(e == T[e->i])
    deletePrim(e);
  e->l = l;
  e->r = r;
  e->c = c;
  insertPrim(e);
  dllMTF(e);
}
