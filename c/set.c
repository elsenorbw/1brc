#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "set.h"

int AddToSet(SetNode **Target, uint64_t data)
{
  int result = ADDED_OK;

  if(NULL == *Target)
  {
    // Found somewhere to add this one..
    *Target = malloc(sizeof **Target);
    if(NULL == *Target)
    {
      fprintf(stderr, "error: cannot malloc another SetNode\n");
      result = ADDED_NOMEM;
    }
    else
    {
      (*Target)->data = data;
       (*Target)->left = NULL;
       (*Target)->right = NULL;
    }
  } else {
    if(data == (*Target)->data)
    {
      result = DUPLICATE;
    }
    else if(data < (*Target)->data)
    {
      result = AddToSet(&(*Target)->left, data);
    }
    else
    {
      result = AddToSet(&(*Target)->right, data);
    }
  }

  return result;
}


