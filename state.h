#ifndef _STATE_H
#define _STATE_H 
#include <stdbool.h>
void
freeState(void
   );
uint
setInitialState(void);
uint
energy(void
       );
void
printState(void
    );
int
vertexEnergy(uint i);
bool
vertexLR(uint i);
void
transitionBound(int ell
  );
void
saveMin(void);
uint
getMin(void);
void
showMin(void);
void
resetState(void);
uint
getScore(void);
#endif
