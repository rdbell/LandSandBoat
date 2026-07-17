#pragma once

#include "common/cbasetypes.h"
#include "monstrosity_capacity.h"

#include <array>
#include <cstddef>

namespace monstrosity
{

// Fixed equipped-instinct slot count (MonstrosityData_t::EquippedInstincts).
constexpr std::size_t InstinctSlotCount = 12;

// First purchasable instinct id (LSB IsInstinctUnlocked: instinct >= 768).
constexpr uint16 PurchasableInstinctBase = 768;

// Instincts bitpack gap used for purchasable unlock bits: bytes [20, 24).
constexpr uint8 PurchasableInstinctByteBase  = 20;
constexpr uint8 PurchasableInstinctByteLimit = 24;

// First species index treated as a variant (SpeciesIndex >= 256).
constexpr uint16 VariantSpeciesThreshold = 256;

// InstinctMaxPoints is the client point budget: level + 10.
constexpr auto InstinctMaxPoints(const uint8 level) -> uint16
{
    return static_cast<uint16>(level) + 10;
}

// TotalInstinctCost sums host-resolved per-slot costs (0 for empty / missing).
constexpr auto TotalInstinctCost(const std::array<uint8, InstinctSlotCount>& costs) -> uint16
{
    uint16 total = 0;
    for (const auto cost : costs)
    {
        total = static_cast<uint16>(total + cost);
    }
    return total;
}

// HasDuplicateInstincts skips empty (0) slots; true if any non-zero id repeats.
constexpr auto HasDuplicateInstincts(const std::array<uint16, InstinctSlotCount>& slots) -> bool
{
    for (std::size_t i = 0; i < InstinctSlotCount; ++i)
    {
        const auto id = slots[i];
        if (id == 0)
        {
            continue;
        }
        for (std::size_t j = i + 1; j < InstinctSlotCount; ++j)
        {
            if (slots[j] == id)
            {
                return true;
            }
        }
    }
    return false;
}

// ShouldRejectInstinctLoadout dual-wires monstrosityhelpers capacity (slice 2872):
// duplicates OR totalCost > maxPoints. Exact budget equality is accepted.
constexpr auto ShouldRejectInstinctLoadout(const uint16 totalCost, const uint16 maxPoints, const bool hasDuplicates) -> bool
{
    return monstrosityhelpers::ShouldRejectInstinctLoadout(totalCost, maxPoints, hasDuplicates);
}

// IsPurchasableInstinctIndex mirrors instinct >= 768.
constexpr auto IsPurchasableInstinctIndex(const uint16 instinct) -> bool
{
    return instinct >= PurchasableInstinctBase;
}

// PurchasableInstinctByteOffset is 20 + ((instinct - 768) / 8).
constexpr auto PurchasableInstinctByteOffset(const uint16 instinct) -> uint8
{
    const auto idx = static_cast<uint16>(instinct - PurchasableInstinctBase);
    return static_cast<uint8>(PurchasableInstinctByteBase + (idx / 8));
}

// PurchasableInstinctBitShift is (instinct - 768) % 8.
constexpr auto PurchasableInstinctBitShift(const uint16 instinct) -> uint8
{
    const auto idx = static_cast<uint16>(instinct - PurchasableInstinctBase);
    return static_cast<uint8>(idx % 8);
}

// IsValidPurchasableInstinctByteOffset: byteOffset in [20, 24).
constexpr auto IsValidPurchasableInstinctByteOffset(const uint8 byteOffset) -> bool
{
    return byteOffset >= PurchasableInstinctByteBase && byteOffset < PurchasableInstinctByteLimit;
}

// ShouldRejectUnleveledSpecies: levels[monstrosityId] == 0.
constexpr auto ShouldRejectUnleveledSpecies(const uint8 level) -> bool
{
    return level == 0;
}

// ShouldCheckVariantUnlock: SpeciesIndex >= 256.
constexpr auto ShouldCheckVariantUnlock(const uint16 speciesIndex) -> bool
{
    return speciesIndex >= VariantSpeciesThreshold;
}

// ShouldWipeInstinctsOnFamilyChange: previous MonstrosityId != new id.
constexpr auto ShouldWipeInstinctsOnFamilyChange(const uint8 previousMonstrosityId, const uint8 newId) -> bool
{
    return previousMonstrosityId != newId;
}

} // namespace monstrosity
