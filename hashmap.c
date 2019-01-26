

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "math.h"
#include "linked-list.h"
#include "hashmap.h"
#include "monte-carlo.h"

// Note: you may notice some functions would be shorter and less repetative
// if written via calls to the other functions. While I would usually do this,
// hashing is expensive, so I've done that as few times as possible.


Hashmap * create_hashmap(hash_function hash, key_comparator key_diff, int expected_size) {
  // Creates a hashmap with starting size as a function of expectations
  
  Hashmap * map = malloc(sizeof(Hashmap));
  
  map -> elements = 0;
  map -> size = expected_size * 2;
  
  map -> hash = hash;
  map -> key_diff = key_diff;
  
  map -> table = malloc(map -> size * sizeof(List **));
  
  // Create table of singlely linked lists
  for (int i = 0; i < map -> size; i++) {
    map -> table[i] = create_list();
  }
  
  return map;
}

int hash_string(void * string, int upper_bound) {
  // Hashes a string into a range using java's algorithm
  // h(x) has been chosen to minimize collisions
  // Note, hx will overflow but this is no problem

  int hx = 0;
  for (int i = 0; i < strlen((char *) string); i++) {
    hx = hx * 31 + ((char *) string)[i];
  }
  
  return abs(hx) % upper_bound;
}

int compare_strings(void * first, void * other) {
  return strcmp(first, other);
}

void * hashmap_get(Hashmap * this, void * key) {
  // Gets element from hashmap

  int hx = this -> hash(key, this -> size);

  List * list = this -> table[hx];

  // Ensure list isn't empty
  if (!this -> table[hx] -> elements) {
    abort();
    printf("Tried to retrieve element that does not exist\n");
    exit(1);
  }

  for (Node * node = list -> head; node; node = node -> next) {
    
    char * node_key = ((HashmapElement *) node -> value) -> key;
    
    //if (strcmp(string, key)) continue;
    if (this -> key_diff(key, node_key)) continue;

    // string matches key
    return ((HashmapElement *) node -> value) -> datum;
  }

  // No match was found in the matching table list
  abort();
  printf("Tried to retrieve element that does not exist\n");
  exit(1);
}

void hashmap_add(Hashmap * this, void * key, void * datum) {
  // Adds an element to the hashmap
  
  int hx = this -> hash(key, this -> size);
  
  HashmapElement * element = malloc(sizeof(HashmapElement));
  element -> key = key;
  element -> datum = datum;
  
  list_insert(this -> table[hx], create_basic_node((void *) element));
  
  this -> elements++;
}

bool hashmap_exists(Hashmap * this, void * key) {
  
  // Gets element from hashmap
  
  int hx = this -> hash(key, this -> size);
  
  List * list = this -> table[hx];
  
  // Check if list is empty
  if (!this -> table[hx] -> elements) return false;    
  
  for (Node * node = list -> head; node; node = node -> next) {
    
    void * node_key = ((HashmapElement *) node -> value) -> key;
    
    //if (strcmp(string, key)) continue;
    if (this -> key_diff(key, node_key)) continue;
    
    // string matches key
    return true;
  }
  
  return false;
}

void init_hashmap() {

  simulation = create_hashmap(hash_string, compare_strings, 16);
  
  leaf_simulacrons = create_list();
}
