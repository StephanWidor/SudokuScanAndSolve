#pragma once
#include <algorithm>

#ifdef emit
#undef emit    // Qt's emit define clashes with TBB's emit define
#endif
#include <execution>

// Workaround for missing parallel execution on Android

namespace par_execution_help {

template<class _ForwardIterator, class _Pred>
inline bool all_of(_ForwardIterator __first, _ForwardIterator __last, _Pred __pred)
{
#if defined(__cpp_lib_execution) && defined(__cpp_lib_parallel_algorithm)
    return std::all_of(std::execution::par, __first, __last, __pred);
#else
    return std::all_of(__first, __last, __pred);
#endif
}

template<class ForwardIt1, class ForwardIt2, class UnaryOperation>
inline ForwardIt2 transform(ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 d_first, UnaryOperation unary_op)
{
#if defined(__cpp_lib_execution) && defined(__cpp_lib_parallel_algorithm)
    return std::transform(std::execution::par, first1, last1, d_first, unary_op);
#else
    size_t n = std::distance(first1, last1);
#pragma omp parallel for
    for (size_t i = 0; i < n; ++i)
        *(d_first + i) = unary_op(*(first1 + i));
    return d_first;
#endif
}

template<class ForwardIt1, class ForwardIt2, class ForwardIt3, class BinaryOperation>
inline ForwardIt3 transform(ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 first2, ForwardIt3 d_first,
                            BinaryOperation binary_op)
{
#if defined(__cpp_lib_execution) && defined(__cpp_lib_parallel_algorithm)
    return std::transform(std::execution::par, first1, last1, first2, d_first, binary_op);
#else
    size_t n = std::distance(first1, last1);
#pragma omp parallel for
    for (size_t i = 0; i < n; ++i)
        *(d_first + i) = binary_op(*(first1 + i), *(first2 + i));
    return d_first;
#endif
}

}    // namespace par_execution_help
