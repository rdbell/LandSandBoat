#include "test_s2c_equip_inspect_runtime.h"

#include <iostream>

#include "map/packets/s2c/0x0c9_equip_inspect_equipment.h"

namespace
{

auto expectPlan(const equipinspecthelpers::CheckItemPlan& actual, const equipinspecthelpers::CheckItemPlan& expected, const char* label) -> bool
{
    if (actual.itemNo != expected.itemNo || actual.equipKind != expected.equipKind || actual.data != expected.data)
    {
        std::cerr << "s2c EQUIP_INSPECT runtime self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runS2CEquipInspectRuntimeSelfTests() -> bool
{
    using namespace equipinspecthelpers;

    auto plain       = CheckItemFacts{ .itemNo = 0x1234, .equipKind = 4 };
    plain.signature  = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
    auto plainWanted = CheckItemPlan{ .itemNo = 0x1234, .equipKind = 4 };
    for (std::size_t index = 0; index < plain.signature.size(); ++index)
    {
        plainWanted.data[12 + index] = plain.signature[index];
    }

    bool ok                   = expectPlan(CheckItemPlanFor(plain), plainWanted, "plain item copies signature");
    auto charged              = plain;
    charged.charged           = true;
    charged.currentCharges    = 7;
    charged.nextUseIsFuture   = true;
    charged.nextUseTimestamp  = 0x11223344;
    charged.useDelayTimestamp = 0x55667788;
    auto chargedWanted        = plainWanted;
    chargedWanted.data[0]     = 1;
    chargedWanted.data[1]     = 7;
    chargedWanted.data[3]     = 0x90;
    PutUint32LE(chargedWanted.data, 4, 0x11223344);
    PutUint32LE(chargedWanted.data, 8, 0x55667788);
    ok = expectPlan(CheckItemPlanFor(charged), chargedWanted, "charged ready item") && ok;

    charged.nextUseIsFuture = false;
    chargedWanted.data[3]   = 0xD0;
    ok                      = expectPlan(CheckItemPlanFor(charged), chargedWanted, "charged cooldown item") && ok;

    auto augmented        = plain;
    augmented.augmented   = true;
    augmented.augments    = { 0x0102, 0x0304, 0x0506, 0x0708 };
    auto augmentWanted    = plainWanted;
    augmentWanted.data[0] = 2;
    for (std::size_t index = 0; index < augmented.augments.size(); ++index)
    {
        PutUint16LE(augmentWanted.data, 2 + index * 2, augmented.augments[index]);
    }
    ok = expectPlan(CheckItemPlanFor(augmented), augmentWanted, "augmented item") && ok;

    charged.augmented  = true;
    charged.augments   = augmented.augments;
    auto bothWanted    = chargedWanted;
    bothWanted.data[0] = 2;
    for (std::size_t index = 0; index < charged.augments.size(); ++index)
    {
        PutUint16LE(bothWanted.data, 2 + index * 2, charged.augments[index]);
    }
    bothWanted.data[1] = 7;
    ok                 = expectPlan(CheckItemPlanFor(charged), bothWanted, "augment precedence retains charged byte") && ok;
    return ok;
}
