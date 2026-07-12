#pragma once

#include <functional>
#include <utility>

namespace charstartsynthhelpers
{

template <typename InternalSynth>
inline bool Apply(const bool hasAI, InternalSynth&& internalSynth)
{
    if (!hasAI)
    {
        return false;
    }
    return std::invoke(std::forward<InternalSynth>(internalSynth));
}

} // namespace charstartsynthhelpers
