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


  int
fill_trapdoors(uint32_t *l, uint32_t *r)
{
  uint32_t randbytes = getrandom(l, N*4, 0);
  randbytes += getrandom(r, N*4, 0);
  int i;
  for(i=0; i<N; i++) 
  {
    l[i] %= 59393;
    r[i] %= 59393;
  }
  return randbytes == N*8;
}

  void
rot(uint32_t *ret, uint32_t *b, uint32_t *x)
{
  int i;
  for(i=0; i<N; i++)
  {
    MUL_MOD(ret[i], b[i], x[i], 59393);
  }
}

  uint32_t *
label(uint32_t *ret, uint32_t *l, uint32_t *r, int index)
{
  int i;
  for(i=0; i<32; i++)
  {
    int is_left = ((index >> i) & 1);
    rot(ret, is_left ? l : r, ret);
  }
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
  const int MAX_LEAVES = 65536;
  uint32_t ret[N];
  for(int i=0; i<N; i++) {
    ret[i] = 1;
  }
  float label_start = get_time(); 
  label(ret, l, r, 15);
  float label_end = get_time(); 
  float label_time = label_end - label_start;
  printf("computing a label: %f\n", label_time);
  return 0;
}
