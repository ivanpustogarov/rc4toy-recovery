#include <stdlib.h>
#include <stdio.h>
#include <alloca.h>
#include <string.h>
#include <stdint.h>
#include "util.h"

const uint8_t hexDigits[] = "0123456789abcdef";

/* Convert array of bytes to its hexadecimal representation
   <dst> -- a char array of size 2*<size>+1 (to store \0 byte)
   <src> -- array of bytes to convert to hex
   <size> -- lenght of <src>
   <rev> -- if true, the last byte becomes the first hex digit, etc.
*/
void toHex(uint8_t *dst, const uint8_t *src, size_t size, int rev)
{
  int incr = 1;
  const uint8_t *p = src;
  const uint8_t *e = size + src;
  if(rev)
  {   
    p = e-1;
    e = src-1;
    incr = -1;
  }

  while(PREDICT_LIKELY(p!=e))
  {
    uint8_t c = p[0];
    dst[0] = hexDigits[c>>4];
    dst[1] = hexDigits[c&0xF];
    p += incr;
    dst += 2;
  }
  dst[0] = 0;
}

/* Print hex representation of a byte array
  <p> -- pointer to the array
  <size> -- size of the array
  <rev> -- print from last to first
*/
void showHex(const uint8_t *p, size_t size, int rev)
{
  uint8_t* buf = (uint8_t*)alloca(2*size + 1);
  toHex(buf, p, size, rev);
  printf("%s", buf);
}

/* Convert a hex character to number that it represents
   <h> -- character to convert
*/
uint8_t fromHexDigit(uint8_t h)
{
  if(PREDICT_LIKELY('0'<=h && h<='9')) return      (h - '0');
  if(PREDICT_LIKELY('a'<=h && h<='f')) return 10 + (h - 'a');
  if(PREDICT_LIKELY('A'<=h && h<='F')) return 10 + (h - 'A');
  return 0xFF;
}

/**
 * Convert a hex string to binary
 *
 * @param dst Binary array to fill
 * @param src Hex string
 * @param dstSize Size of dst
 * @param rev If set, convert from last to first
*/
int fromHex(uint8_t *dst, const uint8_t *src, size_t dstSize, int rev)
{
  int incr = 2;
  uint8_t *end = dstSize + dst;
  if(rev)
  {
    src += 2*(dstSize-1);
    incr = -2;
  }

  while(PREDICT_LIKELY(dst<end))
  {
    uint8_t hi = fromHexDigit(src[0]);
    if(PREDICT_UNLIKELY(0xFF==hi)) return -1;

    uint8_t lo = fromHexDigit(src[1]);
    if(PREDICT_UNLIKELY(0xFF==lo)) return -1;

    *(dst++) = (hi<<4) + lo;
    src += incr;
  }

  return 0;
}
