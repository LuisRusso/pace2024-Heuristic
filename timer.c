#include <time.h>
#include "timer.h"
static clock_t Clock_Begin;
void
startTimer(void)
{
  Clock_Begin = clock();
}
double
getTime(void)
{
  return (double)(clock()-Clock_Begin)/ CLOCKS_PER_SEC;
}
