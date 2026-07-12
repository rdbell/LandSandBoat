#pragma once

#include <cstdint>
#include <functional>

namespace charitemfinishcompletehelpers
{

template <typename SetCurrentCharges, typename SetLastUseTime, typename PersistExtra, typename AddRecast>
inline bool Apply(const bool equipment,
                  const std::uint8_t maxCharges,
                  const std::uint8_t currentCharges,
                  const std::uint8_t slotID,
                  const std::uint8_t locationID,
                  SetCurrentCharges&& setCurrentCharges,
                  SetLastUseTime&&    setLastUseTime,
                  PersistExtra&&      persistExtra,
                  AddRecast&&         addRecast)
{
    if (!equipment)
    {
        return true;
    }

    auto remainingCharges = currentCharges;
    if (maxCharges > 1)
    {
        remainingCharges = static_cast<std::uint8_t>(currentCharges - 1);
        std::invoke(setCurrentCharges, remainingCharges);
    }
    std::invoke(setLastUseTime);
    std::invoke(persistExtra);
    if (remainingCharges != 0)
    {
        const auto key = static_cast<std::uint16_t>(slotID << 8 | locationID);
        std::invoke(addRecast, key);
    }
    return false;
}

} // namespace charitemfinishcompletehelpers
