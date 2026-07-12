#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

// Pure mobutils::GetSubJobStats rank×level scaling curves.
// Parity: internal/mobutils GetSubJobStats (slice 1597 production wire).

namespace subjobstatshelpers
{

// rank 1–7 (A–G); other ranks → stat/2 truncated.
inline auto GetSubJobStats(const std::uint8_t rank, const std::uint16_t level, const std::uint16_t stat) -> std::uint16_t
{
    // These ranks A through G are used by all known JP sources. Please note this is equivalent to the US usage of A+ through F
    // https://w.atwiki.jp/studiogobli/pages/27.html
    float sJobStat = 0.0f;
    const float s  = static_cast<float>(stat);
    const float lv = static_cast<float>(level);

    switch (rank)
    {
        case 1: // A
            if (level <= 30)
            {
                sJobStat = std::max((std::floor(s / (4.0f - 0.225f * (lv - 30.0f)))), 2.0f);
            }
            else if (level <= 40)
            {
                sJobStat = std::floor(s / (3.25f - 0.073f * (lv - 30.0f)));
            }
            else if (level <= 46)
            {
                sJobStat = std::floor(s / (2.55f - 0.001f * (lv - 41.0f)));
            }
            else
            {
                sJobStat = std::floor(s / (2.7f - 0.001f * (lv - 45.0f)));
            }
            break;

        case 2: // B
            if (level <= 30)
            {
                sJobStat = std::max((std::floor(s / (3.1f - 0.075f * (lv - 32.0f)))), 2.0f);
            }
            else if (level <= 40)
            {
                sJobStat = std::floor(s / (3.1f - 0.075f * (lv - 32.0f)));
            }
            else if (level <= 45)
            {
                sJobStat = std::floor(s / (2.5f - 0.025f * (lv - 40.0f)));
            }
            else
            {
                sJobStat = std::floor(s / (2.35f - 0.04f * (lv - 44.0f)));
            }
            break;

        case 3: // C
            if (level <= 30)
            {
                sJobStat = std::max((std::floor(s / (4.5f - 0.15f * (lv - 26.0f)))), 2.0f);
            }
            else if (level <= 40)
            {
                sJobStat = std::floor(s / (3.28f - 0.001f * (lv - 30.0f)));
            }
            else if (level <= 45)
            {
                sJobStat = std::floor(s / (2.6f - 0.025f * (lv - 40.0f)));
            }
            else
            {
                sJobStat = std::floor(s / (2.1f - 0.2f * (lv - 49.0f)));
            }
            break;

        case 4: // D
            if (level <= 30)
            {
                sJobStat = std::max((std::floor(s / (5.0f - 0.05f * (lv - 21.0f)))), 1.0f);
            }
            else if (level <= 40)
            {
                sJobStat = std::floor(s / (3.2f - 0.001f * (lv - 29.0f)));
            }
            else if (level <= 45)
            {
                sJobStat = std::floor(s / (3.5f - 0.08f * (lv - 32.0f)));
            }
            else
            {
                sJobStat = std::floor(s / (3.25f - 0.045f * (lv - 32.0f)));
            }
            break;

        case 5: // E
            if (level <= 30)
            {
                sJobStat = std::max((std::floor(s / (3.8f - 0.1f * (lv - 32.0f)))), 1.0f);
            }
            else if (level <= 40)
            {
                sJobStat = std::floor(s / (3.8f - 0.15f * (lv - 32.0f)));
            }
            else if (level <= 45)
            {
                sJobStat = std::floor(s / (2.7f - 0.075f * (lv - 40.0f)));
            }
            else
            {
                sJobStat = std::floor(s / (2.7f - 0.05f * (lv - 45.0f)));
            }
            break;

        case 6: // F
            if (level <= 30)
            {
                sJobStat = std::max((std::floor(s / (4.0f - 0.15f * (lv - 35.0f)))), 1.0f);
            }
            else if (level <= 40)
            {
                sJobStat = std::floor(s / (4.0f - 0.15f * (lv - 30.0f)));
            }
            else if (level <= 46)
            {
                sJobStat = std::floor(s / (3.0f - 0.1125f * (lv - 40.0f)));
            }
            else
            {
                sJobStat = std::floor(s / (3.0f - 0.07f * (lv - 40.0f)));
            }
            break;

        case 7: // G
            if (level <= 30)
            {
                sJobStat = std::max((std::floor(s / (4.0f - 0.15f * (lv - 35.0f)))), 1.0f);
            }
            else if (level <= 40)
            {
                sJobStat = std::floor(s / (4.0f - 0.2f * (lv - 31.0f)));
            }
            else if (level <= 46)
            {
                sJobStat = std::floor(s / (2.5f - 0.09f * (lv - 40.0f)));
            }
            else
            {
                sJobStat = std::floor(s / 2.0f);
            }
            break;

        default:
            sJobStat = s / 2.0f;
            break;
    }

    return static_cast<std::uint16_t>(sJobStat);
}

} // namespace subjobstatshelpers
