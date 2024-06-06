#ifndef GRAPH_H
#define GRAPH_H 
void
loadGraph(
          );
void
freeGraph(void);
uint
deg(uint j);
uint *
Adj(uint j);
double
getAvg(uint j);
void
freeAvg(void);
void
printGraph(void);
uint
cjj(uint j, uint jj);
uint
totalCross(uint m, uint *L);
uint
getSize(void);
#include "fnv.h"
Fnv64_t
getFNV(void);
#endif
