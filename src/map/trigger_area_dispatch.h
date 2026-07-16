#pragma once

// The membership transition for one character/trigger-area pair.  Keeping
// this decision separate lets the zone host retain ownership of its character
// cache and Lua callbacks.
namespace triggerarea
{

enum class MembershipAction
{
    None,
    Enter,
    Leave,
};

constexpr auto MembershipActionFor(bool isZoningIn, bool isInside, bool isMember) -> MembershipAction
{
    if (isZoningIn || (isInside && isMember) || (!isInside && !isMember))
    {
        return MembershipAction::None;
    }

    return isInside ? MembershipAction::Enter : MembershipAction::Leave;
}

} // namespace triggerarea
