#ifndef __UTIL_H__
#define __UTIL_H__

/* GCC has several useful attributes. */
#if defined(__GNUC__) && __GNUC__ >= 3
/** Macro: Evaluates to <b>exp</b> and hints the compiler that the value
 * of <b>exp</b> will probably be true.
 *
 * In other words, "if (PREDICT_LIKELY(foo))" is the same as "if (foo)",
 * except that it tells the compiler that the branch will be taken most of the
 * time.  This can generate slightly better code with some CPUs.
 */
#define PREDICT_LIKELY(exp) __builtin_expect(!!(exp), 1)
/** Macro: Evaluates to <b>exp</b> and hints the compiler that the value
 * of <b>exp</b> will probably be false.
 *
 * In other words, "if (PREDICT_UNLIKELY(foo))" is the same as "if (foo)",
 * except that it tells the compiler that the branch will usually not be
 * taken.  This can generate slightly better code with some CPUs.
 */
#define PREDICT_UNLIKELY(exp) __builtin_expect(!!(exp), 0)
#else
#define PREDICT_LIKELY(exp) (exp)
#define PREDICT_UNLIKELY(exp) (exp)
#endif



void toHex(uint8_t *dst, const uint8_t *src, size_t size, int rev);  
void showHex(const uint8_t *src, size_t size, int rev);  
uint8_t fromHexDigit(uint8_t h);  
int fromHex(uint8_t *dst, const uint8_t *src, size_t dstSize, int rev); 

#endif // __UTIL_H__
