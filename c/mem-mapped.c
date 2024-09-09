#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define SUCCESS 0 
#define CANNOT_OPEN_FILE 1
#define OOM_SOMEHOW 2
#define BAD_HASH_VALUE 3
#define CANNOT_STAT_FILE 4
#define CANNOT_MMAP_FILE 5
#define CANNOT_NUNMAP_FILE 6

#define BLOCK_LIMIT 5000000
#define MEGABYTE (1024 * 1024)
#define BLOCK_SIZE MEGABYTE


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
  unsigned char a = LocationName[0];
  unsigned char b = LocationName[1];
  int result = (a << 8) | b;
  return result;
}

int AddTemperature(TempCountNode **target, const char *Location, int Temp)
{
  int ErrorStatus = SUCCESS;

  if(NULL == *target)  // This is the first time, ok, cool, let's allocate a new one..
  {
    TempCountNode *t = malloc(sizeof **target);
    if(NULL == t)
    {
      fprintf(stderr, "error: Unable to allocate memory for one more TempCountNode\n");
      ErrorStatus = OOM_SOMEHOW;
    } 
    else
    {
      strcpy(t->location, Location);
      t->count = 1;
      t->total = Temp;
      t->max = Temp;
      t->min = Temp;
      t->next = NULL;
      *target = t;
    }
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
      TempCountNode *t = malloc(sizeof **target);
      if(NULL == t)
      {
        fprintf(stderr, "error: Unable to allocate memory for one more TempCountNode\n");
        ErrorStatus = OOM_SOMEHOW;
      } 
      else
      {
        strcpy(t->location, Location);
        t->count = 1;
        t->total = Temp;
        t->max = Temp;
        t->min = Temp;
        t->next = *target;
        *target = t;
      }
    } 
    else // Not a match, on to the next one..
    {
      ErrorStatus = AddTemperature(&(*target)->next, Location, Temp);
    }
  }

  return ErrorStatus;
}

int StoreTemperature(TempCountNode **HashTable, size_t HashTableLength, const char *Location, int Temp)
{
  int ErrorStatus = SUCCESS;

  // Get a hash and cehck it fits
  int HashLocation = 0;
  if(SUCCESS == ErrorStatus)
  {
    HashLocation = LocationHash(Location);
    if(HashLocation < 0 || (size_t)HashLocation >= HashTableLength)
    {
      fprintf(stderr, "error: We generated a hash of %d for %s but the HashTable only has %lu spots\n", HashLocation, Location, HashTableLength);
      ErrorStatus = BAD_HASH_VALUE;
    }
  }

  // and Store it..
  if(SUCCESS == ErrorStatus)
  {
    ErrorStatus = AddTemperature(&HashTable[HashLocation], Location, Temp);
  }

  return ErrorStatus;
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

int ProcessOneBlock(TempCountNode **HashTable, size_t HashTableLength, size_t *LineCount, char *Start, char *End)
{

  int ErrorStatus = SUCCESS;

  char *p = Start;
  // We always start at the beginning of a name 
  char *Location = p;
  while(SUCCESS == ErrorStatus && p < End)
  {

    switch(*p)
    {
      case '\n':
        *LineCount += 1;
        // Next char will be the start of the next name 
        Location = p + 1;
        // Names are always at least 1 character
        p += 2;
        break;

      case ';':
        // Ok, end of the location, time to parse the number 
        //fprintf(stderr, "info: about to smash out the semi-colon\n");
        *p = '\0';
        //fprintf(stderr, "info: done that smash\n");
        int the_temp = 0;
        p = parse_temp(&the_temp, p + 1);
        //fprintf(stdout, "Location: [%s], Temp: [%d]\n", Location, the_temp);
        ErrorStatus = StoreTemperature(HashTable, HashTableLength, Location, the_temp);
        break;

      default:
        // Nothing special, just moving on
        ++p;
        break;
    }
  }

  // Loop will stop one \n short 
  *LineCount += 1;

  return ErrorStatus;
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

  fprintf(stdout, "}\n");

  return ErrorStatus;
}


int process_file(const char *filename)
{
  int ErrorStatus = SUCCESS;
  int fd = -1;
  off_t FileSize = 0;

  // Lazy, using the first 2 bytes as the location hash, should get us down to 2-5 items in each list which is probably good enough
  TempCountNode *Summary[256*256] = {0};

  // Open the file 
  if(SUCCESS == ErrorStatus)
  {
    fd = open(filename, O_RDONLY);
    if(-1 == fd)
    {
      fprintf(stderr, "error: Unable to open file %s\n", filename);
      ErrorStatus = CANNOT_OPEN_FILE;
    }
  }

  // Stat that sucker to find out how big it is
  if(SUCCESS == ErrorStatus)
  {
    struct stat file_info = {0};
    if(-1 == fstat(fd, &file_info))
    {
      fprintf(stderr, "error: Unable to stat the opened file??\n");
      ErrorStatus = CANNOT_STAT_FILE;
    }
    else
    {
      FileSize = file_info.st_size;
      fprintf(stderr, "info: File is %lu bytes\n", (unsigned long)FileSize);
    }
  }

  // Memory map that sucker 
  char *FileMap = NULL;
  if(SUCCESS == ErrorStatus)
  {
    FileMap = mmap(0, FileSize, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (FileMap == MAP_FAILED) 
    {
      fprintf(stderr, "error: mmap failed!\n");
      FileMap = NULL;
      ErrorStatus = CANNOT_MMAP_FILE;
    }
    else
    {
      fprintf(stderr, "info: mmap worked\n");
    }
  }

  // Process each line in the file and count them 
  if(SUCCESS == ErrorStatus)
  {
    size_t LineCount = 0;

    ErrorStatus = ProcessOneBlock(Summary, sizeof Summary / sizeof *Summary,  &LineCount, FileMap, FileMap + FileSize - 1);
    fprintf(stderr, "info: processed %lu lines\n", (unsigned long)LineCount); 
  }

  // Output the results
  if(SUCCESS == ErrorStatus)
  {
    print_summary(Summary, sizeof Summary / sizeof *Summary);
  }

  // Unmap
  if(NULL != FileMap)
  {
    if(-1 == munmap(FileMap, FileSize))
    {
      fprintf(stderr, "error: Can't unmap the file??\n");
      ErrorStatus = CANNOT_NUNMAP_FILE;
    }
    else
    {
      FileMap = NULL;
    }
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

