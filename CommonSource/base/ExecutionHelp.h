#pragma once
#include <algorithm>

// Seems like Android and Apple clang don't know execution policies yet. We work around with this file.

namespace std {

#if defined(ANDROID) || defined(__APPLE__)
enum class execution
{
    seq,
    par
};

template<class _ForwardIterator, class _Pred>
inline bool all_of(execution __exec, _ForwardIterator __first, _ForwardIterator __last, _Pred __pred)
{
    return all_of(__first, __last, __pred);
}

template<class ForwardIt1, class ForwardIt2, class UnaryOperation>
inline ForwardIt2 transform(execution policy, ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 d_first,
                            UnaryOperation unary_op)
{
    switch (policy)
    {
        case execution::seq:
            return transform(first1, last1, d_first, unary_op);
        case execution::par:
        {
            size_t n = std::distance(first1, last1);
#pragma omp parallel for
            for (size_t i = 0; i < n; ++i)
                *(d_first + i) = unary_op(*(first1 + i));
            return d_first;
        }
    }
}

template<class ForwardIt1, class ForwardIt2, class ForwardIt3, class BinaryOperation>
inline ForwardIt3 transform(execution policy, ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 first2,
                            ForwardIt3 d_first, BinaryOperation binary_op)
{
    switch (policy)
    {
        case execution::seq:
            return std::transform(first1, last1, first2, d_first, binary_op);
        case execution::par:
        {
            size_t n = std::distance(first1, last1);
#pragma omp parallel for
            for (size_t i = 0; i < n; ++i)
                *(d_first + i) = binary_op(*(first1 + i), *(first2 + i));
            return d_first;
        }
    }
}

#endif

}    // namespace std
