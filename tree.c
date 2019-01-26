

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
  prepare_for_simulation(root);
  printf("\nSimulation will use %ld trials\n", trials);
  
  monte_carlo(root, trials);
  
  printf("\n\n");
}

int main() {
  
  init_hashmap();    // sets up probability map for Monte Carlo simulations
  
  
  Node * root = create_node(and_gate,
                            w_children(2,
                                       create_node(or_gate,
                                                   w_children(3,
                                                              create_leaf("λ", 1E-6),
                                                              create_leaf("B", 3E-6),
                                                              create_leaf("C", 4E-6)
                                                              )
                                                   ),
                                       create_node(and_gate,
                                                   w_children(2,
                                                              create_leaf("λ", 1E-6),
                                                              create_leaf("D", 2E-6)
                                                              )
                                                   )
                                       )
                            );
  
  name_branches(root, 0);    // give names to branches
  
  // evaluate assuming leaves are independent
  independent_evaluation(root);
  
  // run a Monte Carlo simulation
  dependent_evaluation(root, 1E9);
}
