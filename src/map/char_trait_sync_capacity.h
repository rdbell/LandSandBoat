#pragma once

#include <cstdint>
#include <functional>
#include <utility>

namespace chartraitsynchelpers
{

template <typename BaseChange, typename CharacterChange>
inline void Apply(const std::uint16_t traitID, BaseChange&& baseChange, CharacterChange&& characterChange)
{
    std::invoke(std::forward<BaseChange>(baseChange));
    std::invoke(std::forward<CharacterChange>(characterChange), traitID);
}

} // namespace chartraitsynchelpers
