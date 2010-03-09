#ifndef BEE_H
#define BEE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "bee.h"

typedef enum _bee_status_ {
  BEE_RUNNING,
  BEE_STOPPED,
  BEE_KILLED
} bee_status;

/**
 * Bee
 **/
class Bee {
public:
  pid_t           m_pid;        // Pid of the custom kill command
  std::string     m_name;         // Name of the command
  // ei::TimeVal     deadline;       // Time when the <m_pid> is supposed to be killed using SIGTERM.
  bool            sigterm;        // <true> if sigterm was issued.
  bool            sigkill;        // <true> if sigkill was issued.
  bee_status      m_status;         // Status of the bee

  Bee() : m_pid(-1), sigterm(false), sigkill(false), m_status(BEE_RUNNING) {}
  ~Bee() {}
  
  Bee(std::string name, pid_t _m_pid) {
    new(this) Bee();
    m_pid = _m_pid;
    m_name = name;
  }
  
  // Accessors
  const char*   name()    const { return m_name.c_str(); }
  const char*   status()  const {
    switch(m_status) {
      case BEE_RUNNING:
      return "running";
      break;
      case BEE_STOPPED:
      return "stopped";
      break;
      case BEE_KILLED:
      return "killed";
      break;
      default:
      return "unknown";
      break;
    }
  }
  
  // Setters
  void set_status(bee_status s) {m_status = s;}
  
public:
  int stop();
};

/*--------------------------------------------------------------------------*/

#endif