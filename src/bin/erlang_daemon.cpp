#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <time.h>                 // For time function
#include <sys/time.h>             // For timeval struct
#include <string>
#include <map>
#include <deque>
#include <pwd.h>        /* getpwdid */

#include "string_utils.h"
#include "process_manager.h"


int send_ok(int transId, pid_t pid) {
  printf("send_ok!\n");
  return 0;
}

int send_pid_status_term(const PidStatusT& stat) {return 0;}
int send_error_str(int transId, bool asAtom, const char* fmt, ...) {return 0;}
int send_pid_list(int transId, const MapChildrenT& children) {return 0;}

int main (int argc, char const *argv[])
{
  printf("in erlang main\n");
  return 0;
}