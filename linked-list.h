#ifndef HEADER_GAURD_LIST
#define HEADER_GAURD_LIST

typedef struct Node Node;
typedef struct List List;

typedef double (* Gate)(List *);

typedef struct Node {
  
  double probability;

  Gate gate;
  List * children;
  Node * parent;
  
  Node * next;

  char * name;
  
} Node;

typedef struct List {
  
  Node * head;

  int elements;
  
} List;

Node * create_node(Gate gate, List * children);
Node * create_leaf(char * name, double probability);

List * create_list();

void list_insert(List * list, Node * node);

List * w_children(unsigned int vargs, ...);


#endif
