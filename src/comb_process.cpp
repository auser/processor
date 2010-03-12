/**** Includes *****/
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>

#include <sys/types.h>  // for kill() command
#include <signal.h>     // for kill() command
#include <sys/wait.h>   // for wait()
#include <stdlib.h>     // for setenv()
#include <time.h>       // for strftime()
#include <libgen.h>     // for basename()
#include <errno.h>

#include "comb_process.h"

extern int dbg;
int gbl_child_pid, gbl_parent_pid;
char *gbl_pidfile;
callback_t gbl_callback;

void gbl_cleanup_exited(int sig, int exit_code)
{	
	debug(dbg, 1, "Killing in %d\n", (int)getpid());
  if (gbl_callback != NULL) gbl_callback(gbl_child_pid, sig);
  debug(dbg, 1, "unlinking pidfile: %s\n", gbl_pidfile);
  unlink(gbl_pidfile);
  kill(gbl_child_pid, sig);
  debug(dbg, 1, "Getting the eff outta here: %d\n", (int)getpid());
}

// Signal handlers
// We've received a signal to process
void gotsignal(int signal, siginfo_t* si, void* context)
{
  debug(dbg, 1, "got signal: %d (%d)\n", signal, (unsigned int)si->si_pid);
  switch (signal) {
    case SIGCHLD:
      if (si->si_code == SI_USER || signal != SIGCHLD) return;
    case SIGTERM:
    case SIGINT:
    gbl_cleanup_exited(signal, 0);
    break;
    case SIGHUP:
    default:
    break;
  }
  waitpid(0, NULL, WNOHANG);
}

/***** Comb process *****/
/**
* Handle the signals to this process
*
* Note: Must use a "trampoline" function to send the 
* process to
**/
int CombProcess::setup_signal_handlers()
{
  // Ignore signals
  m_signore.sa_handler = SIG_IGN;
  sigemptyset(&m_signore.sa_mask);
  m_signore.sa_flags = 0;
  sigaction(SIGQUIT,  &m_signore, NULL); // ignore Quit
  sigaction(SIGABRT,  &m_signore, NULL); // ignore ABRT
  sigaction(SIGTSTP,  &m_signore, NULL);
  
  // SIGCHLD handler
  m_sterm.sa_handler = NULL;
  m_sterm.sa_sigaction = &gotsignal;
  sigemptyset(&m_sterm.sa_mask);
  m_sterm.sa_flags = 0;//m_sterm.sa_flags = SA_SIGINFO | SA_RESTART | SA_NOCLDSTOP | SA_NODEFER;
  
  if (sigaction(SIGINT,  &m_sterm, NULL) < 0) fprintf(stderr, "Error setting INT action\n"); // interrupts
  if (sigaction(SIGTERM, &m_sterm, NULL) < 0) fprintf(stderr, "Error setting TERM handler\n"); // ignore TERM
  if (sigaction(SIGHUP,  &m_sterm, NULL) < 0) fprintf(stderr, "Error setting HUP handler\n"); // ignore hangups
  if (sigaction(SIGCHLD,  &m_sterm, NULL) < 0) fprintf(stderr, "Error setting HUP handler\n"); // ignore hangups
  
  debug(m_dbg, 2, "Set up signal handlers\n");
  return 0;
}

/**
* Start the process
**/
void CombProcess::monitored_start(int argc, char const **argv, char **envp)
{
  set_input(argc, argv);
  set_env(envp);
  return monitored_start();
}
void CombProcess::monitored_start(int argc, char const **argv, char **envp, const char *pidroot)
{
  set_input(argc, argv);
  set_env(envp);
  return monitored_start(pidroot);
}

void CombProcess::monitored_start(){return monitored_start(PID_ROOT);}

int CombProcess::process_is_dead_after_waiting(int sleep_time, int retries)
{
  if (retries < 1) return 1; // Not alive
  int process_is_down = kill(m_process_pid, 0);
  
  if (!process_is_down)
    return 0; // It IS alive
    
  usleep(sleep_time); // Wait for it to sleep a little while more
  return process_is_dead_after_waiting(sleep_time, retries - 1);
}

