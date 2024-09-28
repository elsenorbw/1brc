#include <stdint.h>

uint32_t SuperFastHash (const char * data, int len);
uint64_t CrapHash(const void *data, int chunks_to_hash);
uint64_t FNVHash(const unsigned char *data);


