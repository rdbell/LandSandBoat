#pragma once

namespace automatoncontrollerhealingmasterdistance
{
inline auto ShouldMeasureDistance(bool hasHate, int automatonHPP, int masterHPP, float threshold) -> bool
{
    return hasHate && automatonHPP > 50 && masterHPP <= threshold;
}
} // namespace automatoncontrollerhealingmasterdistance
