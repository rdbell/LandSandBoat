/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_s2c_item_attr_runtime.h"

#include <algorithm>
#include <iostream>

#include "map/item_container.h"
#include "map/packets/s2c/item_attr_runtime.h"

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "s2c ITEM_ATTR runtime self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runS2CItemAttrRuntimeSelfTests() -> bool
{
    using namespace itemattrhelpers;

    bool ok = true;

    const auto empty = PlanFor(LOC_STORAGE, 4, {});
    ok = expect(empty.Category == LOC_STORAGE && empty.ItemIndex == 4 && empty.ItemNum == 0 && empty.Price == 0 && empty.ItemNo == 0 && empty.LockFlg == ItemLockFlg::Normal && std::all_of(std::begin(empty.Attr), std::end(empty.Attr), [](const uint8 value) { return value == 0; }), "nil item clears slot without leaking extdata") && ok;

    auto staleFacts           = Facts{};
    staleFacts.hasStaleItem   = true;
    staleFacts.leakStaleExtra = true;
    staleFacts.staleExtra[0]  = 0xA1;
    staleFacts.staleExtra[23] = 0xB2;
    const auto stale = PlanFor(LOC_STORAGE, 4, staleFacts);
    ok = expect(std::equal(std::begin(stale.Attr), std::end(stale.Attr), staleFacts.staleExtra.begin()), "configured stale item leaks extdata only") && ok;

    auto charged            = Facts{};
    charged.hasItem         = true;
    charged.charged         = true;
    charged.currentCharges  = 2;
    charged.maxCharges      = 3;
    charged.reuseReady      = true;
    charged.extra[1]        = 0xCC;
    const auto ready = PlanFor(LOC_INVENTORY, 1, charged);
    ok = expect(ready.Attr[0] == 0x01 && ready.Attr[1] == 0xCC && ready.Attr[3] == 0xD0, "charged ready item marks remaining charges and reuse") && ok;

    charged.currentCharges    = 1;
    charged.maxCharges        = 1;
    charged.reuseReady        = false;
    charged.nextUseTimestamp  = 0x11223344;
    charged.delayTimestamp    = 0x55667788;
    const auto cooling = PlanFor(LOC_INVENTORY, 1, charged);
    ok = expect(cooling.Attr[3] == 0x80 && cooling.Attr[4] == 0x44 && cooling.Attr[5] == 0x33 && cooling.Attr[6] == 0x22 && cooling.Attr[7] == 0x11 && cooling.Attr[8] == 0x88 && cooling.Attr[9] == 0x77 && cooling.Attr[10] == 0x66 && cooling.Attr[11] == 0x55, "charged cooling item stores timestamps little-endian") && ok;

    charged.currentCharges = 0;
    charged.maxCharges     = 3;
    const auto emptyCharges = PlanFor(LOC_INVENTORY, 1, charged);
    ok = expect(emptyCharges.Attr[3] == 0xB0, "empty charged item marks recharge and exhausted") && ok;

    charged.currentCharges   = 2;
    charged.maxCharges       = 3;
    charged.reuseReady       = true;
    charged.unlockableWeapon = true;
    const auto unlockable = PlanFor(LOC_INVENTORY, 1, charged);
    ok = expect(unlockable.Attr[0] == 0 && unlockable.Attr[1] == 0 && unlockable.Attr[3] == 0xD0, "unlockable weapon clears charge markers after charge shaping") && ok;

    auto locked    = Facts{};
    locked.hasItem = true;
    locked.locked  = true;
    ok = expect(PlanFor(LOC_INVENTORY, 1, locked).LockFlg == ItemLockFlg::NoDrop, "ordinary locked item is no-drop") && ok;
    locked.linkshell = true;
    ok = expect(PlanFor(LOC_INVENTORY, 1, locked).LockFlg == ItemLockFlg::Linkshell, "locked linkshell uses linkshell flag") && ok;
    locked.price = 1;
    ok = expect(PlanFor(LOC_INVENTORY, 1, locked).LockFlg == ItemLockFlg::Unknown0, "price overrides item lock flag") && ok;

    locked.price         = 0;
    locked.linkshellType = 0x7E;
    const auto linkshell = PlanFor(LOC_INVENTORY, 1, locked);
    ok = expect(linkshell.Attr[8] == 0x7E, "linkshell type occupies extdata byte eight") && ok;
    return ok;
}
