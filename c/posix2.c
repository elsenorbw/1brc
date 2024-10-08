#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

#define SUCCESS 0 
#define CANNOT_OPEN_FILE 1
#define OOM_SOMEHOW 2
#define BAD_HASH_VALUE 3
#define CANNOT_FADVISE 4
#define BUFFER_ALLOCATION_FAILED 5
#define THREAD_FAILED 6
#define CANNOT_CREATE_THREAD 7


#define FALSE 0
#define TRUE 1



#define BLOCK_LIMIT 5000000
#define MEGABYTE (1024 * 1024)
#define BLOCK_SIZE (128 * 1024)

#define THREADS 64


typedef struct _somenode {
  char location[101];
  unsigned long int count;
  long int total;
  int min;
  int max;
  struct _somenode *next;
} TempCountNode;


typedef struct _tis {
  char Buf[BLOCK_SIZE];
  size_t BufLen;
  char *EndOfBuf;
  TempCountNode NodePool[10000];
  size_t NextNode;
  TempCountNode *Summary[256*256];
  pthread_t ThreadHandle;
  int ThreadAllocated;
} ThreadInfoSpace;


ThreadInfoSpace *ThreadInfo[THREADS] = {0};


TempCountNode *FastAllocTempNode(ThreadInfoSpace *ThreadInfo)
{
  TempCountNode *result = &ThreadInfo->NodePool[ThreadInfo->NextNode];
  ThreadInfo->NextNode += 1;
  return result;
}


/*
 *  We want a structure for each thread we want to run where we have (a) the input buffer, (b) the length of that buffer (or a pointer to the end), (c) the destination storage for results
 *  This way each thread can whir away on its own and then we can merge the results together at the end.
 */



int LocationHash(const char *LocationName)
{
  unsigned char a = LocationName[0];
  unsigned char b = LocationName[1];
  int result = (a << 8) | b;
  return result;
}

int AddTemperature(TempCountNode **target, ThreadInfoSpace *ThreadInfo, const char *Location, int Temp)
{
  int ErrorStatus = SUCCESS;

  if(NULL == *target)  // This is the first time, ok, cool, let's allocate a new one..
  {
    TempCountNode *t = FastAllocTempNode(ThreadInfo);
    strcpy(t->location, Location);
    t->count = 1;
    t->total = Temp;
    t->max = Temp;
    t->min = Temp;
    t->next = NULL;
    *target = t;
  }
  else
  {
    int CompareResult = strcmp((*target)->location, Location);
    if(0 == CompareResult)
    {
      TempCountNode *t = *target;
      if(Temp < t->min)
      {
        t->min = Temp;
      } 
      else if (Temp > t->max) 
      {
        t->max = Temp;
      }
      t->total += Temp;
      t->count += 1;
    }
    else if(0 < CompareResult)
    {
      // Ok, we need to add this new one here since this is where it belongs, collation-sequence wise
      TempCountNode *t = FastAllocTempNode(ThreadInfo);
      strcpy(t->location, Location);
      t->count = 1;
      t->total = Temp;
      t->max = Temp;
      t->min = Temp;
      t->next = *target;
      *target = t;
    } 
    else // Not a match, on to the next one..
    {
      ErrorStatus = AddTemperature(&(*target)->next, ThreadInfo, Location, Temp);
    }
  }

  return ErrorStatus;
}

