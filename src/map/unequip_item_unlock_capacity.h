#pragma once

#include <cstdint>

namespace unequipitemunlockhelpers
{
struct Plan
{
    bool    setItemSubtype{};
    uint8_t subtype{};
};

constexpr Plan PlanFor()
{
    return { .setItemSubtype = true, .subtype = 0xFE };
}
} // namespace unequipitemunlockhelpers
