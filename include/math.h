#ifndef __MATH_H
#define __MATH_H

#if FLT_EVAL_METHOD == 1
typedef double float_t;
typedef double double_t;
#else
typedef float float_t;
typedef double double_t;
#endif

#define _HUGE_VAL 1e+300
#define INFINITY ((float)(_HUGE_VAL*_HUGE_VAL))
#define HUGE_VAL ((double)INFINITY)
#define HUGE_VALF ((float)INFINITY)
#define HUGE_VALL ((long double)INFINITY)
#define NAN (INFINITY * 0.0f)

#define FP_INFINITE 1
#define FP_NAN 2
#define FP_NORMAL (-1)
#define FP_SUBNORMAL (-2)
#define FP_ZERO 0

#define FP_ILOGB0 (-0x7ffffffe)
#define FP_ILOGBNAN 0x7fffffff

#define MATH_ERRNO 1
#define MATH_ERREXCEPT 2
#define math_errhandling (MATH_ERRNO|MATH_ERREXCEPT)

// TODO

#endif //__MATH_H
