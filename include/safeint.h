#ifndef SAFEINT_INCLUDED
#define SAFEINT_INCLUDED

#include <limits.h>

#include "utils.h"
#include "assert.h"
#include "except.h"
#include "portable.h"

BEGIN_DECLS

#ifndef SAFE_SIZE
#define SAFE_SIZE SIZE_MAX / 2
#endif

extern const Except_T cast_error;
extern const Except_T overflow_error;
extern const Except_T divide_by_zero;

/* casts */
inline signed   safe_cast_us(unsigned u);
inline unsigned safe_cast_su(signed s);

/* heuristics */
inline void     safe_size(size_t s);

/* operations */
inline void safe_sum_uu(unsigned u1, unsigned u2);
inline void safe_sub_uu(unsigned u1, unsigned u2);
inline void safe_mul_uu(unsigned u1, unsigned u2);
inline void safe_div_uu(unsigned u1, unsigned u2);

inline void safe_sum_sisi(size_t u1, size_t u2);
inline void safe_sub_sisi(size_t u1, size_t u2);
inline void safe_mul_sisi(size_t u1, size_t u2);
inline void safe_div_sisi(size_t u1, size_t u2);

/************************************************************************/

C_ASSERT( -1 == (int)0xffffffff ); /* we are on complement 2 machine*/

/* casts */
inline signed safe_cast_us(unsigned u) {
    if(fast_c(u <= INT_MAX)) return (signed) u;
    RAISE_INT(cast_error);
}

inline unsigned safe_cast_su(signed s) {
    if(fast_c(s >= 0)) return (unsigned) s;
    RAISE_INT(cast_error);
}

inline void safe_size(size_t s) {
    if(slow_c(s > SAFE_SIZE)) RAISE_RET(overflow_error);
}

/* unsigned int */

inline void safe_sum_uu(unsigned u1, unsigned u2) {
    if(slow_c(u1 + u2 < u1)) RAISE_RET(overflow_error);
}

inline void safe_sub_uu(unsigned u1, unsigned u2) {
    if(slow_c(u1 < u2)) RAISE_RET(overflow_error);
}

inline void safe_mul_uu(unsigned u1, unsigned u2) {
    unsigned res = u1 * u2;
    if(slow_c(u1 != 0 && res / u1 < u2)) RAISE_RET(overflow_error);   
}

inline void safe_div_uu(unsigned u1, unsigned u2) {
    if(slow_c(u2 == 0)) RAISE_RET(divide_by_zero);
}

/* size_t */

inline void safe_sum_sisi(size_t u1, size_t u2) {
    if(slow_c(u1 + u2 < u1)) RAISE_RET(overflow_error);
}

inline void safe_sub_sisi(size_t u1, size_t u2) {
    if(slow_c(u1 < u2)) RAISE_RET(overflow_error);
}

inline void safe_mul_sisi(size_t u1, size_t u2) {
    size_t res = u1 * u2;
    if(slow_c(u1 != 0 && res / u1 < u2)) RAISE_RET(overflow_error);   
}

inline void safe_div_sisi(size_t u1, size_t u2) {
    if(slow_c(u2 == 0)) RAISE_RET(divide_by_zero);
}

END_DECLS

#endif