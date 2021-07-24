#include <stdio.h>
#include <sys/random.h>
#include "FFT/FFT_1024_59393.h"
#include "FFT/FFT_1024_59393_constants.c"
#include <time.h>
#include <sys/resource.h>

#define N 1024

long double get_time()
{
  return(long double)clock()/CLOCKS_PER_SEC;
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
  int i, j;
  long double trapdoor_start = get_time(); 
  uint32_t l[N];
  uint32_t r[N];
  long double trapdoor_end = get_time(); 
  if (!fill_trapdoors(l, r)) return 1;
  long double trapdoor_time = trapdoor_end - trapdoor_start;
  printf("trapdoor generation: %Lf\n", trapdoor_time);
  {
    uint32_t ret[N];
    for(i=0; i<N; i++) {
      ret[i] = 1;
    }
    long double label_start = get_time(); 
    label(ret, l, r, 15);
    long double label_end = get_time(); 
    long double label_time = label_end - label_start;
    printf("computing a label: %Lf\n", label_time);
  }
  uint32_t accumulator[N] = {0}; 
  uint32_t current_label[N];
  {
    long double accumulation_start = get_time();
    for(i=0; i<65536; i++)
    {
      label(current_label, l, r, i);
      for(j=0; j<N; j++)
      {
        ADD_MOD(accumulator[j], accumulator[j], current_label[j], 59393);
      }
    }
    long double accumulation_end = get_time(); 
    long double accumulation_time = accumulation_end - accumulation_start;
    printf("accumulation of 65536 elements: %Lf\n", accumulation_time);
  }
  uint32_t witness[N] = {0};
  {
    long double witness_start = get_time();
    for(i=0; i<N; i++)
    {
      SUB_MOD(witness[i], accumulator[i], current_label[i], 59393);
    }
    long double witness_end = get_time(); 
    long double witness_time = witness_end - witness_start;
    printf("witness generation: %Lf\n", witness_time);
  }
  uint32_t verifier_state[N] = {0};
  {
    long double verification_start = get_time();
    for(i=0; i<N; i++)
    {
      ADD_MOD(verifier_state[i], witness[i], current_label[i], 59393);
      if(verifier_state[i] != accumulator[i]) {
        printf("Verification failed!");
        return 1;
      }
    }
    long double verification_end = get_time(); 
    long double verification_time = verification_end - verification_start;
    printf("verification: %Lf\n", verification_time);
  }
  return 0;
}
