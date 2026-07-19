#pragma once

namespace mobcontrollerpartylinkfamily
{
// CanLink reports whether a party member's family is eligible to link.
template <typename Family>
constexpr auto CanLink(const bool forceLink, const bool hasSubLink, const Family mobFamily, const Family partyMemberFamily) -> bool
{
    return forceLink || hasSubLink || mobFamily != partyMemberFamily;
}
} // namespace mobcontrollerpartylinkfamily
