#include "test_linkshell_send_message_ipc_3403.h"

#include "map/linkshell_capacity.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "linkshell ShouldSendLinkshellMessageIPC 3403 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CLinkshell::setMessage IPC gate for dual-wire cross-check
// (slice 3403):
//   messageNonEmpty
auto inlineShouldSendLinkshellMessageIPC3403(const bool messageNonEmpty) -> bool
{
    return messageNonEmpty;
}

// Direct-return dual-wire pin matching free / capacity body (slice 3403).
// Identity form only — free == inline == pin == messageNonEmpty.
// Formula unchanged from residual 2977 / pure 1354 / 2171.
auto pinShouldSendLinkshellMessageIPC3403(const bool messageNonEmpty) -> bool
{
    return messageNonEmpty;
}

// Residual dual-wire 2977 pin (retained suite form) for residual re-pins.
auto pinShouldSendLinkshellMessageIPC2977(const bool messageNonEmpty) -> bool
{
    return messageNonEmpty;
}

} // namespace

// Pure dual-wire expansion for linkshellhelpers::ShouldSendLinkshellMessageIPC
// (messageNonEmpty identity; dedicated slice 3403; residual expand 2977 /
// pure 1354 / 2171).
//
// Coverage:
//   - free == inline == pin (direct return identity)
//   - residual 2977 / 1354 / 2171 pins still hold
//   - dense 2¹ free == inline == pin
//   - host inject CLinkshell::setMessage path semantics
auto runLinkshellSendMessageIPC3403SelfTests() -> bool
{
    using linkshellhelpers::ShouldPushStoredLinkshellMessage;
    using linkshellhelpers::ShouldSendLinkshellMessageIPC;

    bool ok = true;

    // Residual 1354 / 2171 / prior residual dual-wire 2977 pins still hold.
    ok = expect(ShouldSendLinkshellMessageIPC(true), "residual 1354/2171: non-empty sends IPC") && ok;
    ok = expect(!ShouldSendLinkshellMessageIPC(false), "residual 1354/2171: empty skips IPC") && ok;
    ok = expect(ShouldSendLinkshellMessageIPC(true), "prior residual 2977: non-empty sends IPC") && ok;
    ok = expect(!ShouldSendLinkshellMessageIPC(false), "prior residual 2977: empty skips IPC") && ok;
    ok = expect(pinShouldSendLinkshellMessageIPC2977(true), "prior residual 2977 pin non-empty") && ok;
    ok = expect(!pinShouldSendLinkshellMessageIPC2977(false), "prior residual 2977 pin empty") && ok;

    // Core poles: free == inline == pin (direct return identity).
    const struct
    {
        bool        messageNonEmpty;
        bool        want;
        const char* label;
    } cases[] = {
        // Dense 2¹ dual poles.
        { false, false, "empty skips IPC" },
        { true, true, "non-empty sends IPC" },

        // Residual 2977 / 1354 / 2171 re-pins.
        { true, true, "residual 2977 non-empty sends IPC" },
        { false, false, "residual 2977 empty skips IPC" },
        { true, true, "residual 1354 non-empty sends IPC" },
        { false, false, "residual 1354 empty skips IPC" },
        { true, true, "residual 2171 non-empty sends IPC" },
        { false, false, "residual 2171 empty skips IPC" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldSendLinkshellMessageIPC(c.messageNonEmpty);
        const bool inlineF = inlineShouldSendLinkshellMessageIPC3403(c.messageNonEmpty);
        const bool pin     = pinShouldSendLinkshellMessageIPC3403(c.messageNonEmpty);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldSendLinkshellMessageIPC dual-wire == inline LSB formula") && ok;
        ok = expect(got == pin, "ShouldSendLinkshellMessageIPC == direct return pin") && ok;
    }

    // Free == pin across residual poles (direct return pin form).
    ok = expect(ShouldSendLinkshellMessageIPC(true) == pinShouldSendLinkshellMessageIPC3403(true),
                "free == pin residual non-empty send") &&
         ok;
    ok = expect(ShouldSendLinkshellMessageIPC(false) == pinShouldSendLinkshellMessageIPC3403(false),
                "free == pin residual empty skip") &&
         ok;

    // Dense compose: full 2¹ boolean space free == inline == pin.
    for (const bool messageNonEmpty : { false, true })
    {
        const bool got     = ShouldSendLinkshellMessageIPC(messageNonEmpty);
        const bool inlineF = inlineShouldSendLinkshellMessageIPC3403(messageNonEmpty);
        const bool pin     = pinShouldSendLinkshellMessageIPC3403(messageNonEmpty);
        ok                 = expect(got == pin, "compose free == direct return pin") && ok;
        ok                 = expect(got == inlineF, "compose free == inline") && ok;
        ok                 = expect(got == messageNonEmpty, "compose free == messageNonEmpty identity") && ok;
    }

    // --- Production CLinkshell::setMessage path semantics ---
    // Host injects:
    //   messageNonEmpty = (message.size() != 0)
    // when true  → message::send(ipc::LinkshellSetMessage{...})
    // when false → skip IPC (DB poster/message/messagetime already updated)
    ok = expect(ShouldSendLinkshellMessageIPC(true), "setMessage non-empty → IPC send path") && ok;
    ok = expect(!ShouldSendLinkshellMessageIPC(false), "setMessage empty → skip IPC") && ok;

    // Host-style string inject poles (host owns size()!=0).
    const struct
    {
        std::string message;
        const char* label;
    } hostPoles[] = {
        { "hello LS", "non-empty string → send" },
        { "", "empty string → skip" },
        { " ", "whitespace-only still non-empty → send" },
        { "x", "single char → send" },
    };
    for (const auto& p : hostPoles)
    {
        const bool messageNonEmpty = p.message.size() != 0;
        const bool got             = ShouldSendLinkshellMessageIPC(messageNonEmpty);
        const bool inlineF         = inlineShouldSendLinkshellMessageIPC3403(messageNonEmpty);
        const bool pin             = pinShouldSendLinkshellMessageIPC3403(messageNonEmpty);
        ok                         = expect(got == messageNonEmpty, p.label) && ok;
        ok                         = expect(got == inlineF && got == pin, "host string free == inline == pin") && ok;
    }

    // Explicit dual-wire: free function is identity of host inject.
    for (const bool messageNonEmpty : { false, true })
    {
        ok = expect(ShouldSendLinkshellMessageIPC(messageNonEmpty) == messageNonEmpty,
                    "host inject identity") &&
             ok;
        ok = expect(ShouldSendLinkshellMessageIPC(messageNonEmpty) ==
                        inlineShouldSendLinkshellMessageIPC3403(messageNonEmpty),
                    "host inject dual-wire free == inline") &&
             ok;
        ok = expect(ShouldSendLinkshellMessageIPC(messageNonEmpty) ==
                        pinShouldSendLinkshellMessageIPC3403(messageNonEmpty),
                    "host inject dual-wire free == pin") &&
             ok;
    }

    // Sibling dual-wire independence (2993 leave alone).
    ok = expect(ShouldPushStoredLinkshellMessage(true), "sibling residual: push non-empty") && ok;
    ok = expect(!ShouldPushStoredLinkshellMessage(false), "sibling residual: skip empty push") && ok;
    ok = expect(ShouldSendLinkshellMessageIPC(true),
                "non-empty must send IPC even if push sibling also admits") &&
         ok;
    ok = expect(!ShouldSendLinkshellMessageIPC(false),
                "empty must skip IPC even if push sibling also skips") &&
         ok;

    return ok;
}
