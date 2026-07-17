#include "test_linkshell_mark_pearl_broken_3008.h"

#include "map/linkshell_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "linkshell ShouldMarkPearlBroken 3008 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CLinkshell::RemoveMemberByName pearl-type break gate for dual-wire
// cross-check (slice 3008):
//   lsType != LSTYPE_LINKSHELL
auto inlineShouldMarkPearlBroken(const uint8 lsType) -> bool
{
    return lsType != LSTYPE_LINKSHELL;
}

} // namespace

// Pure dual-wire expansion for linkshellhelpers::ShouldMarkPearlBroken
// (RemoveMemberByName pearl-type break gate; slice 3008).
auto runLinkshellMarkPearlBroken3008SelfTests() -> bool
{
    using linkshellhelpers::ShouldBreakInventoryPearl;
    using linkshellhelpers::ShouldMarkPearlBroken;

    bool ok = true;

    // Residual 1354 pins still hold under dual-wire.
    ok = expect(ShouldMarkPearlBroken(LSTYPE_PEARLSACK), "residual mark sack") && ok;
    ok = expect(!ShouldMarkPearlBroken(LSTYPE_LINKSHELL), "residual keep shell") && ok;

    const struct
    {
        uint8       lsType;
        bool        want;
        const char* label;
    } cases[] = {
        // Shell-holder type is never marked broken.
        { LSTYPE_LINKSHELL, false, "shell kept intact" },

        // Pearls / sacks are marked broken on remove.
        { LSTYPE_PEARLSACK, true, "sack marked broken" },
        { LSTYPE_LINKPEARL, true, "pearl marked broken" },

        // Broken / new ranks are not shell holders → mark.
        { LSTYPE_BROKEN, true, "already-broken still != shell" },
        { LSTYPE_NEW_LINKSHELL, true, "new type marked broken" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldMarkPearlBroken(c.lsType);
        const bool inlineF = inlineShouldMarkPearlBroken(c.lsType);
        const bool wantPin = c.lsType != LSTYPE_LINKSHELL;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldMarkPearlBroken dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldMarkPearlBroken == pin formula") && ok;
    }

    // Pin composition: non-shell only.
    ok = expect(!ShouldMarkPearlBroken(LSTYPE_LINKSHELL), "shell holder type must never mark broken") && ok;
    ok = expect(ShouldMarkPearlBroken(LSTYPE_PEARLSACK), "sack must mark broken") && ok;
    ok = expect(ShouldMarkPearlBroken(LSTYPE_LINKPEARL), "pearl must mark broken") && ok;

    // Dense compose: all rank poles free == inline == pin.
    const uint8 ranks[] = {
        static_cast<uint8>(LSTYPE_NEW_LINKSHELL),
        static_cast<uint8>(LSTYPE_LINKSHELL),
        static_cast<uint8>(LSTYPE_PEARLSACK),
        static_cast<uint8>(LSTYPE_LINKPEARL),
        static_cast<uint8>(LSTYPE_BROKEN),
    };
    for (const uint8 rank : ranks)
    {
        const bool got  = ShouldMarkPearlBroken(rank);
        const bool want = rank != LSTYPE_LINKSHELL;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldMarkPearlBroken(rank), "compose free == inline") && ok;
    }

    // --- Production CLinkshell::RemoveMemberByName path semantics ---
    // Host injects:
    //   lsType = static_cast<uint8>(newPItemLinkshell->GetLSType())
    // when true  → SetLSType(LSTYPE_BROKEN) + char_inventory UPDATE + ITEM_ATTR
    // when false → leave shell item type intact
    // Evaluated only after ShouldBreakInventoryPearl passes (slice 3001).
    ok = expect(!ShouldMarkPearlBroken(LSTYPE_LINKSHELL), "RemoveMemberByName shell item → keep type path") && ok;
    ok = expect(ShouldMarkPearlBroken(LSTYPE_PEARLSACK), "RemoveMemberByName sack item → mark broken path") && ok;
    ok = expect(ShouldMarkPearlBroken(LSTYPE_LINKPEARL), "RemoveMemberByName pearl item → mark broken path") && ok;

    // Host-style inject poles (host owns GetLSType).
    const struct
    {
        uint8       lsType;
        const char* label;
    } hostPoles[] = {
        { LSTYPE_LINKSHELL, "shell holder inventory item → keep" },
        { LSTYPE_PEARLSACK, "sack inventory item → mark broken" },
        { LSTYPE_LINKPEARL, "pearl inventory item → mark broken" },
        { LSTYPE_BROKEN, "already-broken inventory item → still != shell" },
        { LSTYPE_NEW_LINKSHELL, "new-type inventory item → mark broken" },
    };
    for (const auto& p : hostPoles)
    {
        const bool got     = ShouldMarkPearlBroken(p.lsType);
        const bool inlineF = inlineShouldMarkPearlBroken(p.lsType);
        const bool want    = p.lsType != LSTYPE_LINKSHELL;
        ok                 = expect(got == want, p.label) && ok;
        ok                 = expect(got == inlineF, "host inject dual-wire free == inline") && ok;
    }

    // Explicit dual-wire: free function is != shell of host inject.
    for (const uint8 rank : ranks)
    {
        const bool want = rank != LSTYPE_LINKSHELL;
        ok              = expect(ShouldMarkPearlBroken(rank) == want, "host inject formula") && ok;
        ok              = expect(ShouldMarkPearlBroken(rank) == inlineShouldMarkPearlBroken(rank),
                    "host inject dual-wire free == inline") &&
             ok;
    }

    // Compose with upstream ShouldBreakInventoryPearl (3001): shell holder
    // may select a pearl for break, but shell type itself is never marked.
    ok = expect(ShouldBreakInventoryPearl(LSTYPE_LINKSHELL, false),
                "upstream 3001: shell selects all matching pearls for break") &&
         ok;
    ok = expect(!ShouldMarkPearlBroken(LSTYPE_LINKSHELL),
                "3008: shell type still not marked after break gate") &&
         ok;
    ok = expect(ShouldBreakInventoryPearl(LSTYPE_PEARLSACK, true),
                "upstream 3001: non-shell equipped selects for break") &&
         ok;
    ok = expect(ShouldMarkPearlBroken(LSTYPE_PEARLSACK),
                "3008: sack type marked after break gate") &&
         ok;

    return ok;
}
