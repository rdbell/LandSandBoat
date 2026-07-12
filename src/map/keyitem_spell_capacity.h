#pragma once

#include <cstdint>
#include <utility>

// Pure key-item indexing and spell/learned-weaponskill gates from charutils.

namespace keyitemspellhelpers
{

// Key items are packed 512 per table.
constexpr std::uint16_t KeyItemsPerTable = 512;

// KeyItemTableIndex mirrors keyItemId / 512.
constexpr auto KeyItemTableIndex(const std::uint16_t keyItemId) -> std::uint16_t
{
    return static_cast<std::uint16_t>(keyItemId / KeyItemsPerTable);
}

// KeyItemBitIndex mirrors keyItemId % 512.
constexpr auto KeyItemBitIndex(const std::uint16_t keyItemId) -> std::uint16_t
{
    return static_cast<std::uint16_t>(keyItemId % KeyItemsPerTable);
}

// KeyItemTableInRange mirrors tableIndex < tablesSize.
constexpr auto KeyItemTableInRange(const std::uint16_t tableIndex, const std::size_t tablesSize) -> bool
{
    return tableIndex < tablesSize;
}

// ShouldAddSpell mirrors spell exists && !alreadyHas.
constexpr auto ShouldAddSpell(const bool spellExists, const bool alreadyHas) -> bool
{
    return spellExists && !alreadyHas;
}

// ShouldDelSpell mirrors spell exists && alreadyHas.
constexpr auto ShouldDelSpell(const bool spellExists, const bool alreadyHas) -> bool
{
    return spellExists && alreadyHas;
}

// SpellMutationResult: 1 success, 0 no-op (LSB int32 returns).
constexpr auto SpellMutationSuccess() -> std::int32_t
{
    return 1;
}

constexpr auto SpellMutationNoOp() -> std::int32_t
{
    return 0;
}

// LearnedWeaponskillInRange mirrors wsUnlockId <= size-1 (size > 0 assumed when checking).
constexpr auto LearnedWeaponskillInRange(const std::uint8_t wsUnlockId, const std::size_t learnedSize) -> bool
{
    if (learnedSize == 0)
    {
        return false;
    }
    return wsUnlockId <= learnedSize - 1;
}

// ShouldRejectNullChar mirrors PChar == nullptr for learned WS helpers.
constexpr auto ShouldRejectNullChar(const bool isNull) -> bool
{
    return isNull;
}

} // namespace keyitemspellhelpers
