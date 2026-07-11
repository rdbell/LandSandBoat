#pragma once

namespace petabilityhelpers
{

template <typename Message, typename Value>
struct Result
{
    Message message;
    Value   param;
};

template <typename Message, typename Value, typename AbsorbVariant>
inline auto FinalizeResult(
    const Message previousMessage,
    Message currentMessage,
    const Message abilityMessage,
    const Message noneMessage,
    const Message usesJobAbilityMessage,
    const Value value,
    AbsorbVariant&& absorbVariant) -> Result<Message, Value>
{
    if (currentMessage == previousMessage)
    {
        currentMessage = abilityMessage;
    }
    if (currentMessage == noneMessage)
    {
        currentMessage = usesJobAbilityMessage;
    }

    auto param = value;
    if (value < Value{})
    {
        currentMessage = absorbVariant(currentMessage);
        param          = -value;
    }
    return { currentMessage, param };
}

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
