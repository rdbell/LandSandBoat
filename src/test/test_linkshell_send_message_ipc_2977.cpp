#include "test_linkshell_send_message_ipc_2977.h"

#include "map/linkshell_capacity.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "linkshell ShouldSendLinkshellMessageIPC 2977 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CLinkshell::setMessage IPC gate for dual-wire cross-check
// (slice 2977):
//   messageNonEmpty
auto inlineShouldSendLinkshellMessageIPC(const bool messageNonEmpty) -> bool
{
    return messageNonEmpty;
}

} // namespace

// Pure dual-wire expansion for linkshellhelpers::ShouldSendLinkshellMessageIPC
// (setMessage post-DB IPC identity gate; slice 2977).
auto runLinkshellSendMessageIPC2977SelfTests() -> bool
{
    using linkshellhelpers::ShouldSendLinkshellMessageIPC;

    bool ok = true;

    // Residual 1354 / 2171 pins still hold under dual-wire.
    ok = expect(ShouldSendLinkshellMessageIPC(true), "residual non-empty sends IPC") && ok;
    ok = expect(!ShouldSendLinkshellMessageIPC(false), "residual empty skips IPC") && ok;

    const struct
    {
        bool        messageNonEmpty;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic accept pole — non-empty message.
        { true, true, "non-empty sends IPC" },

        // Residual empty pole.
        { false, false, "empty skips IPC" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldSendLinkshellMessageIPC(c.messageNonEmpty);
        const bool inlineF = inlineShouldSendLinkshellMessageIPC(c.messageNonEmpty);
        const bool wantPin = c.messageNonEmpty;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldSendLinkshellMessageIPC dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldSendLinkshellMessageIPC == pin formula") && ok;
    }

    // Pin composition: identity of messageNonEmpty.
    ok = expect(ShouldSendLinkshellMessageIPC(true), "messageNonEmpty true must send") && ok;
    ok = expect(!ShouldSendLinkshellMessageIPC(false), "messageNonEmpty false must skip") && ok;

    // Dense compose: full boolean space free == inline == pin.
    for (const bool messageNonEmpty : { false, true })
    {
        const bool got  = ShouldSendLinkshellMessageIPC(messageNonEmpty);
        const bool want = messageNonEmpty;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldSendLinkshellMessageIPC(messageNonEmpty),
                    "compose free == inline") &&
             ok;
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
        const bool inlineF         = inlineShouldSendLinkshellMessageIPC(messageNonEmpty);
        ok                         = expect(got == messageNonEmpty, p.label) && ok;
        ok                         = expect(got == inlineF, "host string dual-wire free == inline") && ok;
    }

    // Explicit dual-wire: free function is identity of host inject.
    for (const bool messageNonEmpty : { false, true })
    {
        ok = expect(ShouldSendLinkshellMessageIPC(messageNonEmpty) == messageNonEmpty,
                    "host inject identity") &&
             ok;
        ok = expect(ShouldSendLinkshellMessageIPC(messageNonEmpty) ==
                        inlineShouldSendLinkshellMessageIPC(messageNonEmpty),
                    "host inject dual-wire free == inline") &&
             ok;
    }

    return ok;
}
