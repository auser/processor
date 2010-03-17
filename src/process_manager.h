#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

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

// Types
typedef unsigned char byte;
typedef int   exit_status_t;
typedef pid_t kill_cmd_pid_t;
typedef std::pair<pid_t, exit_status_t>     PidStatusT;
typedef std::pair<pid_t, CmdInfo>           PidInfoT;
typedef std::map <pid_t, CmdInfo>           MapChildrenT;
typedef std::pair<kill_cmd_pid_t, pid_t>    KillPidStatusT;
typedef std::map <kill_cmd_pid_t, pid_t>    MapKillPidT;

// Signals
int process_child_signal(pid_t pid);
void pending_signals(int sig);
void gotsignal(int signal);
void gotsigchild(int signal, siginfo_t* si, void* context);
void setup_signal_handlers();

// Returns
int send_ok(int transId, pid_t pid = -1);
int send_pid_status_term(const PidStatusT& stat);
int send_error_str(int transId, bool asAtom, const char* fmt, ...);
int send_pid_list(int transId, const MapChildrenT& children);

// Management
pid_t start_child(int command_argc, const char** command_argv, const char *cd, const char** env, int user, int nice);
int stop_child(CmdInfo& ci, int transId, time_t &now, bool notify);
void stop_child(pid_t pid, int transId, time_t &now);
int kill_child(pid_t pid, int signal, int transId, bool notify);
void terminate_all();
void check_pending();
int check_children(int& isTerminated);

// Other
void setup_defaults();

#endif