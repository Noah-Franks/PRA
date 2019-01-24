

#include <stdlib.h>

#include <stdio.h>

#include "linked-list.h"
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
    printf("\t%s: %E\n", parent -> name, parent -> probability);
    return parent -> probability;
  }
  
  // go through all children and evaluate them
  for (Node * child = children -> head; child; child = child -> next) {
    evaluate_tree(child);
  }
  
  // return the gate applied to the children list
  
  parent -> probability = (parent -> gate)(children);
  printf("Branch: " CONSOLE_YELLOW "%E\n" CONSOLE_RESET, parent -> probability);
  return parent -> probability;
}

int main() {
  
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
  
  
  printf("Failure probability: " CONSOLE_GREEN "%E\n" CONSOLE_RESET, evaluate_tree(root));
}
