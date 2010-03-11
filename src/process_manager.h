#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <string>
#include <map>

#include "babysitter_utils.h"
#include "comb_process.h"
#include "bee.h"

class ProcessManager {

private:
  typedef std::map <pid_t, Bee>   MapChildrenT;
  MapChildrenT                    children;
  int                             dbg;
  char                            **command; // Ew
  struct sigaction                old_action;
  struct sigaction                sact, sterm;
  pid_t                           process_pid;

public:
  
}