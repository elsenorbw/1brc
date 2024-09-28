#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"
#include "set.h"


#define SUCCESS 0
#define CANNOT_OPEN_FILE 1
#define HAS_CLASHES 2

#define LOCATIONS_FILENAME "locations.txt"


int main(void)
{
  FILE *fp = NULL;
  int ErrorStatus = SUCCESS;
  SetNode *TheSet = NULL;

  if(SUCCESS == ErrorStatus)
  {
    fp = fopen(LOCATIONS_FILENAME, "r");
    if(NULL == fp)
    {
      fprintf(stderr, "error: Unable to open " LOCATIONS_FILENAME " - weird\n");
      ErrorStatus = CANNOT_OPEN_FILE;
    }
  }

  // Hash each value 
  if(SUCCESS == ErrorStatus)
  {
    char Buf[108] = {0};
    while(SUCCESS == ErrorStatus && NULL != fgets(Buf, sizeof Buf, fp))
    {
      char *p = Buf + strlen(Buf) - 1;
      *p = '\0';

      uint64_t hash = CrapHash(Buf, 13);
      uint32_t trunc_hash = hash;

      fprintf(stdout, "[%s][%lu]trunc=[%lu]\n", Buf, hash, trunc_hash);

      if(SUCCESS != AddToSet(&TheSet, hash))
      {
        fprintf(stderr, "error: collision on %s\n", Buf);
        ErrorStatus = HAS_CLASHES;
      }

      memset(Buf, 0, sizeof Buf);
    }
  }

  


  if(NULL != fp)
  {
    fclose(fp);
    fp = NULL;
  }

  return 0;
}
