#ifndef HEADER_GAURD_MONTE_CARLO
#define HEADER_GAURD_MONTE_CARLO

#include <stdbool.h>

typedef struct Simulacron Simulacron;

#include "hashmap.h"
#include "linked-list.h"

typedef struct Simulacron {
  
  long failures;         // total number of failures acumulated
  long trials;           // total number of trials this simulacron has undergone
  
  bool failed;           // current state during evaluation
  bool evaluated;        // whether this simulacron has been evaluated
  
  long threshold;        // inverse the probability of failure. Used for rand()
  
  // used for advanced simulation
  double probability;    // probability from leaf
   
} Simulacron;


Hashmap * simulation;         // map associating names to simulacrons

List * leaf_simulacrons;      // all leaf simulacrons (for advanced Monte Carlo)

void prepare_tree(Node * root, bool simulating);

double monte_carlo(Node * root, long trials);

double exaustive_calculation(Node * root);

#endif
