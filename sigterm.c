#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include "sigterm.h"
static volatile sig_atomic_t tle = 0;
static void term(__attribute__ ((unused)) int signum)
{
    tle = 1;
}
void
startSigterm(void)
{
  struct sigaction action;
  memset(&action, 0, sizeof(struct sigaction));
  action.sa_handler = term;
  sigaction(SIGTERM, &action, NULL);
}
bool
canContinue(void)
{
  return 0 == tle;
}
