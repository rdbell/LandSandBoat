#pragma once

#include <cstdint>

namespace charvarentityopshelpers
{
enum class ReadAction : std::uint8_t
{
    ReturnZero,
    ReadLocalCache,
};

// ReadActionFor preserves GetCharVar's null-character behavior.
constexpr auto ReadActionFor(const bool hasCharacter) -> ReadAction
{
    if (!hasCharacter)
    {
        return ReadAction::ReturnZero;
    }

    return ReadAction::ReadLocalCache;
}

enum class SetAction : std::uint8_t
{
    Noop,
    UpdateLocalCache,
};

// SetActionFor preserves the entity overload of SetCharVar.
constexpr auto SetActionFor(const bool hasCharacter) -> SetAction
{
    if (!hasCharacter)
    {
        return SetAction::Noop;
    }

    return SetAction::UpdateLocalCache;
}

enum class ClearPrefixAction : std::uint8_t
{
    ReturnZero,
    ClearLocalCache,
};

// ClearPrefixActionFor preserves ClearCharVarsWithPrefix's null-character behavior.
constexpr auto ClearPrefixActionFor(const bool hasCharacter) -> ClearPrefixAction
{
    if (!hasCharacter)
    {
        return ClearPrefixAction::ReturnZero;
    }

    return ClearPrefixAction::ClearLocalCache;
}
} // namespace charvarentityopshelpers
