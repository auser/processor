#include <string.h>
#include <string>   // STL string

#include "babysitter_utils.h"
#include "bee.h"

#ifndef COMB_PROCESS_H
#define COMB_PROCESS_H

/*** Defines ***/
#ifndef PID_ROOT
#define PID_ROOT "/tmp/beehive/pid"
#endif

#ifndef INITIAL_PROCESS_WAIT
#define INITIAL_PROCESS_WAIT 5
#endif
#ifndef LIL_BUF
#define LIL_BUF 100
#endif
#ifndef BIG_BUF
#define BIG_BUF 1024
#endif

#ifndef FATAL_ERROR
#define FATAL_ERROR fprintf
#endif

typedef enum _process_status_ {
	P_WAITING,
  P_RUNNING,
  P_STOPPING,
  P_KILLED
} process_status;

typedef void (*callback_t) (pid_t p, int sig);

// Definitions
void  gotsignal(int signal, siginfo_t* si, void* context);

class CombProcess {
private:
  callback_t      m_callback;                         // Callback after the process dies (process is restarted if no callback)
  unsigned long   m_sec, m_micro, m_nano;             // time to live times
  pid_t           m_process_pid;                      // The pid of the process who started this CombProcess
  pid_t           m_parent_pid;                       // The parent pid
  char*           m_name;                             // The name of the process
  process_status  m_status;                           // Status of the process
  char            m_pidroot[LIL_BUF];                 // The directory where the pid should be stored
  std::string     m_pidfile;                          // Pidfile name
  char            m_cd[BIG_BUF];                      // Working directory
  char            m_input[BIG_BUF];                   // Input line
  int             m_argc;                             // Number of arguments
  char**          m_argv;                             // A string list of the arguments
  const char**    m_cenv;                             // The string list of environment variables
  unsigned int    m_cenv_c;                           // The current count of the environment variables
  Bee*            m_bee;                              // Used to pass back a bee
  
  // Internal
  struct sigaction m_sact, m_sterm, m_signore;
  int             m_dbg;                              // Debug level
  
public:
  CombProcess(int dbg) {
    new (this) CombProcess;
    set_debug(dbg);
  }
  CombProcess() : m_callback(NULL),m_sec(5),m_micro(0),m_nano(0),m_process_pid(-1),m_name(NULL),m_status(P_WAITING), m_argc(0), m_cenv_c(0), m_bee(NULL),m_dbg(0) {
    m_pidfile = "";
    memset(m_cd, 0, BIG_BUF);
    memset(m_input, 0, BIG_BUF);
  }
  ~CombProcess() {
    debug(m_dbg, 2, "freeing CombProcess: %p\n", this);
    if (m_pidfile != "") unlink(m_pidfile);
    if (m_name != NULL) free(m_name);
    if (m_argv != NULL) free(m_argv);
    if (m_cenv != NULL) free(m_cenv);
  }
  
  Bee *bee()  {
    if (m_bee == NULL) {
      m_bee = new Bee(m_argv[0], m_process_pid);
    }
    return m_bee;
  }
  
  void set_callback(callback_t f)   {m_callback = f;}
  void set_secs(unsigned long s)    {m_sec = s;}
  void set_micro(unsigned long m)   {m_micro = m;}
  void set_nano(unsigned long n)    {m_nano = n;}
  void set_cd(const char* c)              {
    // m_cd = (char *)malloc(sizeof(char) * strlen(c));
    memset(m_cd, 0, strlen(c)); 
    strncpy(m_cd, c, strlen(c));
  }
  void set_debug(int d)             {m_dbg = d;}
  void set_pidroot(const char* c)   {
    memset(m_pidroot, 0, LIL_BUF); strncpy(m_pidroot, c, strlen(c));
  }
  void set_env(char **e)            {
    int total_len = 0, i = 0;
    while(e[i] != (char)'\0') {
      debug(m_dbg, 2, "e[%d] = %s\n", i, e[i]);
      total_len+=strlen((const char*)e[i]);
      i++;
    }
    
    m_cenv_c = i;
    
    total_len = total_len * sizeof(char);
    
    if ( (m_cenv = (const char**) malloc( total_len )) == NULL ) {
      fprintf(stderr, "Could not allocate a new char. Out of memory\n");
    } else {
      memset(m_cenv, 0, total_len);
      memcpy(m_cenv, e, total_len);
    }
  }
  void set_input(int i, char const *input[])     {
    m_argc = i;
    for (int j = 0; j < i; j++) debug(m_dbg, 2, "input[%d] = %s\n", j, input[j]);
    int tlen = sizeof(char*) * sizeof(input);
    m_argv = (char **) malloc( tlen );
    memset(m_argv, 0, tlen);
    memcpy(m_argv, input, tlen);
    m_name = strdup(m_argv[0]);
    debug(m_dbg, 2, "Prog name: %s\n", m_name);
    for (int j = 0; j < i; j++) debug(m_dbg, 2, "m_argv[%d] = %s\n", j, m_argv[j]);
  }
  
  pid_t monitored_start();
  pid_t monitored_start(const char *pidroot);
  pid_t monitored_start(int i, char const *argv[], char **envp);
  pid_t monitored_start(int i, char const *argv[], char **envp, const char *pidroot);
  
private:
  int start_process();
  int setup_signal_handlers();
  int safe_fork();
  int write_to_pidfile();
  void cleanup_exited(int);
  int process_is_dead_after_waiting(int sleep_time, int retries);
};

#endif