

#include <stdlib.h>

#include <stdio.h>

#include "linked-list.h"
#include "hashmap.h"

#include "tree.h"
#include "monte-carlo.h"
#include "colors.h"

double and_gate(List * children) {

  double probability = 1;
  
  for (Node * child = children -> head; child; child = child -> next)
    probability *= child -> probability;
  
  return probability;
}

double or_gate(List * children) {
  
  double product = 1;
  
  for (Node * child = children -> head; child; child = child -> next)
    product *= (1 - child -> probability);
  
  return 1 - product;
}

double evaluate_tree(Node * parent) {
  
  List * children = parent -> children;
  
  // if no children, we are at a leaf
  if (!children) {
    for (int i = 0; i < parent -> level; i++) printf("\t");
    printf("%E                    %s\n", parent -> probability, parent -> name);
    return parent -> probability;
  }
  
  // go through all children and evaluate them
  for (Node * child = children -> head; child; child = child -> next) {
    evaluate_tree(child);
  }
  
  // return the gate applied to the children list
  
  parent -> probability = (parent -> gate)(children);

  for (int i = 0; i < parent -> level; i++) printf("\t");
  printf(CONSOLE_YELLOW "%E                    %s\n" CONSOLE_RESET, parent -> probability, parent -> name);
  
  return parent -> probability;
}

void independent_evaluation(Node * root) {
  printf("Evaluating failure rates assuming all leaves are independent\n\n");
  printf("Bottom of tree\n\n");
  
  double mission_failure_probability = evaluate_tree(root);
  
  printf("Top of tree\n\n");
  printf("Mission failure probability: " CONSOLE_GREEN "%E\n" CONSOLE_RESET, mission_failure_probability);
  printf("\n\n");
}

void dependent_evaluation(Node * root, long trials) {
  
  printf("Discovering failure rates assuming leaves are dependent\n\n");
  
  prepare_tree(root, true);
  
  printf("\nSimulation will use %ld trials\n", trials);
  
  double mission_failure_probability = monte_carlo(root, trials);
  
  printf("\nMission failure probability: " CONSOLE_RED "%E\n" CONSOLE_RESET, mission_failure_probability);
  
  printf("\n\n");
}

void true_evaluation(Node * root) {
  
  printf("Exaustively calculating true probabilities\n\n");
  printf("Bottom of tree\n\n");
  
  prepare_tree(root, false);
  
  double mission_failure_probability = exaustive_calculation(root);

  printf("Top of tree\n\n");
  printf("\nMission failure probability: " CONSOLE_RED "%E\n" CONSOLE_RESET, mission_failure_probability);
}

#define crn create_node 
#define w_c w_children
#define and and_gate
#define or   or_gate

int main() {
  
  init_hashmap();    // sets up probability map for Monte Carlo simulations
  
  
  Node * root = crn(and,
                    w_c(2,
                        crn(or,
                            w_c(3,
                                create_leaf("λ", 1E-6),
                                create_leaf("C", 3E-6),
                                create_leaf("B", 4E-6)
                                )
                            ),
                        crn(and,
                            w_c(2,
                                create_leaf("λ", 1E-6),
                                create_leaf("A", 2E-6)
                                )
                            )
                        )
                    );
  
  
  // give names to branches
  name_branches(root, 0);
  
  
  // evaluate assuming leaves are independent
  independent_evaluation(root);
  
  // run a Monte Carlo simulation
  dependent_evaluation(root, 1E4);    // number of trials
  
  // calculate true probabilities exaustively
  true_evaluation(root);
}
