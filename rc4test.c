#include <stdint.h>
#include <stdlib.h>
#include <alloca.h>
#include <stdio.h>
#include <string.h>
#include "util.h" // convert from hex to binary
#include "rc4prga.h"


/* Print permutation
*/
void print_permutation(uint8_t *s)
{
  int l = 0;

  printf("Permuation:\n");
  for(l=0;l<SIZE;l++)
    printf("% 3d ", l);
  printf("\n");
  for(l=0;l<SIZE;l++)
    printf("% 3d ", s[l]);

  printf("\n");
  return;
}

int main(int argc, char *argv[])
{
  if(argc != 3)
  {
    printf("Reduced RC4 key stream cipher (ALPHA=%d bits, state size is %d)\n\n", ALPHA, SIZE);
    printf("Usage: rc4test KEY LEN \n");
    printf("          KEY	encryption key (in hex)\n");
    printf("          LEN	the lenght of the keystream to generate\n");
    exit(0);
  }
  int i = 0;
  int j = 0;
  uint8_t s[SIZE]; // RC4 state permutation

  // Parse the key from the command line
  uint8_t *key_str = (uint8_t *)argv[1];
  int stream_len = atoi(argv[2]);
  int key_len = strlen((char *)key_str)/2;
  uint8_t *key = (uint8_t*)alloca(key_len);
  fromHex(key, key_str, key_len, 0);

  // Init the cipher
  rc4_init(key,key_len,s);
  printf("Keystream: ");
  for(i=1;i<=stream_len;i++)
  {
    uint8_t k = rc4_step(s,ind(i),&j);
    printf("%02x",k);
  }
  printf("\nSecret state at the last step:\n");
  print_permutation(s);
  return 0;
}
