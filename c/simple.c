#include <stdio.h>

#define SUCCESS 0 
#define CANNOT_OPEN_FILE 1

typedef struct _somenode {
  char location[101];
  unsigned long int count;
  long int total;
  int min;
  int max;
  struct _somenode *next;
} TempCountNode;


int LocationHash(const char *LocationName)
{
  int result = (LocationName[0] << 8) & LocationName[1];
  return result;
}

int AddTemperature(TempCountNode **target)
{
  
}


int process_file(const char *filename)
{
  int ErrorStatus = SUCCESS;
  FILE *fp = NULL;
  // Lazy, using the first 2 bytes as the location hash, should get us down to 2-5 items in each list which is probably good enough
  TempCountNode *Summary[256*256] = {0};


  // Open the file 
  if(SUCCESS == ErrorStatus)
  {
    fp = fopen(filename, "r");
    if(NULL == fp)
    {
      fprintf(stderr, "error: Unable to open file %s\n", filename);
      ErrorStatus = CANNOT_OPEN_FILE;
    }
  }

  // Process each line in the file and count them 
  if(SUCCESS == ErrorStatus)
  {
    unsigned long LineCount = 0;
    char Buf[8192] = {0};
    while(NULL != fgets(Buf, sizeof Buf, fp))
    {
      ++LineCount; 
    }
    fprintf(stdout, "line count: %lu\n", LineCount);
  }


  // Make sure to close it again
  if(NULL != fp)
  {
    fclose(fp);
    fp = NULL;
  }

  return ErrorStatus;
}


int main(int argc, char *argv[])
{
  const char *filename = "measurements.txt";

  // Check additional args for different filename
  if(2 == argc)
  {
    filename = argv[1];
  }
  printf("Input file: %s\n", filename);

  // Process the file for the result
  process_file(filename);

  return 0;
}

