#include "test_linkshell_break_inventory_pearl_3264.h"

#include "map/linkshell_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "linkshell ShouldBreakInventoryPearl 3264 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CLinkshell::RemoveMemberByName inventory break gate for dual-wire
// cross-check (dedicated 3264):
//   requesterRank == LSTYPE_LINKSHELL || isEquippedItem
auto inlineShouldBreakInventoryPearl(const uint8 requesterRank, const bool isEquippedItem) -> bool
{
    return requesterRank == LSTYPE_LINKSHELL || isEquippedItem;
}

// Compact dual-wire pin matching Go pinShouldBreakInventoryPearl3264 / C++
// capacity shell-OR-equipped form (formula unchanged from 1354 / 3001):
//   requesterRank == LSTYPE_LINKSHELL || isEquippedItem
// Positive direct-return form (lint QF1001/SA1008-friendly).
auto pinShouldBreakInventoryPearl(const uint8 requesterRank, const bool isEquippedItem) -> bool
{
    return requesterRank == LSTYPE_LINKSHELL || isEquippedItem;
}

} // namespace

// Pure dual-wire expansion for linkshellhelpers::ShouldBreakInventoryPearl
// (shell holder OR equipped item; OmegaXI internal/linkshell; dedicated slice
// 3264; residual expand 3001 / pure 1354).
//
// Coverage:
//   - free == inline == pin positive shell-OR-equipped form
//   - poles: shell breaks all; non-shell equipped only
//   - residual 3001 / 1354 pins still hold
//   - dense rank × equipped free == inline == pin
//   - host-style inject + residual independence (3008)
auto runLinkshellBreakInventoryPearl3264SelfTests() -> bool
{
    using linkshellhelpers::ShouldBreakInventoryPearl;
    using linkshellhelpers::ShouldMarkPearlBroken;

    bool ok = true;

    // Residual 1354 / 3001 pins still hold under dedicated dual-wire.
    ok = expect(ShouldBreakInventoryPearl(LSTYPE_LINKSHELL, false), "residual: shell holder breaks all matching pearls") && ok;
    ok = expect(ShouldBreakInventoryPearl(LSTYPE_PEARLSACK, true), "residual: non-shell breaks equipped item") && ok;
    ok = expect(!ShouldBreakInventoryPearl(LSTYPE_PEARLSACK, false), "residual: non-shell skips unequipped inventory pearl") && ok;

    // --- Core poles: free == inline == pin positive form ---
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

        // Residual 3001 / 1354 re-pins.
        { LSTYPE_LINKSHELL, false, true, "residual 3001 shell + unequipped breaks" },
        { LSTYPE_PEARLSACK, true, true, "residual 3001 sack + equipped breaks" },
        { LSTYPE_PEARLSACK, false, false, "residual 3001 sack + unequipped skips" },
        { LSTYPE_LINKSHELL, false, true, "residual 1354 shell breaks all" },
        { LSTYPE_PEARLSACK, true, true, "residual 1354 equipped only" },
        { LSTYPE_PEARLSACK, false, false, "residual 1354 not equipped" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldBreakInventoryPearl(c.requesterRank, c.isEquippedItem);
        const bool inlineF = inlineShouldBreakInventoryPearl(c.requesterRank, c.isEquippedItem);
        const bool pinGot  = pinShouldBreakInventoryPearl(c.requesterRank, c.isEquippedItem);
        const bool wantPin = c.requesterRank == LSTYPE_LINKSHELL || c.isEquippedItem;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin") && ok;
        ok = expect(got == wantPin, "dual-wire free==positive pin form") && ok;
    }

    // Free == pin across residual poles.
    ok = expect(ShouldBreakInventoryPearl(LSTYPE_LINKSHELL, false) ==
                    pinShouldBreakInventoryPearl(LSTYPE_LINKSHELL, false),
                "free==pin shell + unequipped") &&
         ok;
    ok = expect(ShouldBreakInventoryPearl(LSTYPE_PEARLSACK, true) ==
                    pinShouldBreakInventoryPearl(LSTYPE_PEARLSACK, true),
                "free==pin sack + equipped") &&
         ok;
    ok = expect(ShouldBreakInventoryPearl(LSTYPE_PEARLSACK, false) ==
                    pinShouldBreakInventoryPearl(LSTYPE_PEARLSACK, false),
                "free==pin sack + unequipped") &&
         ok;

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
            const bool got     = ShouldBreakInventoryPearl(rank, isEquippedItem);
            const bool inlineF = inlineShouldBreakInventoryPearl(rank, isEquippedItem);
            const bool pinGot  = pinShouldBreakInventoryPearl(rank, isEquippedItem);
            const bool want    = rank == LSTYPE_LINKSHELL || isEquippedItem;
            ok                 = expect(got == want, "compose free==pin formula") && ok;
            ok                 = expect(got == inlineF, "compose free==inline") && ok;
            ok                 = expect(got == pinGot, "compose free==pin") && ok;
        }
    }

    // Host-style inject poles: CLinkshell::RemoveMemberByName injects
    // requesterRank and (newPItemLinkshell == PItemLinkshell).
    const struct
    {
        uint8       requesterRank;
        bool        isEquippedItem;
        bool        wantBreak;
        const char* label;
    } hostCases[] = {
        { LSTYPE_LINKSHELL, false, true, "shell holder inventory scan → break" },
        { LSTYPE_LINKSHELL, true, true, "shell holder equipped slot → break" },
        { LSTYPE_PEARLSACK, true, true, "sack kick equipped pearl → break" },
        { LSTYPE_PEARLSACK, false, false, "sack kick other inventory pearl → skip" },
        { LSTYPE_LINKPEARL, true, true, "pearl requester equipped → break" },
        { LSTYPE_LINKPEARL, false, false, "pearl requester unequipped → skip" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldBreakInventoryPearl(c.requesterRank, c.isEquippedItem);
        const bool inlineF = inlineShouldBreakInventoryPearl(c.requesterRank, c.isEquippedItem);
        const bool pinGot  = pinShouldBreakInventoryPearl(c.requesterRank, c.isEquippedItem);

        ok = expect(got == c.wantBreak, c.label) && ok;
        ok = expect(got == inlineF, "host compose free==inline") && ok;
        ok = expect(got == pinGot, "host compose free==pin") && ok;
    }

    // Production CLinkshell::RemoveMemberByName path semantics:
    // shell → break all; non-shell equipped → break; non-shell unequipped → skip.
    ok = expect(ShouldBreakInventoryPearl(LSTYPE_LINKSHELL, false) &&
                    pinShouldBreakInventoryPearl(LSTYPE_LINKSHELL, false),
                "RemoveMemberByName shell → break all path") &&
         ok;
    ok = expect(ShouldBreakInventoryPearl(LSTYPE_PEARLSACK, true) &&
                    pinShouldBreakInventoryPearl(LSTYPE_PEARLSACK, true),
                "RemoveMemberByName equipped → break path") &&
         ok;
    ok = expect(!ShouldBreakInventoryPearl(LSTYPE_PEARLSACK, false) &&
                    !pinShouldBreakInventoryPearl(LSTYPE_PEARLSACK, false),
                "RemoveMemberByName non-shell unequipped → skip break") &&
         ok;

    // Residual independence (1354 / 3001 / 3008):
    // inventory break gate is distinct from ShouldMarkPearlBroken type gate.
    // Sibling ShouldMarkPearlBroken (3008) is left alone; only independence pins.
    ok = expect(ShouldBreakInventoryPearl(LSTYPE_LINKSHELL, false),
                "shell must select break via free gate") &&
         ok;
    ok = expect(!ShouldMarkPearlBroken(LSTYPE_LINKSHELL),
                "shell type still kept intact under residual (3008)") &&
         ok;
    ok = expect(ShouldBreakInventoryPearl(LSTYPE_PEARLSACK, true),
                "sack equipped must select break via free gate") &&
         ok;
    ok = expect(ShouldMarkPearlBroken(LSTYPE_PEARLSACK),
                "sack type still marks broken under residual (3008)") &&
         ok;
    ok = expect(!ShouldBreakInventoryPearl(LSTYPE_PEARLSACK, false),
                "sack unequipped must skip break even if type would mark") &&
         ok;

    return ok;
}
