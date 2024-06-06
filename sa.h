#ifndef _SA_H
#define _SA_H 
double
getTemperature(double cp,
               int d
               );
int
getEll(double T,
       unsigned int *R
       );
void
runSA(double Tstart,
      double Tstop,
      uint batch,
      uint steps,
      uint goal
      );
#endif
