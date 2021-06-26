#include <stdio.h>
#include <sys/random.h>
#include "FFT/FFT_1024_59393.h"
#include "FFT/FFT_1024_59393_constants.c"
#include <time.h>
#include <sys/resource.h>

#define N 1024

float get_time()
{
  return(float)clock()/CLOCKS_PER_SEC;
}

uint32_t ones[N] = {1};

int
fill_trapdoors(uint32_t *l, uint32_t *r)
{
  uint32_t randbytes = getrandom(l, N, 0);
  randbytes += getrandom(l, N, 0);
  return randbytes == N*2;
}

int main()
{
  float trapdoor_start = get_time(); 
  uint32_t l[N];
  uint32_t r[N];
  float trapdoor_end = get_time(); 
  if (!fill_trapdoors(l, r)) return 1;
  float trapdoor_time = trapdoor_end - trapdoor_start;
  printf("trapdoor generation: %f\n", trapdoor_time);
  return 0;
}
