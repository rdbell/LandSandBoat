#include "test_linkshell_push_stored_message_2993.h"

#include "map/linkshell_capacity.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "linkshell ShouldPushStoredLinkshellMessage 2993 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CLinkshell::PushLinkshellMessage gate for dual-wire cross-check
// (slice 2993):
//   messageNonEmpty
auto inlineShouldPushStoredLinkshellMessage(const bool messageNonEmpty) -> bool
{
    return messageNonEmpty;
}

} // namespace

// Pure dual-wire expansion for linkshellhelpers::ShouldPushStoredLinkshellMessage
// (PushLinkshellMessage post-DB-load identity gate; slice 2993).
auto runLinkshellPushStoredMessage2993SelfTests() -> bool
{
    using linkshellhelpers::ShouldPushStoredLinkshellMessage;

    bool ok = true;

    // Residual 1354 pins still hold under dual-wire.
    ok = expect(ShouldPushStoredLinkshellMessage(true), "residual non-empty pushes stored LS message") && ok;
    ok = expect(!ShouldPushStoredLinkshellMessage(false), "residual empty skips push") && ok;

    const struct
    {
        bool        messageNonEmpty;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic accept pole — non-empty stored message.
        { true, true, "non-empty pushes stored message" },

        // Residual empty pole.
        { false, false, "empty skips push" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldPushStoredLinkshellMessage(c.messageNonEmpty);
        const bool inlineF = inlineShouldPushStoredLinkshellMessage(c.messageNonEmpty);
        const bool wantPin = c.messageNonEmpty;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldPushStoredLinkshellMessage dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldPushStoredLinkshellMessage == pin formula") && ok;
    }

    // Pin composition: identity of messageNonEmpty.
    ok = expect(ShouldPushStoredLinkshellMessage(true), "messageNonEmpty true must push") && ok;
    ok = expect(!ShouldPushStoredLinkshellMessage(false), "messageNonEmpty false must skip") && ok;

    // Dense compose: full boolean space free == inline == pin.
    for (const bool messageNonEmpty : { false, true })
    {
        const bool got  = ShouldPushStoredLinkshellMessage(messageNonEmpty);
        const bool want = messageNonEmpty;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldPushStoredLinkshellMessage(messageNonEmpty),
                    "compose free == inline") &&
             ok;
    }

    // --- Production CLinkshell::PushLinkshellMessage path semantics ---
    // Host injects:
    //   messageNonEmpty = !message.empty()
    // when true  → pushPacket GP_SERV_COMMAND_LINKSHELL_MESSAGE
    // when false → skip push (empty stored motd)
    ok = expect(ShouldPushStoredLinkshellMessage(true), "PushLinkshellMessage non-empty → push path") && ok;
    ok = expect(!ShouldPushStoredLinkshellMessage(false), "PushLinkshellMessage empty → skip push") && ok;

    // Host-style string inject poles (host owns !empty()).
    const struct
    {
        std::string message;
        const char* label;
    } hostPoles[] = {
        { "hello LS", "non-empty string → push" },
        { "", "empty string → skip" },
        { " ", "whitespace-only still non-empty → push" },
        { "x", "single char → push" },
    };
    for (const auto& p : hostPoles)
    {
        const bool messageNonEmpty = !p.message.empty();
        const bool got             = ShouldPushStoredLinkshellMessage(messageNonEmpty);
        const bool inlineF         = inlineShouldPushStoredLinkshellMessage(messageNonEmpty);
        ok                         = expect(got == messageNonEmpty, p.label) && ok;
        ok                         = expect(got == inlineF, "host string dual-wire free == inline") && ok;
    }

    // Explicit dual-wire: free function is identity of host inject.
    for (const bool messageNonEmpty : { false, true })
    {
        ok = expect(ShouldPushStoredLinkshellMessage(messageNonEmpty) == messageNonEmpty,
                    "host inject identity") &&
             ok;
        ok = expect(ShouldPushStoredLinkshellMessage(messageNonEmpty) ==
                        inlineShouldPushStoredLinkshellMessage(messageNonEmpty),
                    "host inject dual-wire free == inline") &&
             ok;
    }

    return ok;
}
