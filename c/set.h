#include <stdint.h>
#include <stdlib.h>

typedef struct _setnode {
  uint64_t data;
  struct _setnode *left;
  struct _setnode *right;
} SetNode;

#define ADDED_OK 0
#define ADDED_NOMEM 1
#define DUPLICATE 2

int AddToSet(SetNode **Target, uint64_t data);


