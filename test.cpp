#include <stdio.h>

#include "babysitter_utils.h"
#include "comb_process.h"

int dbg;
struct sigaction old_action;
struct sigaction sact, sterm;

void callback(int p)
{
  printf("callback called!\n");
}

int main (int argc, char const *argv[])
{
  dbg = 4;
  CombProcess p(dbg);
  
  // p.set_cd("/tmp/beehive/pid");
  p.set_callback(callback);
  p.set_secs(0);
  p.set_micro(2);
  
  const char* env[] = { "NAME=bob", NULL };
  
  argv++; // We don't want our program name to be included
  for(int i = 0; i < argc; i++)
    printf("argv[%d] = %s\n", i, argv[i]);
  
  pid_t pid = p.monitored_start(argc-1, argv, (char **) env);
  
  printf("monitored_start pid: %d\n", (int)pid);
  
  return 0;
}