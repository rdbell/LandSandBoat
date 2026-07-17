#include "test_monstrosity_instinct_equip_2766.h"

#include "map/monstrosity_instinct_equip.h"

#include <array>
#include <iostream>

auto runMonstrosityInstinctEquip2766SelfTests() -> bool
{
    using monstrosity::HasDuplicateInstincts;
    using monstrosity::InstinctMaxPoints;
    using monstrosity::IsPurchasableInstinctIndex;
    using monstrosity::IsValidPurchasableInstinctByteOffset;
    using monstrosity::PurchasableInstinctBitShift;
    using monstrosity::PurchasableInstinctByteOffset;
    using monstrosity::ShouldCheckVariantUnlock;
    using monstrosity::ShouldRejectInstinctLoadout;
    using monstrosity::ShouldRejectUnleveledSpecies;
    using monstrosity::ShouldWipeInstinctsOnFamilyChange;
    using monstrosity::TotalInstinctCost;

    if (InstinctMaxPoints(0) != 10 || InstinctMaxPoints(1) != 11 || InstinctMaxPoints(99) != 109 || InstinctMaxPoints(255) != 265)
    {
        std::cerr << "monstrosity instinct equip: InstinctMaxPoints failed\n";
        return false;
    }

    {
        const std::array<uint8, 12> costs{ 1, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        if (TotalInstinctCost(costs) != 6)
        {
            std::cerr << "monstrosity instinct equip: TotalInstinctCost failed\n";
            return false;
        }
        const std::array<uint8, 12> zeros{};
        if (TotalInstinctCost(zeros) != 0)
        {
            std::cerr << "monstrosity instinct equip: TotalInstinctCost zeros failed\n";
            return false;
        }
    }

    {
        const std::array<uint16, 12> unique{ 1, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        const std::array<uint16, 12> empty{};
        const std::array<uint16, 12> dup{ 5, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        if (HasDuplicateInstincts(unique) || HasDuplicateInstincts(empty) || !HasDuplicateInstincts(dup))
        {
            std::cerr << "monstrosity instinct equip: HasDuplicateInstincts failed\n";
            return false;
        }
    }

    if (ShouldRejectInstinctLoadout(10, 11, false) || !ShouldRejectInstinctLoadout(12, 11, false) ||
        !ShouldRejectInstinctLoadout(0, 11, true) || ShouldRejectInstinctLoadout(11, 11, false))
    {
        std::cerr << "monstrosity instinct equip: ShouldRejectInstinctLoadout failed\n";
        return false;
    }

    if (IsPurchasableInstinctIndex(767) || !IsPurchasableInstinctIndex(768) || !IsPurchasableInstinctIndex(792))
    {
        std::cerr << "monstrosity instinct equip: IsPurchasableInstinctIndex failed\n";
        return false;
    }

    if (PurchasableInstinctByteOffset(768) != 20 || PurchasableInstinctBitShift(768) != 0 ||
        PurchasableInstinctByteOffset(773) != 20 || PurchasableInstinctBitShift(773) != 5 ||
        PurchasableInstinctByteOffset(792) != 23 || PurchasableInstinctByteOffset(800) != 24)
    {
        std::cerr << "monstrosity instinct equip: purchasable offset/shift failed\n";
        return false;
    }

    if (IsValidPurchasableInstinctByteOffset(19) || !IsValidPurchasableInstinctByteOffset(20) ||
        !IsValidPurchasableInstinctByteOffset(23) || IsValidPurchasableInstinctByteOffset(24))
    {
        std::cerr << "monstrosity instinct equip: IsValidPurchasableInstinctByteOffset failed\n";
        return false;
    }

    if (!ShouldRejectUnleveledSpecies(0) || ShouldRejectUnleveledSpecies(1))
    {
        std::cerr << "monstrosity instinct equip: ShouldRejectUnleveledSpecies failed\n";
        return false;
    }

    if (ShouldCheckVariantUnlock(255) || !ShouldCheckVariantUnlock(256) || !ShouldCheckVariantUnlock(300))
    {
        std::cerr << "monstrosity instinct equip: ShouldCheckVariantUnlock failed\n";
        return false;
    }

    if (!ShouldWipeInstinctsOnFamilyChange(1, 2) || ShouldWipeInstinctsOnFamilyChange(5, 5))
    {
        std::cerr << "monstrosity instinct equip: ShouldWipeInstinctsOnFamilyChange failed\n";
        return false;
    }

    return true;
}
