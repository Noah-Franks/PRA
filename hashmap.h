#ifndef HEADER_GAURD_HASHMAP
#define HEADER_GAURD_HASHMAP

#include <stdbool.h>

#include "linked-list.h"
#include "types.h"

typedef int (*  hash_function)(void *, int upper_bound);
typedef int (* key_comparator)(void * first, void * other);

typedef struct HashmapElement {

  void * key;     // The key used to access this value   - might need to free this?
  void * datum;   // The content this element points to

} HashmapElement;

// Forward promise Hashmap's definition to compiler
typedef struct Hashmap Hashmap;

// Structure for hashmap
typedef struct Hashmap {
  
  uint elements;
  uint size;
  
  List ** table;
  
  hash_function hash;
  key_comparator key_diff;
  
} Hashmap;

Hashmap * create_hashmap(hash_function hash, key_comparator key_diff, int expected_size);

int hash_string(void *  string, int upper_bound);    // (char *, int)

int compare_strings(void * first, void * other);     // (char *, char *)

void * hashmap_get   (Hashmap * this, void * key);
void   hashmap_add   (Hashmap * this, void * key, void * datum);
bool   hashmap_exists(Hashmap * this, void * key);

void init_hashmap();

#endif
