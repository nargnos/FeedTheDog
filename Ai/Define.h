#pragma once
#define USE_FLOAT
#ifdef USE_FLOAT
using FloatingPoint = float;
#define MATH_NAMESPACE concurrency::fast_math::
#else
using FloatingPoint = double;
#define MATH_NAMESPACE concurrency::precise_math::
#endif // USE_FLOAT



#define CHECK_ALPHA(alpha) //assert((alpha) >= 0 && (alpha) <= 1.f)