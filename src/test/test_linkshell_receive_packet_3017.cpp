#include "test_linkshell_receive_packet_3017.h"

#include "map/linkshell_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "linkshell ShouldReceiveLinkshellPacket 3017 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CLinkshell::PushPacket per-member receive gate for
// dual-wire cross-check (slice 3017):
//   !isSender && !isDisappear && !inPrison
auto inlineShouldReceiveLinkshellPacket(const bool isSender, const bool isDisappear, const bool inPrison) -> bool
{
    return !isSender && !isDisappear && !inPrison;
}

} // namespace

// Pure dual-wire expansion for linkshellhelpers::ShouldReceiveLinkshellPacket
// (PushPacket per-member receive gate; slice 3017).
auto runLinkshellReceivePacket3017SelfTests() -> bool
{
    using linkshellhelpers::ShouldReceiveLinkshellPacket;
    using linkshellhelpers::ShouldSendBreakMessage;

    bool ok = true;

    // Residual 1354 pins still hold under dual-wire.
    ok = expect(ShouldReceiveLinkshellPacket(false, false, false), "residual non-sender visible free → receive") && ok;
    ok = expect(!ShouldReceiveLinkshellPacket(true, false, false), "residual sender filtered") && ok;
    ok = expect(!ShouldReceiveLinkshellPacket(false, true, false), "residual disappear filtered") && ok;
    ok = expect(!ShouldReceiveLinkshellPacket(false, false, true), "residual prison filtered") && ok;

    const struct
    {
        bool        isSender;
        bool        isDisappear;
        bool        inPrison;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic accept pole — non-sender, visible, free.
        { false, false, false, true, "non-sender visible free → receive" },

        // Single-filter reject poles.
        { true, false, false, false, "sender filtered" },
        { false, true, false, false, "disappear filtered" },
        { false, false, true, false, "prison filtered" },

        // Multi-filter reject poles (any filter is enough to skip).
        { true, true, false, false, "sender + disappear filtered" },
        { true, false, true, false, "sender + prison filtered" },
        { false, true, true, false, "disappear + prison filtered" },
        { true, true, true, false, "all filters → skip" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldReceiveLinkshellPacket(c.isSender, c.isDisappear, c.inPrison);
        const bool inlineF = inlineShouldReceiveLinkshellPacket(c.isSender, c.isDisappear, c.inPrison);
        const bool wantPin = !c.isSender && !c.isDisappear && !c.inPrison;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldReceiveLinkshellPacket dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldReceiveLinkshellPacket == pin formula") && ok;
    }

    // Pin composition: all three filters must pass.
    ok = expect(ShouldReceiveLinkshellPacket(false, false, false), "all-clear must receive") && ok;
    ok = expect(!ShouldReceiveLinkshellPacket(true, false, false), "sender must skip") && ok;
    ok = expect(!ShouldReceiveLinkshellPacket(false, true, false), "disappear must skip") && ok;
    ok = expect(!ShouldReceiveLinkshellPacket(false, false, true), "prison must skip") && ok;

    // Dense compose: full 2^3 boolean space free == inline == pin.
    for (const bool isSender : { false, true })
    {
        for (const bool isDisappear : { false, true })
        {
            for (const bool inPrison : { false, true })
            {
                const bool got  = ShouldReceiveLinkshellPacket(isSender, isDisappear, inPrison);
                const bool want = !isSender && !isDisappear && !inPrison;
                ok              = expect(got == want, "compose free == pin formula") && ok;
                ok              = expect(got == inlineShouldReceiveLinkshellPacket(isSender, isDisappear, inPrison),
                            "compose free == inline") &&
                     ok;
            }
        }
    }

    // --- Production CLinkshell::PushPacket path semantics ---
    // Host injects:
    //   isSender    = member->id == senderID
    //   isDisappear = member->status == STATUS_TYPE::DISAPPEAR
    //   inPrison    = jailutils::InPrison(member)
    // when true  → copy packet, optional LS2 rewrite, pushPacket
    // when false → skip this online member
    ok = expect(ShouldReceiveLinkshellPacket(false, false, false), "PushPacket non-sender visible free → receive path") && ok;
    ok = expect(!ShouldReceiveLinkshellPacket(true, false, false), "PushPacket sender → skip path") && ok;
    ok = expect(!ShouldReceiveLinkshellPacket(false, true, false), "PushPacket disappear → skip path") && ok;
    ok = expect(!ShouldReceiveLinkshellPacket(false, false, true), "PushPacket prison → skip path") && ok;

    // Host-style inject poles (host owns id/status/jail evaluation).
    const struct
    {
        bool        isSender;
        bool        isDisappear;
        bool        inPrison;
        const char* label;
    } hostPoles[] = {
        { false, false, false, "member free → receive" },
        { true, false, false, "member is sender → skip" },
        { false, true, false, "member disappear → skip" },
        { false, false, true, "member in prison → skip" },
    };
    for (const auto& p : hostPoles)
    {
        const bool got     = ShouldReceiveLinkshellPacket(p.isSender, p.isDisappear, p.inPrison);
        const bool inlineF = inlineShouldReceiveLinkshellPacket(p.isSender, p.isDisappear, p.inPrison);
        const bool want    = !p.isSender && !p.isDisappear && !p.inPrison;
        ok                 = expect(got == want, p.label) && ok;
        ok                 = expect(got == inlineF, "host inject dual-wire free == inline") && ok;
    }

    // Explicit dual-wire: free function is AND of three inverted host injects.
    for (const bool isSender : { false, true })
    {
        for (const bool isDisappear : { false, true })
        {
            for (const bool inPrison : { false, true })
            {
                const bool want = !isSender && !isDisappear && !inPrison;
                ok              = expect(ShouldReceiveLinkshellPacket(isSender, isDisappear, inPrison) == want,
                            "host inject AND") &&
                     ok;
                ok = expect(ShouldReceiveLinkshellPacket(isSender, isDisappear, inPrison) ==
                                inlineShouldReceiveLinkshellPacket(isSender, isDisappear, inPrison),
                            "host inject dual-wire free == inline") &&
                     ok;
            }
        }
    }

    // Compose with sibling ShouldSendBreakMessage (3009): break-message is a
    // RemoveMemberByName gate; receive filter is independent (PushPacket path).
    ok = expect(ShouldSendBreakMessage(true),
                "sibling 3009: break flag still selects NoLongerExists") &&
         ok;
    ok = expect(ShouldReceiveLinkshellPacket(false, false, false),
                "3017: all-clear still receives after break-message gate compose") &&
         ok;
    ok = expect(!ShouldSendBreakMessage(false),
                "sibling 3009: kick flag still selects Kicked") &&
         ok;
    ok = expect(!ShouldReceiveLinkshellPacket(true, false, false),
                "3017: sender still filtered after break-message gate compose") &&
         ok;

    return ok;
}
