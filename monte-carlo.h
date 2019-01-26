#ifndef HEADER_GAURD_MONTE_CARLO
#define HEADER_GAURD_MONTE_CARLO

#include <stdbool.h>

typedef struct Simulacron Simulacron;

#include "hashmap.h"
#include "linked-list.h"

typedef struct Simulacron {
  
  long threshold;
  
  long failures;
  long trials;

  bool failed;    // current state during evaluation
  bool evaluated;
  
} Simulacron;

Hashmap * simulation;

Simulacron * create_simulacron(Node * node);

void prepare_for_simulation(Node * root);

void monte_carlo(Node * root, long trials);

#endif
