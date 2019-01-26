

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

#include "linked-list.h"
#include "hashmap.h"

#include "monte-carlo.h"
#include "colors.h"


Node * create_node(Gate gate, List * children) {

  Node * node = malloc(sizeof(Node));

  node -> gate     = gate;
  
  node -> children = children;
  node -> parent   = NULL;
  node -> next     = NULL;
  
  node -> leaf = false;
  
  node -> probability = 0;
  
  return node;
}

Node * create_leaf(char * name, double probability) {
  
  Node * node = malloc(sizeof(Node));
  
  node -> probability = probability;
  node -> name = name;
  
  node -> leaf = true;

  node -> children   = NULL;
  node -> parent     = NULL;
  node -> next       = NULL;
  node -> simulacron = NULL;
  node -> gate       = NULL;
  
  return node;
}

Node * create_basic_node(void * value) {
  // creates a basic node consisting of a value and a next pointer
  // this is useful for creating hashmaps
  
  Node * node = malloc(sizeof(Node));

  node -> value = value;
  node -> next  = NULL;

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

void name_branches(Node * parent, int level) {
  // recursively names the branches of the tree while assigning them levels.
  // this is useful for printing the tree out in a meaningful manner, as spacing
  // can be used to denote the heigherarchy.

  parent -> level = level;    // stack get's level in tree
  
  List * list = parent -> children;

  if (!list) return;    // node is a leaf, so it already has a name
  
  char name_buffer[1024];
  name_buffer[0] = '\0';
  
  for (Node * child = list -> head; child; child = child -> next) {
    name_branches(child, level + 1);

    if (name_buffer[0] == '\0') sprintf(name_buffer, "%s"   ,              child -> name);
    else                        sprintf(name_buffer, "%s-%s", name_buffer, child -> name);
    
  }
  
  parent -> name = strdup(name_buffer);
}
