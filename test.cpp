#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <string>
#include <map>

/* Readline */
#ifdef GNU_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#else
#include <editline/readline.h>
#endif /* GNU_READLINE */

#include "babysitter_utils.h"
#include "comb_process.h"
#include "bee.h"

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL 4
#endif

#ifndef PROMPT_STR
#define PROMPT_STR "bs >"
#endif

// Globals
typedef std::map <pid_t, Bee>   MapChildrenT;
MapChildrenT                    children;
int                             dbg = DEBUG_LEVEL;
struct sigaction                old_action;
struct sigaction                sact, sterm;
pid_t                           process_pid;

Bee* find_bee_by_pid(pid_t pid)
{
  for (MapChildrenT::iterator it=children.begin(), end=children.end(); it != end; ++it) {
    pid_t p = it->first;
    if (p == pid) return &it->second;
  }
  return NULL;
}

void list_processes()
{
  printf("Pid\tName\tStatus\n-----------------------\n");
  if (children.size() == 0) {
    return;
  }
  for(MapChildrenT::iterator it=children.begin(); it != children.end(); it++) 
    printf("%d\t%s\t%s\n",
      it->first, 
      it->second.name(), 
      it->second.status()
    );
}

void callback(pid_t pid, int sig)
{
  printf("callback called %d - %i (in %d / %d)\n", pid, sig, (int)getpid(), process_pid);
  list_processes();
  Bee *bee = find_bee_by_pid(pid);
  if (bee == NULL) return;
  switch(sig) {
    case SIGINT:
    case SIGTERM:
    bee->set_status(BEE_STOPPED);
    break;
    case SIGCHLD:
    bee->set_status(BEE_KILLED);
    break;
    default:
    printf("Received a signal...: %d\n", (int)sig);
    break;
  }
}

void terminate_by_pid(pid_t p)
{
  Bee *b;
  if ((b = find_bee_by_pid(p)) == NULL) return;
  
  b->set_status(BEE_STOPPED);
  kill(p, SIGTERM);
}

void terminate_all()
{
  for (MapChildrenT::iterator it=children.begin(), end=children.end(); it != end; ++it) {
    pid_t p = it->first;
    terminate_by_pid(p);
  }
}

void print_help()
{
  printf("Babysitter program help\n"
    "---Commands---\n"
    "h | help             Show this screen\n"
    "s | start <args>     Start a program\n"
    "b | bundle <starts>  Bundle a bee\n"
    "k | kill <args>      Stop a program\n"
    "l | list <args>      List the programs\n"
    "q | quit             Quit the daemon\n"
    "\n"
  );
}

void start(int argc, const char **argv, const char *env[])
{
  CombProcess p(DEBUG_LEVEL);
  // p.set_cd("/tmp/beehive/pid");
  p.set_callback(callback);
  p.set_secs(1);
  p.set_micro(2);

  p.monitored_start(argc, argv, (char **) env);
}

int main (int argc, const char *argv[])
{  
  const char* env[] = { "NAME=bob", NULL };
  
  process_pid = (int)getpid();
  
  // drop_into_shell();  
  char *cmd_buf;
  char *expansion;
  int result;
  
  while (1) {
    cmd_buf = readline(PROMPT_STR);
    result = history_expand(cmd_buf, &expansion);
    
    if (!strncmp(cmd_buf, "", sizeof(cmd_buf))) {
      continue;
    }
    if (result < 0 || result == 2) fprintf(stderr, "%s\n", expansion);
    else { add_history(expansion); }
    
    cmd_buf[ strlen(cmd_buf) ] = '\0';

    if ( !strncmp(cmd_buf, "quit", 4) || !strncmp(cmd_buf, "exit", 4) ) break; // Get the hell outta here
    
    // Gather args
    char **command_argv = {0};
    int command_argc = 0;
    
    argify(cmd_buf, &command_argc, &command_argv);
    
    // for (int i = 0; i < command_argc; i++) printf("command_argv[%d] = %s\n", i, command_argv[i]);
    
    if (command_argc <= 0) {
    } else {
      if ( !strncmp(command_argv[0], "help", 4) || !strncmp(command_argv[0], "h", 1) ) {
        print_help();
      } else if ( !strncmp(command_argv[0], "list", 4) || !strncmp(command_argv[0], "l", 1) ) {
        list_processes();
      } else if ( !strncmp(command_argv[0], "start", 5) || !strncmp(command_argv[0], "s", 1) ) {
        if (command_argc < 2) {
          fprintf(stderr, "usage: start [command]\n");
        } else {
          command_argv++; command_argc--;
          // For example: start ./comb_test.sh
          command_argv[command_argc] = 0; // NULL TERMINATE IT
          
          start(command_argc-1, (const char**)command_argv, env);
        }
      } else if ( !strncmp(command_argv[0], "kill", 4) || !strncmp(command_argv[0], "k", 1)) {
        printf("kill\n");
        terminate_by_pid(atoi( command_argv[1]) );
      } else {
        printf("Unknown command: %s\ntype 'help' for available commands\n", cmd_buf);
      }
    }
  }
  
  printf("Exiting... killing all processes...\n");
  terminate_all();
  return 0;
}