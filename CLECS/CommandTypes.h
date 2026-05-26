#pragma once
#include <type_traits>

template<typename... Components, typename... Args>
concept ValidCommandArgs = requires {sizeof...(Components) == sizeof...(Args) && (std::is_same_v<std::remove_cvref_t<Args>, Components> && ...); };