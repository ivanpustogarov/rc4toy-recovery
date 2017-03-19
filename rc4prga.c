#include <stdint.h>
#include <stdlib.h>
#include <alloca.h>
#include <stdio.h>
#include <string.h>
#include "util.h" // convert from hex to binary
#include "rc4prga.h"


/**
 * Initialize the RC4 SIZE bytes state
 *
 * @param key The key of RC4 cypher
 * @param kelen Length of the key
 * @param s The results will be written here. Should be preallocated by the caller
 * @return none
*/
void rc4_init(uint8_t *key, int keylen, uint8_t *s)
{
  int i = 0;
  int j = 0;
  int t = 0;
  for(i=0;i<SIZE;i++)
    s[i] = i;
  for(i=0;i<SIZE;i++)
  {
    j = (j + s[i] + key[i%keylen])%SIZE;
    t = s[i];
    s[i] = s[j];
    s[j] = t;
  }
}

/**
  * Generate the next byte of key stream based on state <s> and put the result to <r>
  *
  * @param s The current permutation of RC4, use rc4_init() to initalize this state
  * @param r The next byte of the keystream will be put here, should be preallocated by the caller
  * @param j Current value of j
*/
uint8_t rc4_step(uint8_t *s, int i, int *j)
{
   register int a,x,y;
   uint8_t r;
   a=*j;

   x=s[i];
   a=ind(a+x);
   y=s[a];
   s[i]=y; s[a]=x;
   r = s[ind(x+y)];

   *j = a;
   return r;
}
