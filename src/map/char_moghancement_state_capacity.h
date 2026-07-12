#pragma once

#include <functional>
#include <utility>

namespace charmoghancementstatehelpers
{

template <typename ID>
inline bool Has(const ID currentID, const ID requestedID)
{
    return currentID == requestedID;
}

template <typename ID, typename Change>
inline void Set(ID& currentID, const ID newID, Change&& change)
{
    std::invoke(change, currentID, false);
    std::invoke(change, newID, true);
    currentID = newID;
}

} // namespace charmoghancementstatehelpers
