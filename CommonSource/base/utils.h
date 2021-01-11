#pragma once
#include <array>
#include <functional>
#include <tuple>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

namespace utils {

template<typename InitFunction, size_t... I>
constexpr auto makeArray(InitFunction initFunction, std::index_sequence<I...>)
{
    return std::array{initFunction(I)...};
}
template<size_t N, typename InitFunction>
constexpr auto makeArray(InitFunction initFunction)
{
    return makeArray(initFunction, std::make_index_sequence<N>{});
}

template<size_t N, typename T>
constexpr std::array<T, N> makeFilledArray(const T &value)
{
    return makeArray<N>([&value](size_t) { return value; });
}

template<class... Ts>
struct match : Ts...
{
    using Ts::operator()...;
};
template<class... Ts>
match(Ts...) -> match<Ts...>;

}    // namespace utils
