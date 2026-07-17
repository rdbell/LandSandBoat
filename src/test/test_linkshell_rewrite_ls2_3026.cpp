#include "test_linkshell_rewrite_ls2_3026.h"

#include "map/linkshell_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "linkshell ShouldRewritePacketAsLinkshell2 3026 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CLinkshell::PushPacket LS2 rewrite gate for dual-wire cross-check
// (slice 3026):
//   memberIsLS2
auto inlineShouldRewritePacketAsLinkshell2(const bool memberIsLS2) -> bool
{
    return memberIsLS2;
}

} // namespace

// Pure dual-wire expansion for linkshellhelpers::ShouldRewritePacketAsLinkshell2
// (PushPacket optional LS2 rewrite identity gate; slice 3026).
auto runLinkshellRewriteLS23026SelfTests() -> bool
{
    using linkshellhelpers::ApplyLinkshellMessageLS2Flag;
    using linkshellhelpers::ChatStdMessageTypeForLS2;
    using linkshellhelpers::LinkshellMessageLS2Flag;
    using linkshellhelpers::MessageLinkshell2;
    using linkshellhelpers::ShouldReceiveLinkshellPacket;
    using linkshellhelpers::ShouldRewritePacketAsLinkshell2;

    bool ok = true;

    // Residual 1354 pins still hold under dual-wire.
    ok = expect(ShouldRewritePacketAsLinkshell2(true), "residual LS2 member → rewrite") && ok;
    ok = expect(!ShouldRewritePacketAsLinkshell2(false), "residual LS1 member → no rewrite") && ok;

    const struct
    {
        bool        memberIsLS2;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic LS2 pole — member attached as PLinkshell2.
        { true, true, "LS2 member → rewrite" },

        // Residual LS1 pole — member attached only as PLinkshell1 / other.
        { false, false, "LS1 member → no rewrite" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRewritePacketAsLinkshell2(c.memberIsLS2);
        const bool inlineF = inlineShouldRewritePacketAsLinkshell2(c.memberIsLS2);
        const bool wantPin = c.memberIsLS2;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRewritePacketAsLinkshell2 dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldRewritePacketAsLinkshell2 == pin formula") && ok;
    }

    // Pin composition: identity of memberIsLS2.
    ok = expect(ShouldRewritePacketAsLinkshell2(true), "memberIsLS2 true must rewrite") && ok;
    ok = expect(!ShouldRewritePacketAsLinkshell2(false), "memberIsLS2 false must not rewrite") && ok;

    // Dense compose: full boolean space free == inline == pin.
    for (const bool memberIsLS2 : { false, true })
    {
        const bool got  = ShouldRewritePacketAsLinkshell2(memberIsLS2);
        const bool want = memberIsLS2;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldRewritePacketAsLinkshell2(memberIsLS2),
                    "compose free == inline") &&
             ok;
    }

    // --- Production CLinkshell::PushPacket path semantics ---
    // Host injects (after ShouldReceiveLinkshellPacket):
    //   memberIsLS2 = member->PLinkshell2 == this
    // when true  → rewrite chat_std type (MESSAGE_LINKSHELL2) or OR 0x40 on
    //              LS message byte 0x05, then pushPacket
    // when false → pushPacket without rewrite
    ok = expect(ShouldRewritePacketAsLinkshell2(true), "PushPacket LS2 → rewrite path") && ok;
    ok = expect(!ShouldRewritePacketAsLinkshell2(false), "PushPacket LS1 → no rewrite path") && ok;

    // Host-style inject poles (host owns PLinkshell2 pointer compare).
    const struct
    {
        bool        memberIsLS2;
        const char* label;
    } hostPoles[] = {
        { true, "PLinkshell2 == this → rewrite" },
        { false, "PLinkshell2 != this → no rewrite" },
    };
    for (const auto& p : hostPoles)
    {
        const bool got     = ShouldRewritePacketAsLinkshell2(p.memberIsLS2);
        const bool inlineF = inlineShouldRewritePacketAsLinkshell2(p.memberIsLS2);
        const bool want    = p.memberIsLS2;
        ok                 = expect(got == want, p.label) && ok;
        ok                 = expect(got == inlineF, "host inject dual-wire free == inline") && ok;
    }

    // Explicit dual-wire: free function is identity of host inject.
    for (const bool memberIsLS2 : { false, true })
    {
        ok = expect(ShouldRewritePacketAsLinkshell2(memberIsLS2) == memberIsLS2,
                    "host inject identity") &&
             ok;
        ok = expect(ShouldRewritePacketAsLinkshell2(memberIsLS2) ==
                        inlineShouldRewritePacketAsLinkshell2(memberIsLS2),
                    "host inject dual-wire free == inline") &&
             ok;
    }

    // Rewrite side effects remain residual capacity (not this dual-wire):
    // chat_std type pin and LS message flag OR are independent helpers.
    ok = expect(ChatStdMessageTypeForLS2() == MessageLinkshell2,
                "residual ChatStdMessageTypeForLS2 still MESSAGE_LINKSHELL2 (27)") &&
         ok;
    ok = expect(ApplyLinkshellMessageLS2Flag(0x01) == static_cast<uint8>(0x01 | LinkshellMessageLS2Flag),
                "residual ApplyLinkshellMessageLS2Flag still ORs 0x40") &&
         ok;

    // Compose with sibling ShouldReceiveLinkshellPacket (3017): receive filter
    // runs first; LS2 rewrite is independent (identity of slot inject).
    ok = expect(ShouldReceiveLinkshellPacket(false, false, false),
                "sibling 3017: all-clear still receives") &&
         ok;
    ok = expect(ShouldRewritePacketAsLinkshell2(true),
                "3026: LS2 still rewrites after receive gate compose") &&
         ok;
    ok = expect(!ShouldReceiveLinkshellPacket(true, false, false),
                "sibling 3017: sender still filtered") &&
         ok;
    ok = expect(!ShouldRewritePacketAsLinkshell2(false),
                "3026: LS1 still skips rewrite after receive gate compose") &&
         ok;

    return ok;
}
