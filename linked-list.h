#ifndef HEADER_GAURD_LIST
#define HEADER_GAURD_LIST

typedef struct Node Node;
typedef struct List List;

#include "monte-carlo.h"

typedef double (* Gate)(List *);

typedef struct Node {
  
  union {
    
    // when serving as node in tree
    struct {
      double probability;
      
      Gate gate;
      List * children;
      Node * parent;
      
      char * name;

      int level;
      
      Simulacron * simulacron;
      
      bool leaf;
    };
    
    // when serving as node in hashmap
    struct {
      void * value;
    };
  };
  
  Node * next;
  
} Node;

typedef struct List {
  
  Node * head;
  
  int elements;
  
} List;

Node * create_node(Gate gate, List * children);
Node * create_leaf(char * name, double probability);

Node * create_basic_node(void * value);    // useful for hashmaps

List * create_list();

void name_branches(Node * root, int level);

void list_insert(List * list, Node * node);

List * w_children(unsigned int vargs, ...);


#endif
