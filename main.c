#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"
#include "sa.h"
#include "state.h"
#include "timer.h"
#include "sigterm.h"
int
main(int argc,
     char **argv
     )
{
    int bp = 4;
    int sp = 13;
    double cp_start = 0.03;
    double cp_end = 0.01;
    int d_start = -1;
    int d_end = d_start;
  if(false){
    printf("Usage: ./project stepsExp2 ts_d te_d ts_Cprob te_Cprob batchExp2\n");
    printf("\n");
    printf("Any number of arguments can be omitted. The first values will be considered.\n");
    printf("\n");
    printf("The prefix ts represents starting temperature.\n");
    printf("The prefix te represents ending temperature.\n");
    printf("The Cprob is the complementar probability\n");
    printf("The d is the delta, so usually parameterized as negative.\n");
    printf("\n");
    printf("stepsExp2 is the power of 2 exponent for the number of steps\n");
    printf("batchExp2 is the power of 2 exponent for the batch size\n");
    printf("\n");
    printf("Defaults:\n");
    printf("\n");
    printf("stepsExp2 = %d\n", sp);
    printf("batchExp2 = %d\n", bp);
    printf("ts_Cprob = %.3f\n", cp_start);
    printf("ts_d = %d\n", d_start);
    printf("te_Cprob = %.3f\n", cp_end);
    printf("te_d = %d\n", d_end);
    printf("\n");
    printf("./project fname %d %d %.3f %d %.3f %d\n", sp, bp,
    cp_start, d_start, cp_end, d_end);
    printf("\n");
    return 0;
  } else {
    loadGraph();
    double iscore = setInitialState();
    if(d_start > -0.05*(double)iscore)
      d_start = -0.05*(double)iscore;
    d_end = d_start;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
    switch(argc){
    case 7:
      sscanf(argv[6], "%d", &bp);
    case 6:
      sscanf(argv[5], "%d", &sp);
    case 5:
      sscanf(argv[4], "%lf", &cp_end);
    case 4:
      sscanf(argv[3], "%lf", &cp_start);
    case 3:
      sscanf(argv[2], "%d", &d_end);
    case 2:
      sscanf(argv[1], "%d", &d_start);
    }
#pragma GCC diagnostic pop
#warning TODO: change to arc4randomuniform
    srandom(6174);
    startSigterm();
    uint goal = 0;
    switch(getFNV()){
#include "calibrate.h"
    }
    runSA(
   getTemperature(cp_start, d_start),
   getTemperature(cp_end, d_end),
   1<<bp,
   10*getSize(),
   goal
   );
     showMin();
    freeState();
    freeGraph();
  }
  return 0;
}
