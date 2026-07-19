#pragma once

namespace trustcontrollertargetsyncadmission
{
// CanCheck reports whether target IDs may be compared.
template <typename IsMismatch>
constexpr auto CanCheck(const bool hasMaster, const bool hasMob, IsMismatch&& isMismatch) -> bool
{
    return hasMaster && hasMob && isMismatch();
}
} // namespace trustcontrollertargetsyncadmission
