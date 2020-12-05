#pragma once

namespace paddock
{
namespace mp
{
template <typename... Ts>
struct Types
{
};

namespace detail
{
    template <template <typename... > class MF, typename...>
    struct apply;

    template <template <typename... > class MF, typename... Ts>
    struct apply<MF, Types<Ts...>>
    {
        using type = MF<Ts...>;
    };
}

template <template <typename... > class MF, class List>
using apply = detail::apply<MF, List>::type;

namespace detail
{
template <typename...>
struct join;

template <>
struct join<>
{
    using type = Types<>;
};

template <typename... Ts>
struct join<Types<Ts...>>
{
    using type = Types<Ts...>;
};

template <typename... Ts, typename... Us, typename... remainder>
struct join<Types<Ts...>, Types<Us...>, remainder...>
{
    using type = typename join<Types<Ts..., Us...>, remainder...>::type;
};
} // namespace detail

template <typename... Lists>
using join = detail::join<Lists...>::type;

} // namespace mp
} // namespace paddock
