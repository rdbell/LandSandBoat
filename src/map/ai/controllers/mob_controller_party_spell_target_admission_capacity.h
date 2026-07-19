#pragma once

namespace mobcontrollerpartyspelltargetadmission
{
// CanUse reports whether a selected party target shares the caster's combat state.
constexpr auto CanUse(const bool casterEngaged, const bool targetEngaged) -> bool
{
    return casterEngaged == targetEngaged;
}
} // namespace mobcontrollerpartyspelltargetadmission
