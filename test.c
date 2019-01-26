
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sched.h>

long get_time() {
  struct timespec time_period;
  
  clock_gettime(CLOCK_REALTIME, &time_period);
  
  return time_period.tv_sec;
}

int main() {
  
  long start_time = get_time();

  long simulations = 1E10;
  
  printf("Performing %E simulations\n", (double) simulations);
  
  for (long s = 0; s < simulations; s++) {
    rand();
  }
  
  
  long end_time = get_time();
  
  long duration = end_time - start_time;
  
  printf("Time %lds\n", duration);
  printf("Time per sample %f\n", (double) duration / simulations);
}
