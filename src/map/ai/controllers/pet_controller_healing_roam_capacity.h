#pragma once

namespace petcontrollerhealingroam
{
inline auto ShouldHold(bool isBstPet, bool isHealing) -> bool
{
    return isBstPet && isHealing;
}
} // namespace petcontrollerhealingroam
