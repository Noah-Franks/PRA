
#include <stdlib.h>
#include <stdio.h>

#include "monte-carlo.h"
#include "tree.h"
#include "linked-list.h"
#include "hashmap.h"
#include "colors.h"

Simulacron * create_simulacron(Node * node) {
  
  Simulacron * simulacron = malloc(sizeof(Simulacron));
  
  simulacron -> failures = 0;
  simulacron -> trials = 0;
  simulacron -> failed = false;
  simulacron -> evaluated = false;
  
  if (node -> leaf) {
    simulacron -> threshold = (long) (1.0 / node -> probability);
    printf("Leaf " CONSOLE_BLUE  "%s" CONSOLE_RESET" added to simulation with threshold "
                   CONSOLE_BLUE "%ld" CONSOLE_RESET " \n",
           node -> name, simulacron -> threshold);
  }
  
  return simulacron;
}

void prepare_for_simulation(Node * parent) {
  
  if (parent -> leaf) {
    
    if (!hashmap_exists(simulation, (void *) parent -> name)) {

      // first time encountering a leaf with this name,
      // so we'll add a new one to the map
      
      parent -> simulacron = create_simulacron(parent);
      
      hashmap_add(simulation, (void *) parent -> name, parent -> simulacron);
      return;
    }

    // we've seen a node with this name before,
    // so retrieve the proper simulacron
    
    parent -> simulacron = (Simulacron *) hashmap_get(simulation, (void *) parent -> name);
    return;
  }

  // not a leaf, so names don't matter
  parent -> simulacron = create_simulacron(parent);
  
  List * list = parent -> children;
  
  for (Node * child = list -> head; child; child = child -> next) {
    prepare_for_simulation(child);
  }
}

void monte_trial(Node * parent) {
  
  Simulacron * simulacron = parent -> simulacron;
  
  if (parent -> leaf) {

    if (simulacron -> evaluated) return;    // we've already done this node
    
    bool node_fails = ((rand() % (simulacron -> threshold)) == 0);
    
    if (node_fails) {
      simulacron -> failed = true;
      simulacron -> failures++;
    }
    
    simulacron -> evaluated = true;    // this isn't checked for non-leaves btw
    simulacron -> trials++;
    return;
  }

  simulacron -> trials++;
  
  int child_failures = 0;
  
  for (Node * child = parent -> children -> head; child; child = child -> next) {
    
    monte_trial(child);
    
    if (child -> simulacron -> failed) child_failures++;
  }
  
  if (parent -> gate == or_gate) {
    // OR gate, so if any child failed, this one failed
    
    if (child_failures) {
      simulacron -> failed = true;
      simulacron -> failures++;
      return;
    }
  }
  if (parent -> gate == and_gate) {
    // AND gate, so all need to have failed for this one to fail

    if (child_failures == parent -> children -> elements) {
      simulacron -> failed = true;
      simulacron -> failures++;
      return;
    }
  }
}

void monte_clean(Node * parent) {
  
  parent -> simulacron -> failed    = false;
  parent -> simulacron -> evaluated = false;
  
  if (parent -> leaf) return;
  
  for (Node * child = parent -> children -> head; child; child = child -> next) {
    monte_clean(child);
  }
}

void monte_print(Node * parent) {

  double percent_failed = (double) (parent -> simulacron -> failures) / (parent -> simulacron -> trials);
  
  if (parent -> leaf) {
    for (int i = 0; i < parent -> level; i++) printf("\t");
    printf("%E                %s\n", percent_failed, parent -> name);
    return;
  }
  
  for (Node * child = parent -> children -> head; child; child = child -> next) {
    monte_print(child);
  }
  
  for (int i = 0; i < parent -> level; i++) printf("\t");
  printf("%E                %s\n", percent_failed, parent -> name);
}

void monte_carlo(Node * root, long trials) {
  
  for (long t = 0; t < trials; t++) {
    monte_trial(root);
    monte_clean(root);
  }

  monte_print(root);
}

