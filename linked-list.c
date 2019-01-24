

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "linked-list.h"


Node * create_node(Gate gate, List * children) {

  Node * node = malloc(sizeof(Node));

  node -> gate     = gate;
  
  node -> children = children;
  node -> parent   = NULL;
  node -> next     = NULL;
  
  node -> probability = 0;
  
  return node;
}

Node * create_leaf(char * name, double probability) {

  Node * node = malloc(sizeof(Node));

  node -> probability = probability;
  node -> name = name;

  node -> children = NULL;
  node -> parent = NULL;
  node -> next = NULL;

  node -> gate = NULL;
  
  return node;
}

List * create_list() {
  
  List * list = malloc(sizeof(List));

  list -> head = NULL;
  list -> elements = 0;

  return list;
}

void list_insert(List * list, Node * node) {

  list -> elements++;
  
  if (list -> head == NULL) {
    list -> head = node;
    return;
  }

  node -> next = list -> head;
  list -> head = node;
}

List * w_children(unsigned int vargs, ...) {
  
  List * list = create_list();

  va_list argPointer;
  
  va_start(argPointer, vargs);
  
  for (int i = 0; i < vargs; i++) {
    
    Node * child = va_arg(argPointer, Node *);
    
    list_insert(list, child);
  }
  
  va_end(argPointer);

  return list;
}
