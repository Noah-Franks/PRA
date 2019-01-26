
#include <stdlib.h>
#include <stdio.h>

#include "monte-carlo.h"
#include "tree.h"
#include "linked-list.h"
#include "hashmap.h"
#include "colors.h"

Simulacron * create_simulacron(Node * node, bool simulating) {
  // creates a simulacron, a tiny piece of a simulation.
  // simulacrons are used to store partial evaluation in the tree.
  
  Simulacron * simulacron = malloc(sizeof(Simulacron));
  
  simulacron -> failures  = 0;
  simulacron -> trials    = 0;
  simulacron -> failed    = false;
  simulacron -> evaluated = false;
  simulacron -> probability = 0.0;    // for branches, this grows (only used for advanced simulations)
  
  if (node -> leaf) {
    simulacron -> threshold = (long) (1.0 / node -> probability);
    printf("Leaf " CONSOLE_BLUE  "%s" CONSOLE_RESET" added to simulation with threshold "
                   CONSOLE_BLUE "%ld" CONSOLE_RESET " \n",
           node -> name, simulacron -> threshold);
    
    simulacron -> probability = node -> probability;
  }
  
  return simulacron;
}

void prepare_tree(Node * parent, bool simulating) {
  
  if (parent -> leaf) {
    
    if (!hashmap_exists(simulation, (void *) parent -> name)) {

      // first time encountering a leaf with this name,
      // so we'll add a new one to the map

      Simulacron * simulacron = create_simulacron(parent, simulating);
      
      list_insert(leaf_simulacrons, create_basic_node((void *) simulacron));
      
      parent -> simulacron = simulacron;
      
      hashmap_add(simulation, (void *) parent -> name, parent -> simulacron);
      return;
    }
    
    // we've seen a node with this name before,
    // so retrieve the proper simulacron
    
    parent -> simulacron = (Simulacron *) hashmap_get(simulation, (void *) parent -> name);
    return;
  }

  // not a leaf, so names don't matter
  parent -> simulacron = create_simulacron(parent, simulating);
  
  List * list = parent -> children;
  
  for (Node * child = list -> head; child; child = child -> next) {
    prepare_tree(child, simulating);
  }
}

void monte_trial(Node * parent) {
  // recursively accomplishes a Monte Carlo trial.
  // each leaf is assigned failure with it's designated probability, with
  // branches failing via a function of their children.
  
  // note that this means the vast, vast majority of trials won't result in failure.
  // for this reason, other methods should be considered that simulate differently.
  
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
  
  for (Node * child = parent -> children -> head; child; child = child -> next)
    monte_clean(child);
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




double trial_select() {
  // instead of choosing failure with probability equal to the node's chance of failure,
  // get the trial probability of an equiprobablely selected set of failure states.
  // if this works, this should significantly reduce the number of trials necessary.
  
  double trial_likelihood = 1.0;
  
  for (Node * node = leaf_simulacrons -> head; node; node = node -> next) {
    
    Simulacron * sim = (Simulacron *) node -> value;
    
    sim -> failed = (!(rand() % 2));
    
    trial_likelihood *= (sim -> failed) * (sim -> probability) + (!sim -> failed) * (1 - sim -> probability);
  }
  
  return trial_likelihood;
}

void monte_advanced_walk(Node * parent, double trial_likelihood) {
  // walks the tree, updating probabilities considering the trial likelihood
  
  if (parent -> leaf) return;    // nodes have been already given states from trial_select()
  
  // get children values before parent calculation, since it depends on these
  int child_failures = 0;
  
  for (Node * child = parent -> children -> head; child; child = child -> next) {
    
    monte_advanced_walk(child, trial_likelihood);
    
    if (child -> simulacron -> failed) child_failures++;
  }
  
  
  if (parent -> gate ==  or_gate) {    // OR gate, so if any child failed, this one failed
    
    if (child_failures) {
      parent -> simulacron -> failed       = true;
      parent -> simulacron -> probability += trial_likelihood;
      return;
    }
  }
  if (parent -> gate == and_gate) {    // AND gate, so all need to have failed for this one to fail

    if (child_failures == parent -> children -> elements) {
      parent -> simulacron -> failed       = true;
      parent -> simulacron -> probability += trial_likelihood;
      return;
    }
  }
}

void monte_advanced_print(Node * parent, double integral) {
  
  if (parent -> leaf) {
    for (int i = 0; i < parent -> level; i++) printf("\t");
    printf("%E                %s\n", parent -> probability, parent -> name);
    return;
  }
  
  for (Node * child = parent -> children -> head; child; child = child -> next) {
    monte_advanced_print(child, integral);
  }
  
  double percent_failed = (parent -> simulacron -> probability) / integral;
  
  for (int i = 0; i < parent -> level; i++) printf("\t");
  printf(CONSOLE_CYAN "%E                %s\n" CONSOLE_RESET, percent_failed, parent -> name);
}


double monte_carlo_advanced(Node * root, long trials) {
  
  double likelihood_integral = 0.0;
  
  for (long t = 0; t < trials; t++) {
    
    double trial_likelihood = trial_select();    // has side effect of picking states
    
    monte_advanced_walk(root, trial_likelihood);

    likelihood_integral += trial_likelihood;

    monte_clean(root);
  }
  
  monte_advanced_print(root, likelihood_integral);

  return (root -> simulacron -> probability) / likelihood_integral;
}



double exaustive_calculation(Node * root) {

  int num_states = 1;
  for (int s = 0; s < leaf_simulacrons -> elements; s++)
    num_states *= 2;
  
  for (int s = 0; s < num_states; s++) {

    double trial_likelihood = 1.0;
    
    int l = 0;
    
    for (Node * leaf = leaf_simulacrons -> head; leaf; leaf = leaf -> next) {
      
      Simulacron * sim = (Simulacron *) (leaf -> value);
      
      sim -> failed = (s & (1 << l)) >> l++;

      trial_likelihood *= (sim -> failed) * (sim -> probability) + (!sim -> failed) * (1 - sim -> probability);
    }
    
    monte_advanced_walk(root, trial_likelihood);

    monte_clean(root);
    
  }
  
  monte_advanced_print(root, 1.0);

  
  
  return (root -> simulacron -> probability);
}
