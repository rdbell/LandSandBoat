#pragma once

#include <algorithm>
#include <cstdint>

// Pure attackutils/battleutils getHitCount CDF after RNG inject.
// Parity: internal/attackutils hit_count.go
//
// distribution is xirand::GetRandomNumber(100) → 0..99.
// Result is always in 1..8. hits > 8 leaves num==1 (no switch case).

namespace hitcounthelpers
{

constexpr auto GetHitCount(const std::uint8_t hits, const std::uint8_t distribution) -> std::uint8_t
{
    std::uint8_t num = 1;

    switch (hits)
    {
        case 0:
        case 1:
            break;
        case 2: // cdf = 55,100
            if (distribution >= 55)
            {
                num = static_cast<std::uint8_t>(num + 1);
            }
            break;
        case 3: // cdf = 30,80,100
            if (distribution < 30)
            {
                break;
            }
            if (distribution < 80)
            {
                num = static_cast<std::uint8_t>(num + 1);
            }
            else
            {
                num = static_cast<std::uint8_t>(num + 2);
            }
            break;
        case 4: // cdf = 20,50,80,100
            if (distribution < 20)
            {
                break;
            }
            if (distribution < 50)
            {
                num = static_cast<std::uint8_t>(num + 1);
            }
            else if (distribution < 80)
            {
                num = static_cast<std::uint8_t>(num + 2);
            }
            else
            {
                num = static_cast<std::uint8_t>(num + 3);
            }
            break;
        case 5: // cdf = 10,30,60,90,100
            if (distribution < 10)
            {
                break;
            }
            if (distribution < 30)
            {
                num = static_cast<std::uint8_t>(num + 1);
            }
            else if (distribution < 60)
            {
                num = static_cast<std::uint8_t>(num + 2);
            }
            else if (distribution < 90)
            {
                num = static_cast<std::uint8_t>(num + 3);
            }
            else
            {
                num = static_cast<std::uint8_t>(num + 4);
            }
            break;
        case 6: // cdf = 10,30,50,70,90,100
            if (distribution < 10)
            {
                break;
            }
            if (distribution < 30)
            {
                num = static_cast<std::uint8_t>(num + 1);
            }
            else if (distribution < 50)
            {
                num = static_cast<std::uint8_t>(num + 2);
            }
            else if (distribution < 70)
            {
                num = static_cast<std::uint8_t>(num + 3);
            }
            else if (distribution < 90)
            {
                num = static_cast<std::uint8_t>(num + 4);
            }
            else
            {
                num = static_cast<std::uint8_t>(num + 5);
            }
            break;
        case 7: // cdf = 5,20,45,70,85,95,100
            if (distribution < 5)
            {
                break;
            }
            if (distribution < 20)
            {
                num = static_cast<std::uint8_t>(num + 1);
            }
            else if (distribution < 45)
            {
                num = static_cast<std::uint8_t>(num + 2);
            }
            else if (distribution < 70)
            {
                num = static_cast<std::uint8_t>(num + 3);
            }
            else if (distribution < 85)
            {
                num = static_cast<std::uint8_t>(num + 4);
            }
            else if (distribution < 95)
            {
                num = static_cast<std::uint8_t>(num + 5);
            }
            else
            {
                num = static_cast<std::uint8_t>(num + 6);
            }
            break;
        case 8: // cdf = 5,20,45,70,85,95,98,100
            if (distribution < 5)
            {
                break;
            }
            if (distribution < 20)
            {
                num = static_cast<std::uint8_t>(num + 1);
            }
            else if (distribution < 45)
            {
                num = static_cast<std::uint8_t>(num + 2);
            }
            else if (distribution < 70)
            {
                num = static_cast<std::uint8_t>(num + 3);
            }
            else if (distribution < 85)
            {
                num = static_cast<std::uint8_t>(num + 4);
            }
            else if (distribution < 95)
            {
                num = static_cast<std::uint8_t>(num + 5);
            }
            else if (distribution < 98)
            {
                num = static_cast<std::uint8_t>(num + 6);
            }
            else
            {
                num = static_cast<std::uint8_t>(num + 7);
            }
            break;
        default:
            // hits > 8: no LSB switch case → num stays 1
            break;
    }

    return std::min<std::uint8_t>(num, 8);
}

} // namespace hitcounthelpers
