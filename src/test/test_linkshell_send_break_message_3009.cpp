#include "test_linkshell_send_break_message_3009.h"

#include "map/linkshell_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "linkshell ShouldSendBreakMessage 3009 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CLinkshell::RemoveMemberByName break vs kick message gate for
// dual-wire cross-check (slice 3009):
//   breakLinkshell
auto inlineShouldSendBreakMessage(const bool breakLinkshell) -> bool
{
    return breakLinkshell;
}

} // namespace

// Pure dual-wire expansion for linkshellhelpers::ShouldSendBreakMessage
// (RemoveMemberByName break vs kick message identity gate; slice 3009).
auto runLinkshellSendBreakMessage3009SelfTests() -> bool
{
    using linkshellhelpers::ShouldMarkPearlBroken;
    using linkshellhelpers::ShouldSendBreakMessage;

    bool ok = true;

    // Residual 1354 pins still hold under dual-wire.
    ok = expect(ShouldSendBreakMessage(true), "residual break → NoLongerExists") && ok;
    ok = expect(!ShouldSendBreakMessage(false), "residual kick → Kicked") && ok;

    const struct
    {
        bool        breakLinkshell;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic break pole — shell broken; NoLongerExists.
        { true, true, "break → NoLongerExists" },

        // Residual kick pole — member removed without shell break.
        { false, false, "kick → Kicked" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldSendBreakMessage(c.breakLinkshell);
        const bool inlineF = inlineShouldSendBreakMessage(c.breakLinkshell);
        const bool wantPin = c.breakLinkshell;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldSendBreakMessage dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldSendBreakMessage == pin formula") && ok;
    }

    // Pin composition: identity of breakLinkshell.
    ok = expect(ShouldSendBreakMessage(true), "breakLinkshell true must select NoLongerExists") && ok;
    ok = expect(!ShouldSendBreakMessage(false), "breakLinkshell false must select Kicked") && ok;

    // Dense compose: full boolean space free == inline == pin.
    for (const bool breakLinkshell : { false, true })
    {
        const bool got  = ShouldSendBreakMessage(breakLinkshell);
        const bool want = breakLinkshell;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldSendBreakMessage(breakLinkshell),
                    "compose free == inline") &&
             ok;
    }

    // --- Production CLinkshell::RemoveMemberByName path semantics ---
    // Host injects:
    //   breakLinkshell = RemoveMemberByName(..., breakLinkshell)
    // when true  → pushPacket MESSAGE(MsgStd::LinkshellNoLongerExists)
    // when false → pushPacket MESSAGE(MsgStd::LinkshellKicked)
    // Evaluated after inventory break / mark-broken / ITEM_SAME / CharStatus.
    ok = expect(ShouldSendBreakMessage(true), "RemoveMemberByName break → NoLongerExists path") && ok;
    ok = expect(!ShouldSendBreakMessage(false), "RemoveMemberByName kick → Kicked path") && ok;

    // Host-style inject poles (host owns breakLinkshell flag).
    const struct
    {
        bool        breakLinkshell;
        const char* label;
    } hostPoles[] = {
        { true, "break flag → NoLongerExists" },
        { false, "kick flag → Kicked" },
    };
    for (const auto& p : hostPoles)
    {
        const bool got     = ShouldSendBreakMessage(p.breakLinkshell);
        const bool inlineF = inlineShouldSendBreakMessage(p.breakLinkshell);
        const bool want    = p.breakLinkshell;
        ok                 = expect(got == want, p.label) && ok;
        ok                 = expect(got == inlineF, "host inject dual-wire free == inline") && ok;
    }

    // Explicit dual-wire: free function is identity of host inject.
    for (const bool breakLinkshell : { false, true })
    {
        ok = expect(ShouldSendBreakMessage(breakLinkshell) == breakLinkshell,
                    "host inject identity") &&
             ok;
        ok = expect(ShouldSendBreakMessage(breakLinkshell) ==
                        inlineShouldSendBreakMessage(breakLinkshell),
                    "host inject dual-wire free == inline") &&
             ok;
    }

    // Compose with upstream ShouldMarkPearlBroken (3008): mark-broken is an
    // inventory-type gate; message choice is independent (identity of flag).
    ok = expect(!ShouldMarkPearlBroken(LSTYPE_LINKSHELL),
                "upstream 3008: shell type still not marked") &&
         ok;
    ok = expect(ShouldSendBreakMessage(true),
                "3009: break flag still selects NoLongerExists after mark gate") &&
         ok;
    ok = expect(ShouldMarkPearlBroken(LSTYPE_PEARLSACK),
                "upstream 3008: sack type marked broken") &&
         ok;
    ok = expect(!ShouldSendBreakMessage(false),
                "3009: kick flag still selects Kicked after mark gate") &&
         ok;

    return ok;
}
