#pragma once
namespace targetfindplayerhelpers
{
constexpr auto IsPlayerTarget(const bool targetExists, const bool targetIsPlayer, const bool masterIsPlayer) -> bool
{
    return targetExists && (targetIsPlayer || masterIsPlayer);
}
} // namespace targetfindplayerhelpers
