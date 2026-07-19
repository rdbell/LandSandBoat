#pragma once

#include <cstdint>

namespace automatoncontrollercooldown
{
struct Plan
{
    uint16_t ranged;
    uint16_t shieldBash;
    uint16_t magic;
    uint16_t enfeeble;
    uint16_t heal;
    uint16_t elemental;
    uint16_t status;
    uint16_t enhance;
};

inline auto Magic(uint8_t head) -> Plan
{
    switch (head)
    {
        case 0x01: return { 0, 0, 10, 12, 12, 0, 0, 0 };
        case 0x02: return { 0, 0, 10, 0, 20, 0, 0, 0 };
        case 0x03: return { 0, 0, 10, 12, 20, 0, 0, 0 };
        case 0x04: return { 0, 0, 8, 10, 20, 25, 0, 25 };
        case 0x05: return { 0, 0, 8, 10, 10, 0, 10, 25 };
        case 0x06: return { 0, 0, 8, 10, 0, 30, 0, 35 };
        default: return {};
    }
}
} // namespace automatoncontrollercooldown
