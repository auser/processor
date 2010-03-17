#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>             // For timeval struct
#include <time.h>                 // For time function
#include <string>
#include <map>
#include <deque>
#include <pwd.h>        /* getpwdid */

/* Readline */
#include <readline/readline.h>
#include <readline/history.h>
#include <readline/tilde.h>

#include "babysitter_utils.h"
#include "fs.h"
#include "string_utils.h"
#include "time.h"
#include "printf.h"

#include "command_info.h"
extern MapChildrenT children;

int main (int argc, const char *argv[])
{
  setup_signal_handlers();
  setup_defaults();
  
  const char* env[] = { "PLATFORM_HOST=beehive", NULL };
  
  process_pid = (int)getpid();
  
  // drop_into_shell();  
  static char *line = (char *)NULL;
  char *cmd_buf;
  int terminated = 0;
  
  while (!terminated) {
    
    while (!terminated && (exited_children.size() > 0 || signaled)) check_children(terminated);
    check_pending(); // Check for pending signals arrived while we were in the signal handler
    if (terminated) break;
    
    // Read the next command
    line = readline(PROMPT_STR);
    int result;
    result = history_expand(line, &cmd_buf);
        
    if (result < 0 || result == 2) fprintf(stderr, "%s\n", cmd_buf);
    else { add_history(cmd_buf); }

    cmd_buf[ strlen(cmd_buf) ] = '\0';

    if ( !strncmp(cmd_buf, "quit", 4) || !strncmp(cmd_buf, "exit", 4) ) break; // Get the hell outta here
    
    // Gather args        
    char **command_argv = {0};
    int command_argc = 0;
    if ((command_argc = argify(cmd_buf, &command_argv)) < 1) {
      continue; // Ignore blanks
    }
    
    for (int i = 0; i < command_argc; i++) printf("command_argv[%d] = %s\n", i, command_argv[i]);

    if ( !strncmp(command_argv[0], "help", 4) ) {
      print_help();
    } else if ( !strncmp(command_argv[0], "list", 4) ) {
      list_processes();
    } else if ( !strncmp(command_argv[0], "start", 5) ) {
      if (command_argc < 2) {
        fprintf(stderr, "usage: start [command]\n");
      } else {
        command_argv++;command_argc--;
        // For example: start ./comb_test.sh
        command_argv[command_argc] = 0; // NULL TERMINATE IT
        const char *cd = NULL;
        // start_child(const char* cmd, const char* cd, char* const* env, int user, int nice)
        pid_t pid = start_child(command_argc, (const char**)command_argv, cd, (const char**)env, run_as_user, 0);
        //CmdInfo(const char* _cmd, const char* _kill_cmd, pid_t _cmd_pid)
        CmdInfo ci(*command_argv, "", pid);
        children[pid] = ci;
      }
    } else if ( !strncmp(command_argv[0], "kill", 4) ) {
      if (command_argc < 2){
        if (command_argc < 2) {
          fprintf(stderr, "usage: kill [pid]\n");
        } else {
          pid_t kill_pid = atoi(command_argv[1]);
          time_t now = time (NULL);
          printf("can you kill_pid: %d\n", kill_pid);
          stop_child(kill_pid, 0, now);
        }
      }
    } else {
      printf("Unknown command: %s\ntype 'help' for available commands\n", cmd_buf);
    }
    
    for (int i = 0; i < command_argc; i++) free(command_argv[i]);
    free(cmd_buf);
    free(line);
  }
  
  printf("Exiting... killing all processes...\n");
  terminate_all();
  return 0;
}