int MergeTemperature(TempCountNode **target, ThreadInfoSpace *ThreadInfo, TempCountNode *SrcData)
{
  int ErrorStatus = SUCCESS;

  if(NULL == *target)  // This is the first time, ok, cool, let's allocate a new one..
  {
    TempCountNode *t = FastAllocTempNode(ThreadInfo);
    strcpy(t->location, SrcData->location);
    t->count = SrcData->count;
    t->total = SrcData->total;
    t->max = SrcData->max;
    t->min = SrcData->min;
    t->next = NULL;
    *target = t;
  }
  else
  {
    int CompareResult = strcmp((*target)->location, SrcData->location);
    if(0 == CompareResult)
    {
      TempCountNode *t = *target;
      if(SrcData->min < t->min)
      {
        t->min = SrcData->min;
      } 
      if (SrcData->max > t->max) 
      {
        t->max = SrcData->max;
      }
      t->total += SrcData->total;
      t->count += SrcData->count;
    }
    else if(0 < CompareResult)
    {
      // Ok, we need to add this new one here since this is where it belongs, collation-sequence wise
      TempCountNode *t = FastAllocTempNode(ThreadInfo);
      strcpy(t->location, SrcData->location);
      t->count = SrcData->count;
      t->total = SrcData->total;
      t->max = SrcData->max;
      t->min = SrcData->min;
      t->next = *target;
      *target = t;
    } 
    else // Not a match, on to the next one..
    {
      ErrorStatus = MergeTemperature(&(*target)->next, ThreadInfo, SrcData);
    }
  }

  return ErrorStatus;
}

int StoreTemperature(ThreadInfoSpace *ThreadInfo, const char *Location, int Temp)
{
  int ErrorStatus = SUCCESS;

  // Get a hash and cehck it fits
  int HashLocation = 0;
  if(SUCCESS == ErrorStatus)
  {
    HashLocation = LocationHash(Location);
    ErrorStatus = AddTemperature(&ThreadInfo->Summary[HashLocation], ThreadInfo, Location, Temp);
  }

  return ErrorStatus;
}

void MergeSummary(ThreadInfoSpace *Dest, ThreadInfoSpace *Src)
{
  // Add everything we have in Src into Dest.. 
  for(size_t i = 0; i < sizeof Src->Summary / sizeof *Src->Summary; i++)
  {
    TempCountNode *t = Src->Summary[i];
    while(NULL != t)
    {
      // Merge this one into Dest..
      MergeTemperature(&Dest->Summary[i], Dest, t);

      // Next one
      t = t->next;
    }
  }
  
}

int intfor(int c)
{
  return c - '0';
}

// parse the temperature, return a pointer to after it is complete
// we want all the digits as an int, there may be a sign
// after the decimal point there will be exactly 1 digit
char *parse_temp(int *dest, char *p)
{
  if('-' == *p)
  {
    ++p;
    // so, we're pointed at the first digit, everything is in the range 0-99.9 
    // really, there are 2 options here we have n.n or nn.n 
    if(p[1] == '.')
    {
      *dest = - (intfor(p[0]) * 10 + intfor(p[2]));
      p += 3;
    } else {
      *dest = - (intfor(p[0]) * 100 + intfor(p[1]) * 10 + intfor(p[3]));
      p += 4;
    }
  }
  else
  {
    // so, we're pointed at the first digit, everything is in the range 0-99.9 
    // really, there are 2 options here we have n.n or nn.n 
    if(p[1] == '.')
    {
      *dest = (intfor(p[0]) * 10 + intfor(p[2]));
      p += 3;
    } else {
      *dest = (intfor(p[0]) * 100 + intfor(p[1]) * 10 + intfor(p[3]));
      p += 4;
    }
  }

  return p;
}

void *ProcessOneBlock(void *Info)
{

  int ErrorStatus = SUCCESS;
  ThreadInfoSpace *ThreadInfo = Info;

  char *p = ThreadInfo->Buf;
  // We always start at the beginning of a name 
  char *Location = p;
  while(SUCCESS == ErrorStatus && p < ThreadInfo->EndOfBuf)
  {

    switch(*p)
    {
      case '\n':
        // Next char will be the start of the next name 
        Location = p + 1;
        // Names are always at least 1 character
        p += 2;
        break;

      case ';':
        // Ok, end of the location, time to parse the number 
        *p = '\0';
        int the_temp = 0;
        p = parse_temp(&the_temp, p + 1);
        //fprintf(stdout, "Location: [%s], Temp: [%d]\n", Location, the_temp);
        ErrorStatus = StoreTemperature(ThreadInfo, Location, the_temp);
        break;

      default:
        // Nothing special, just moving on
        ++p;
        break;
    }
  }

  return NULL;
}


