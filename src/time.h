#ifndef _TIME_H
#define _TIME_H

#include <time.h>
#include <sys/time.h>

/*      
  struct timeval {
    long tv_sec;         seconds 
    long tv_usec;   microseconds 
  };
*/

static inline double timediff(time_t starttime, time_t finishtime) {
  return difftime(starttime, finishtime);
}

#endif