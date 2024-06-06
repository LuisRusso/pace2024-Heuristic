#include <stdlib.h>
#include <assert.h>
#include "state.h"
#include "splayTree.h"
#define assertM(T,M) { assert((T) && (M)); }
typedef struct node *splayT;
struct node {
  nodeT above;
  nodeT below;
  nodeT father;
  uint size;
};
static splayT t;
static nodeT root;
static void splay(nodeT v);
static nodeT *getHook(nodeT v);
static void setSize(nodeT v);
static void rotate(nodeT v);
enum childClass { above, below, across };
void allocSplay(uint n)
{
  root = 0;
  t = (splayT)calloc(n+1, sizeof(struct node));
  for(uint i = 1; i <= n; i++)
    t[i].size = 1;
}
void freeSplay(void)
{
  free(t);
}
enum childClass classify(nodeT v)
{
  if(0 != t[v].father && v == t[t[v].father].above) return above;
  if(0 != t[v].father && v == t[t[v].father].below) return below;
  return across;
}
static nodeT *getHook(nodeT v)
{
  if(v == t[t[v].father].above) return &t[t[v].father].above;
  return &t[t[v].father].below;
}
void setSize(nodeT v)
{
  t[v].size = 1;
  if(0 != t[v].above) t[v].size += t[t[v].above].size;
  if(0 != t[v].below) t[v].size += t[t[v].below].size;
}
static void rotate(nodeT v)
{
  assertM(0 != t[v].father, "Trying to rotate node without a father");
  nodeT u = t[v].father;
  *getHook(u) = v;
  nodeT *ua = getHook(v);
  nodeT *vb = &t[v].below;
  if(below == classify(v)) vb = &t[v].above;
  *ua = *vb;
  *vb = u;
  t[v].father = t[u].father;
  t[*vb].father = v;
  t[*ua].father = u;
  t[v].size = t[u].size;
  setSize(u);
#ifndef NDEBUG
#endif
}
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdangling-else"
static void splay(nodeT v)
{
  while(across != classify(v)){
    nodeT u = t[v].father;
    if(across != classify(u))
      if(classify(v) == classify(u))
        rotate(u);
      else rotate(v);
    rotate(v);
  }
  root = v;
}
#pragma GCC diagnostic pop
uint getNodeOnSplay(uint p)
{
  nodeT v = root;
  if(t[v].size <= p) return 0;
  nodeT u = t[v].above;
  while(p != t[u].size){
    if(t[u].size > p)
      v = u;
    else {
      p -= t[u].size + 1;
      v = t[v].below;
    }
    u = t[v].above;
  }
  splay(v);
  return v-1;
}
uint getSizeByD(int d)
{
  nodeT v = root;
  nodeT u;
  do {
    u = v;
    if(vertexEnergy(v-1) > d)
      v = t[v].above;
    else
      v = t[v].below;
  } while (0 != v);
  splay(u);
  uint r = 0;
  if(t[u].above != 0)
    r = t[t[u].above].size;
  if(vertexEnergy(u-1) <= d)
    r++;
  return r;
}
void insertSp(nodeT i)
{
  assert(t[i+1].above == 0);
  assert(t[i+1].below == 0);
  assert(t[i+1].father == 0);
  assert(t[i+1].size == 1);
  if(0 == root)
    root = i+1;
  else {
    nodeT v = root;
    nodeT u;
    nodeT *p;
    while(v != 0){
      u = v;
      t[u].size++;
      if(vertexEnergy(i) < vertexEnergy(v-1))
 p = &t[v].above;
      else
 p = &t[v].below;
      v = *p;
    }
    *p = i+1;
    t[i+1].father = u;
    splay(i+1);
  }
}
void removeSp(nodeT v)
{
  v++;
  splay(v);
  if(1 == t[v].size)
    root = 0;
  else {
    if(0 == t[v].below){
      root = t[v].above;
      t[root].father = 0;
    } else {
      nodeT s = t[v].below;
      while(t[s].above != 0)
 s = t[s].above;
      splay(s);
      assert(0 < t[s].size);
      t[s].size--;
      t[s].above = t[v].above;
      if(0 != t[s].above)
 t[t[s].above].father = s;
    }
    t[v].above = 0;
    t[v].below = 0;
    t[v].father = 0;
    t[v].size = 1;
  }
}
bool containsSp(nodeT v)
{
  splay(v+1);
  return v+1 == root;
}
uint sizeSp(void)
{
  uint r = 0;
  if(0 != root)
    r = t[root].size;
  return r;
}
