#ifndef __RC4PRGA_H__
#define __RC4PRGA_H__

/*
 * SIZE is (1<<ALPHA) = (1 times 2 to the 8th) = 256.
 * ind(x) is the low order 8 bits of x, or x mod 256.
 */
#ifndef ALPHA
  #define ALPHA      (8)
#endif
#define SIZE       (1<<ALPHA)
#define ind(x)     ((x)&(SIZE-1))

void rc4_init(uint8_t *key, int keylen, uint8_t *s);
uint8_t rc4_step(uint8_t *s, int i, int *j);

#endif // __RC4PRGA_H__
