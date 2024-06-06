#ifndef SPLAYTREE_H
#define SPLAYTREE_H 
#include <stdbool.h>
typedef uint nodeT;
void allocSplay(uint n);
void freeSplay(void);
uint getNodeOnSplay(uint p);
uint getNodeOnLR(uint p);
uint getSizeByD(int d);
void insertSp(nodeT v);
void removeSp(nodeT v);
bool containsSp(nodeT v);
uint sizeSp(void);
uint sizeLR(void);
#endif
