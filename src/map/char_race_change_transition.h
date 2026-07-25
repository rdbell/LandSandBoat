#pragma once

// Pure two-phase raceChange policy.
namespace racechangetransitionhelpers
{
struct RaceChangeValidationPlan
{
    bool attemptLookUpdate{};
    bool reportInvalidArguments{};
    constexpr auto operator==(const RaceChangeValidationPlan&) const -> bool = default;
};
constexpr auto MakeRaceChangeValidationPlan(const bool hasCharacter, const bool argsInBounds) -> RaceChangeValidationPlan
{
    if (!hasCharacter) return {};
    if (!argsInBounds) return { .reportInvalidArguments = true };
    return { .attemptLookUpdate = true };
}
struct RaceChangeCompletionPlan
{
    bool succeeded{};
    bool reportLookUpdateFailure{};
    bool forceRezone{};
    constexpr auto operator==(const RaceChangeCompletionPlan&) const -> bool = default;
};
constexpr auto MakeRaceChangeCompletionPlan(const bool lookUpdateSucceeded) -> RaceChangeCompletionPlan
{
    if (!lookUpdateSucceeded) return { .reportLookUpdateFailure = true };
    return { .succeeded = true, .forceRezone = true };
}
} // namespace racechangetransitionhelpers
