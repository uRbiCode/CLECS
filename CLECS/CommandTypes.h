#pragma once
#include <type_traits>

template<typename... Ts>
struct TypeList {};

template<typename TList, typename... Args>
struct ValidCommandArgsHelper : std::false_type {};

template<typename... Components, typename... Args>
struct ValidCommandArgsHelper<TypeList<Components...>, Args...>
    : std::bool_constant<sizeof...(Components) == sizeof...(Args) && (std::is_same_v<std::remove_cvref_t<Args>, Components> && ...)> {};

template<typename TList, typename... Args>
concept ValidCommandArgs = ValidCommandArgsHelper<TList, Args...>::value;