#pragma once

namespace synthdonehelpers
{

enum class Completion
{
    None,
    Failure,
    Success,
};

struct Plan
{
    Completion completion{ Completion::None };
    bool       skillUp{};
    bool       commitTransaction{};
    bool       removeTransaction{};
    bool       resetStatus{};

    constexpr auto operator==(const Plan&) const -> bool = default;
};

constexpr auto MakePlan(const bool hasTransaction, const bool synthesisFailed) -> Plan
{
    if (!hasTransaction)
    {
        return { .resetStatus = true };
    }

    return {
        .completion        = synthesisFailed ? Completion::Failure : Completion::Success,
        .skillUp           = true,
        .commitTransaction = true,
        .removeTransaction = true,
        .resetStatus       = true,
    };
}

} // namespace synthdonehelpers
