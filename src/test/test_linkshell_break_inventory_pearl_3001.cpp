#include "test_linkshell_break_inventory_pearl_3001.h"

#include "map/linkshell_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "linkshell ShouldBreakInventoryPearl 3001 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CLinkshell::RemoveMemberByName inventory break gate for dual-wire
// cross-check (slice 3001):
//   requesterRank == LSTYPE_LINKSHELL || isEquippedItem
auto inlineShouldBreakInventoryPearl(const uint8 requesterRank, const bool isEquippedItem) -> bool
{
    return requesterRank == LSTYPE_LINKSHELL || isEquippedItem;
}

} // namespace

// Pure dual-wire expansion for linkshellhelpers::ShouldBreakInventoryPearl
// (RemoveMemberByName inventory break gate; slice 3001).
auto runLinkshellBreakInventoryPearl3001SelfTests() -> bool
{
    using linkshellhelpers::ShouldBreakInventoryPearl;

    bool ok = true;

    // Residual 1354 pins still hold under dual-wire.
    ok = expect(ShouldBreakInventoryPearl(LSTYPE_LINKSHELL, false), "residual shell break all") && ok;
    ok = expect(ShouldBreakInventoryPearl(LSTYPE_PEARLSACK, true), "residual equipped only") && ok;
    ok = expect(!ShouldBreakInventoryPearl(LSTYPE_PEARLSACK, false), "residual not equipped") && ok;

    const struct
    {
        uint8       requesterRank;
        bool        isEquippedItem;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic accept poles — shell holder breaks everything.
        { LSTYPE_LINKSHELL, false, true, "shell + unequipped breaks" },
        { LSTYPE_LINKSHELL, true, true, "shell + equipped breaks" },

        // Non-shell: only equipped item.
        { LSTYPE_PEARLSACK, true, true, "sack + equipped breaks" },
        { LSTYPE_PEARLSACK, false, false, "sack + unequipped skips" },
        { LSTYPE_LINKPEARL, true, true, "pearl + equipped breaks" },
        { LSTYPE_LINKPEARL, false, false, "pearl + unequipped skips" },

        // Broken / new ranks are not shell holders.
        { LSTYPE_BROKEN, true, true, "broken + equipped breaks" },
        { LSTYPE_BROKEN, false, false, "broken + unequipped skips" },
        { LSTYPE_NEW_LINKSHELL, true, true, "new + equipped breaks" },
        { LSTYPE_NEW_LINKSHELL, false, false, "new + unequipped skips" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldBreakInventoryPearl(c.requesterRank, c.isEquippedItem);
        const bool inlineF = inlineShouldBreakInventoryPearl(c.requesterRank, c.isEquippedItem);
        const bool wantPin = c.requesterRank == LSTYPE_LINKSHELL || c.isEquippedItem;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldBreakInventoryPearl dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldBreakInventoryPearl == pin formula") && ok;
    }

    // Pin composition: shell OR equipped.
    ok = expect(ShouldBreakInventoryPearl(LSTYPE_LINKSHELL, false), "shell holder must break even unequipped") && ok;
    ok = expect(ShouldBreakInventoryPearl(LSTYPE_PEARLSACK, true), "equipped item must break for non-shell") && ok;
    ok = expect(!ShouldBreakInventoryPearl(LSTYPE_PEARLSACK, false), "non-shell unequipped must skip") && ok;

    // Dense compose: rank poles × equipped bool free == inline == pin.
    const uint8 ranks[] = {
        static_cast<uint8>(LSTYPE_NEW_LINKSHELL),
        static_cast<uint8>(LSTYPE_LINKSHELL),
        static_cast<uint8>(LSTYPE_PEARLSACK),
        static_cast<uint8>(LSTYPE_LINKPEARL),
        static_cast<uint8>(LSTYPE_BROKEN),
    };
    for (const uint8 rank : ranks)
    {
        for (const bool isEquippedItem : { false, true })
        {
            const bool got  = ShouldBreakInventoryPearl(rank, isEquippedItem);
            const bool want = rank == LSTYPE_LINKSHELL || isEquippedItem;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldBreakInventoryPearl(rank, isEquippedItem),
                        "compose free == inline") &&
                 ok;
        }
    }

    // --- Production CLinkshell::RemoveMemberByName path semantics ---
    // Host injects:
    //   requesterRank = remove requester LSTYPE
    //   isEquippedItem = (newPItemLinkshell == PItemLinkshell)
    // when true  → consider break (then ShouldMarkPearlBroken)
    // when false → leave inventory pearl intact
    ok = expect(ShouldBreakInventoryPearl(LSTYPE_LINKSHELL, false), "RemoveMemberByName shell → break all path") && ok;
    ok = expect(ShouldBreakInventoryPearl(LSTYPE_PEARLSACK, true), "RemoveMemberByName equipped → break path") && ok;
    ok = expect(!ShouldBreakInventoryPearl(LSTYPE_PEARLSACK, false), "RemoveMemberByName non-shell unequipped → skip break") && ok;

    // Host-style inject poles (host owns rank + pointer equality).
    const struct
    {
        uint8       requesterRank;
        bool        isEquippedItem;
        const char* label;
    } hostPoles[] = {
        { LSTYPE_LINKSHELL, false, "shell holder inventory scan → break" },
        { LSTYPE_LINKSHELL, true, "shell holder equipped slot → break" },
        { LSTYPE_PEARLSACK, true, "sack kick equipped pearl → break" },
        { LSTYPE_PEARLSACK, false, "sack kick other inventory pearl → skip" },
        { LSTYPE_LINKPEARL, true, "pearl requester equipped → break" },
        { LSTYPE_LINKPEARL, false, "pearl requester unequipped → skip" },
    };
    for (const auto& p : hostPoles)
    {
        const bool got     = ShouldBreakInventoryPearl(p.requesterRank, p.isEquippedItem);
        const bool inlineF = inlineShouldBreakInventoryPearl(p.requesterRank, p.isEquippedItem);
        const bool want    = p.requesterRank == LSTYPE_LINKSHELL || p.isEquippedItem;
        ok                 = expect(got == want, p.label) && ok;
        ok                 = expect(got == inlineF, "host inject dual-wire free == inline") && ok;
    }

    // Explicit dual-wire: free function is shell OR equipped of host inject.
    for (const uint8 rank : ranks)
    {
        for (const bool isEquippedItem : { false, true })
        {
            const bool want = rank == LSTYPE_LINKSHELL || isEquippedItem;
            ok              = expect(ShouldBreakInventoryPearl(rank, isEquippedItem) == want,
                        "host inject formula") &&
                 ok;
            ok = expect(ShouldBreakInventoryPearl(rank, isEquippedItem) ==
                            inlineShouldBreakInventoryPearl(rank, isEquippedItem),
                        "host inject dual-wire free == inline") &&
                 ok;
        }
    }

    return ok;
}