// Entry point
void CombProcess::monitored_start(const char *pidroot)
{
  setsid();
  // Save the pidroot
  set_pidroot(pidroot);
  // Save the callbacks
  gbl_callback = m_callback;

  // Ensure the pid root exists
  mkdir_p(m_pidroot);

  // Setup the signals for the process to follow
  setup_signal_handlers();

  // Start the process and wait for it to start up. 
  start_process();
  // Let's make sure it didn't die before we continue
  if (m_process_pid < 0) {
    fprintf(stderr, "[FATAL ERROR] Failed to start the process\n");
    exit(-1);
  }
  
  // Wait
  sleep(INITIAL_PROCESS_WAIT);
  if (process_is_dead_after_waiting(INITIAL_PROCESS_WAIT, 3)) return;
  
  write_to_pidfile();
  // Copy to the global pid file for safe-keeping
  gbl_pidfile = (char *)malloc(sizeof(char) * (m_pidfile.length()));
  memset(gbl_pidfile, 0, sizeof(char) * m_pidfile.length()); 
  strncpy(gbl_pidfile, m_pidfile.c_str(), m_pidfile.length());  
  gbl_pidfile[m_pidfile.length()] = '\0';
}

void CombProcess::cleanup_exited(int exit_code){gbl_cleanup_exited(SIGINT, exit_code);}

int CombProcess::start_process()
{
  pid_t child_pid = -1;
  switch(child_pid = fork()) {
    case -1:
      fprintf(stderr, "Could not fork in process. Fatal error in CombProcess::start(): %s\n", ::strerror(errno));
      _exit(errno);
      break;
    case 0:
      // We are in the child process
      debug(m_dbg, 1, "Starting comb with the command: %s\n", m_argv[0]);
      
      // cd into the directory if there is one
      if (m_cd[0] != (char)'\0') {
        if (chdir(m_cd)) {
          perror("chdir");
        }
      }
      
      execve(m_argv[0], (char* const*)m_argv, (char* const*)m_cenv);
      
      // If execlp returns than there is some serious error !! And
      // executes the following lines below...
      fprintf(stderr, "Error: Unable to start child process: %s\n", strerror(errno));
      exit(-1);
      break;
    default:
      if (child_pid < 0) fprintf(stderr, "\nFatal Error: Problem while starting child process\n");
      m_process_pid = gbl_child_pid = child_pid;
  }
  debug(m_dbg, 1, "Child pid in start_process %d\n", (int)child_pid);
  return child_pid;
}

/***** Utils *****/

/**
* Write pid to pid file
**/
int CombProcess::write_to_pidfile()
{
  // Buffer
  char buf[BIG_BUF];
  memset(buf, 0, BIG_BUF);snprintf(buf, BIG_BUF, "%s/%d.pid", m_pidroot, (unsigned int)m_process_pid);
  m_pidfile = buf;
  
  // Copy to the global pid file for safe-keeping
  gbl_pidfile = (char *)malloc(sizeof(char) * strlen(buf));
  memset(gbl_pidfile, 0, sizeof(char) * strlen(buf)); 
  strncpy(gbl_pidfile, buf, strlen(buf));
  
  memset(buf, 0, BIG_BUF);snprintf(buf, BIG_BUF, "%d", (unsigned int)m_process_pid);
  
  printf("buf: %s\n", buf);
  
  // Open file
  debug(m_dbg, 2, "Opening pidfile: %s and writing %s to it\n", m_pidfile.c_str(), buf);
  FILE *pFile = fopen(m_pidfile.c_str(), "w");
  if ((ssize_t)fwrite(buf, 1, strlen(buf), pFile) != (ssize_t)strlen(buf)) {
    fprintf(stderr, "Could not write to pid file (%s): %s\n", m_pidfile.c_str(), strerror(errno));
    fclose(pFile);
    _exit(-1);
  }
  fclose(pFile);
  debug(m_dbg, 2, "Closed pidfile: %s and wrote %s to it\n", m_pidfile.c_str(), buf);
  return 0;
}