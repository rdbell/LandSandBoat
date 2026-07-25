#pragma once

#include <array>
#include <cstdint>

namespace savejobchangegearhelpers
{
struct Snapshot
{
    uint16_t main{};
    uint16_t sub{};
    uint16_t ranged{};
    uint16_t ammo{};
    uint16_t head{};
    uint16_t body{};
    uint16_t hands{};
    uint16_t legs{};
    uint16_t feet{};
    uint16_t neck{};
    uint16_t waist{};
    uint16_t ear1{};
    uint16_t ear2{};
    uint16_t ring1{};
    uint16_t ring2{};
    uint16_t back{};
};

constexpr Snapshot SnapshotFor(const std::array<uint16_t, 16>& ids)
{
    return {
        .main = ids[0], .sub = ids[1], .ranged = ids[2], .ammo = ids[3],
        .head = ids[4], .body = ids[5], .hands = ids[6], .legs = ids[7], .feet = ids[8],
        .neck = ids[9], .waist = ids[10], .ear1 = ids[11], .ear2 = ids[12],
        .ring1 = ids[13], .ring2 = ids[14], .back = ids[15],
    };
}
} // namespace savejobchangegearhelpers
