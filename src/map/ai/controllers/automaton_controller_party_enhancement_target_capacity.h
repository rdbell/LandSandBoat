#pragma once

namespace automatoncontrollerpartyenhancementtarget
{
inline auto CanConsiderPartyEnhancement(bool notMaster, bool withinRange, bool targetIsMob, bool hasEnmity) -> bool
{
    return notMaster && withinRange && (!targetIsMob || hasEnmity);
}
} // namespace automatoncontrollerpartyenhancementtarget
