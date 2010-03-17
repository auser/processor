#include "time.h"

double timediff(time_t starttime, time_t finishtime) {
  return difftime(starttime, finishtime);
}
