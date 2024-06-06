#include <time.h>
#include <math.h>
#include <assert.h>
#include <bsd/stdlib.h>
#include <limits.h>
#include "sa.h"
#include "state.h"
#include "graph.h"
#include "splayTree.h"
#include "timer.h"
#include "sigterm.h"
double
getTemperature(double cp,
               int ell
               )
{
  assert(0 >= ell &&
  "Invalid d to define T.");
  assert(0.5 > cp &&
  "Invalid complement probability to define T.");
  double res = cp;
  res /= 1.0 - cp;
  res = log2(res);
  res /= ((double)ell) - 0.5;
  return res;
}
int
getEll(double T,
       unsigned int *pR
       )
{
  double c = 1.0;
  *pR = arc4random();
  unsigned int R = *pR;
  if(0 != R){
    c = log2(-R);
    c -= log2(R);
    c /= T;
    c = round(c);
  }
  return (int) c;
}
void
runSA(double Tstart,
      double Tstop,
      uint batch,
      uint steps,
      uint goal
      )
{
  double Td = Tstop-Tstart;
  Td /= steps;
  unsigned int R;
  while(canContinue() && getScore() > goal){
    double T = Tstart;
    uint i = 0;
    while(i < steps || 0 < getSizeByD(-1)){
      uint j = 0;
      while(j < batch){
 if(!canContinue())
   goto endSA;
 if(0 == sizeSp())
   goto endSA;
 int ell = getEll(T, &R);
 if(i > steps && ell >= 0)
   ell = vertexEnergy(getNodeOnSplay(0));
 transitionBound(ell);
 j++;
      }
      T += Td;
      i++;
    }
    resetState();
  }
 endSA:
  saveMin();
}
