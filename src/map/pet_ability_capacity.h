#pragma once

namespace petabilityhelpers
{

template <typename ValidTarget, typename OutOfRange, typename IsInstant, typename IsParalyzed,
          typename Execute, typename InterruptInvalid, typename InterruptParalyzed, typename ProcessEffectFlags>
inline void Apply(
    const bool hasTarget,
    ValidTarget&& validTarget,
    OutOfRange&& outOfRange,
    IsInstant&& isInstant,
    IsParalyzed&& isParalyzed,
    Execute&& execute,
    InterruptInvalid&& interruptInvalid,
    InterruptParalyzed&& interruptParalyzed,
    ProcessEffectFlags&& processEffectFlags)
{
    if (!hasTarget || !validTarget())
    {
        interruptInvalid();
        processEffectFlags();
        return;
    }
    if (outOfRange())
    {
        return;
    }
    if (isInstant() && isParalyzed())
    {
        interruptParalyzed();
        return;
    }
    execute();
    processEffectFlags();
}

} // namespace petabilityhelpers
