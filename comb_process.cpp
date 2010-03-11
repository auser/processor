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
void gotsignal(int sig)
{
  debug(dbg, 1, "got signal: %d\n", sig);
  switch (sig) {
    case SIGTERM:
    case SIGINT:
    case SIGCHLD:
    gbl_cleanup_exited(sig, 0);
    break;
    case SIGHUP:
    _exit(0);
    default:
    break;
  }
  _exit(0);
}

/**
 * We received a signal for the child pid process
 * Wait for the pid to exit if it hasn't only if the and it's an interrupt process
 * make sure the process get the pid and signal to it
 **/
/**
 * Got a signal from a child
 **/
void gotsigchild(int signal, siginfo_t* si, void* context)
{
  // If someone used kill() to send SIGCHLD ignore the event
  if (si->si_code == SI_USER || signal != SIGCHLD) return;
  
  debug(dbg, 1, "CombProcess %d exited (sig=%d)\r\n", si->si_pid, signal);
  // gbl_cleanup_exited((int)si->si_pid, signal);
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
  sigaction(SIGCHLD,  &m_signore, NULL);
  
  // Termination signals to handle with gotsignal
  m_sterm.sa_handler = gotsignal;
  sigemptyset(&m_sterm.sa_mask);
  sigaddset(&m_sterm.sa_mask, SIGCHLD);
  m_sterm.sa_flags = 0;
  
  if (sigaction(SIGINT,  &m_sterm, NULL) < 0) fprintf(stderr, "Error setting INT action\n"); // interrupts
  if (sigaction(SIGTERM, &m_sterm, NULL) < 0) fprintf(stderr, "Error setting TERM handler\n"); // ignore TERM
  if (sigaction(SIGHUP,  &m_sterm, NULL) < 0) fprintf(stderr, "Error setting HUP handler\n"); // ignore hangups

  // SIGCHLD handler
  m_sact.sa_handler = NULL;
  m_sact.sa_sigaction = &gotsigchild;
  sigemptyset(&m_sact.sa_mask);
  m_sact.sa_flags = SA_SIGINFO | SA_RESTART | SA_NOCLDSTOP | SA_NODEFER;
  sigaction(SIGCHLD, &m_sact, NULL);
  
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

// Entry point
void CombProcess::monitored_start(const char *pidroot)
{
  pid_t pid;
  // Detach from the main process please
  pid = fork();
  if (pid == 0) {
    // We are in the child!
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

    // Copy to the global pid file for safe-keeping
    gbl_pidfile = (char *)malloc(sizeof(char) * strlen(m_pidfile.c_str()));
    memset(gbl_pidfile, 0, sizeof(char) * strlen(m_pidfile.c_str())); 
    strncpy(gbl_pidfile, m_pidfile.c_str(), strlen(m_pidfile.c_str()));

    debug(m_dbg, 1, "Worker process started with a pid of: %d\n", m_process_pid);
    // sleep(INITIAL_PROCESS_WAIT); // Give the process a some time to start

    // Let's make sure it didn't die before we continue
    if (m_process_pid < 0) {
      fprintf(stderr, "[FATAL ERROR] Failed to start the process\n");
      exit(-1);
    }

    // And we're done!
  } else {
    while (waitpid(pid, NULL, 0) == -1 && errno != ECHILD);
    exit(0);
  }
  
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
      child_pid = -2;
      break;
    default:
      if (child_pid < 0) fprintf(stderr, "\nFatal Error: Problem while starting child process\n");
      m_process_pid = gbl_child_pid = child_pid;
      m_process_pid = child_pid;      
      write_to_pidfile();
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