int print_summary(TempCountNode **HashTable, size_t HashTableLength)
{
  int ErrorStatus = SUCCESS;

  for(size_t i = 0; i < HashTableLength; i++)
  {
    TempCountNode *t = HashTable[i];
    while(NULL != t)
    {
      float the_min = t->min / 10.0;
      float the_avg = (t->total / (float)t->count) / 10.0;
      float the_max = t->max / 10.0;
      fprintf(stdout, "%s=%0.1f/%0.1f/%0.1f\n", t->location, the_min, the_avg, the_max);

      // Next one
      t = t->next;
    }
  }

  return ErrorStatus;
}


int process_file(const char *filename)
{
  int ErrorStatus = SUCCESS;
  int fd = -1;

  // Lazy, using the first 2 bytes as the location hash, should get us down to 2-5 items in each list which is probably good enough
  TempCountNode *Summary[256*256] = {0};

  // Open the file 
  if(SUCCESS == ErrorStatus)
  {
    fd = open(filename, O_RDONLY, 0);
    if(-1 == fd)
    {
      fprintf(stderr, "error: Unable to open file %s\n", filename);
      ErrorStatus = CANNOT_OPEN_FILE;
    }
  }

  // Let the kernel know we'll be running through the file in sequential order
  if(SUCCESS == ErrorStatus)
  {
    if(0 != posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL | POSIX_FADV_WILLNEED))
    {
      fprintf(stderr, "error: cannot give the kernel advice\n");
      ErrorStatus = CANNOT_FADVISE;
    }
  }

  // Process each line in the file and count them 
  if(SUCCESS == ErrorStatus)
  {
    unsigned long BlockCount = 0;
    char Buf[BLOCK_SIZE] = {0};
    size_t bytes_read = 0;
    size_t BytesToCopyBack = 0;
    size_t NextThreadIndex = 0;


    // Allocate thread count full-size memory buffers to avoid overlap 
    if(SUCCESS == ErrorStatus)
    {
      for(size_t i = 0; i < sizeof ThreadInfo / sizeof *ThreadInfo; ++i)
      {
        ThreadInfo[i] = malloc(sizeof *ThreadInfo[i]);
        if(NULL == ThreadInfo[i])
        {
          fprintf(stderr, "error: unable to allocate buffer[%zu] for %zu bytes\n", i, sizeof **ThreadInfo);
          ErrorStatus = BUFFER_ALLOCATION_FAILED;
        }
        else
        {
//          fprintf(stderr, "interesting: able to allocate buffer[%zu] for %zu bytes\n", i, sizeof **ThreadInfo);/g
          memset(ThreadInfo[i]->Buf, 0, BLOCK_SIZE);
        }
      }
    }

    while(BlockCount < BLOCK_LIMIT && SUCCESS == ErrorStatus && 0 != (bytes_read = read(fd, Buf + BytesToCopyBack, (sizeof Buf) - BytesToCopyBack) + BytesToCopyBack))
    {
      //fprintf(stdout, "Block[%lu]:\n%*.*s\nEnd of Block\n", BlockCount, (int)bytes_read, (int)bytes_read, Buf);

      // find the last newline character
      char *EndOfBlock = Buf + bytes_read - 1;
      BytesToCopyBack = 0;
      while(EndOfBlock > Buf && *EndOfBlock != '\n')
      {
        --EndOfBlock;
        ++BytesToCopyBack;
      }

      // Has this thread finished running the last block ?
      if(ThreadInfo[NextThreadIndex]->ThreadAllocated)
      {
        // Wait for it...
//        fprintf(stderr, "interesting: About to join thread %zu\n", NextThreadIndex);/g
        int ThreadResult = pthread_join(ThreadInfo[NextThreadIndex]->ThreadHandle, NULL);
        if(0 != ThreadResult)
        {
          fprintf(stderr, "error: Thread %zu has returned a non-zero code of %d\n", NextThreadIndex, ThreadResult);
          ErrorStatus = THREAD_FAILED;
        }
        else
        {
          ThreadInfo[NextThreadIndex]->ThreadAllocated = FALSE;
        }
      }

      // Setup this thread 
      size_t BytesInBlock = EndOfBlock - Buf;
//      fprintf(stderr, "interesting: About to copy %zu to thread %zu\n", BytesInBlock, NextThreadIndex);/g
      memcpy(ThreadInfo[NextThreadIndex]->Buf, Buf, BytesInBlock);
//      fprintf(stderr, "interesting: Done the copy %zu to thread %zu\n", BytesInBlock, NextThreadIndex);/g
      ThreadInfo[NextThreadIndex]->BufLen = BytesInBlock;
      ThreadInfo[NextThreadIndex]->EndOfBuf = ThreadInfo[NextThreadIndex]->Buf + BytesInBlock;

      // And start the thread
//      fprintf(stderr, "interesting: About to start thread %zu\n", NextThreadIndex);/g
      int ThreadCreateResult = pthread_create(&ThreadInfo[NextThreadIndex]->ThreadHandle, NULL, ProcessOneBlock, ThreadInfo[NextThreadIndex]);
      if(0 != ThreadCreateResult)
      {
        fprintf(stderr, "error: cannot create thread %d\n", ThreadCreateResult);
        ErrorStatus = CANNOT_CREATE_THREAD;
      }
      else
      {
//        fprintf(stderr, "interesting: Started thread %zu\n", NextThreadIndex);/g
        ThreadInfo[NextThreadIndex]->ThreadAllocated = TRUE;
        //sleep(10);
      }

      //size_t BufLen = EndOfBlock - Buf;
      //fprintf(stdout, "Block to process:\n%*.*s\nEnd of block\n", (int)BufLen, (int)BufLen, Buf);
      //fprintf(stdout, "Copy back (%lu) will be:[%*.*s]\n", BytesToCopyBack, (int)BytesToCopyBack, (int)BytesToCopyBack, EndOfBlock + 1);
      //ErrorStatus = ProcessOneBlock(Summary, Buf, EndOfBlock);


      // And copy back the remaining bytes
      memcpy(Buf, EndOfBlock+1, BytesToCopyBack);

      // And point at the next Thread 
      NextThreadIndex += 1;
      if(NextThreadIndex >= sizeof ThreadInfo / sizeof *ThreadInfo)
      {
        NextThreadIndex = 0;
      }

      ++BlockCount; 
    }
    //fprintf(stdout, "block count: %lu\n", BlockCount);
  }


  // Wait for all the threads to finish..
  for(size_t i = 0; i < sizeof ThreadInfo / sizeof *ThreadInfo; ++i)
  {
        int ThreadResult = pthread_join(ThreadInfo[i]->ThreadHandle, NULL);
        if(0 != ThreadResult)
        {
          fprintf(stderr, "error: Thread %zu has returned a non-zero code of %d\n", i, ThreadResult);
          ErrorStatus = THREAD_FAILED;
        }
  }

  // Merge all the thread results together
  for(size_t src = 1; src < sizeof ThreadInfo / sizeof *ThreadInfo; ++src)
  {
    MergeSummary(ThreadInfo[0], ThreadInfo[src]);
  }

  // Output the results
  if(SUCCESS == ErrorStatus)
  {
    print_summary(ThreadInfo[0]->Summary, sizeof Summary / sizeof *Summary);
  }


  // Make sure to close it again
  if(-1 != fd)
  {
    close(fd);
    fd = -1;
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
  //printf("Input file: %s\n", filename);

  // Process the file for the result
  process_file(filename);

  return 0;
}

