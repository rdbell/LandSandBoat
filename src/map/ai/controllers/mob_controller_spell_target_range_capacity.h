#pragma once

namespace mobcontrollerspelltargetrange
{
inline auto IsInRange(float distance, float spellRange, float casterHitbox, float targetHitbox) -> bool
{
    return distance <= spellRange + casterHitbox + targetHitbox;
}
} // namespace mobcontrollerspelltargetrange
