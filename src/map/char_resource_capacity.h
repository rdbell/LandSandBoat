#pragma once

namespace charresourcehelpers
{

template <typename Value, typename BaseAdd, typename CheckLatents>
inline auto Apply(const Value requested, BaseAdd&& baseAdd, CheckLatents&& checkLatents) -> Value
{
    const Value applied = baseAdd(requested);
    checkLatents();
    return applied < 0 ? -applied : applied;
}

} // namespace charresourcehelpers
