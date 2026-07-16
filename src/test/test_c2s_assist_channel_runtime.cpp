#include "test_c2s_assist_channel_runtime.h"

#include <iostream>

#include "map/packets/c2s/0x0b7_assist_channel.h"

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "c2s ASSIST_CHANNEL runtime self-test failed: " << label << '\n';
    }
    return condition;
}

auto testTargetAndDefaultBranches() -> bool
{
    bool ok = true;
    ok = expect(assistchannelhelpers::SelectAction(GP_CLI_COMMAND_ASSIST_CHANNEL_KIND::GiveThumbsUp, false, true, true) == AssistChannelAction::Error,
                "unresolved target errors before action routing") && ok;
    ok = expect(assistchannelhelpers::SelectAction(static_cast<GP_CLI_COMMAND_ASSIST_CHANNEL_KIND>(0xFF), true, true, true) == AssistChannelAction::None,
                "unexpected kind preserves process default no-op") && ok;
    return ok;
}

auto testKnownActionGatesAndRouting() -> bool
{
    bool ok = true;
    ok = expect(assistchannelhelpers::SelectAction(GP_CLI_COMMAND_ASSIST_CHANNEL_KIND::GiveThumbsUp, true, false, true) == AssistChannelAction::Error,
                "muted thumbs-up sender errors") && ok;
    ok = expect(assistchannelhelpers::SelectAction(GP_CLI_COMMAND_ASSIST_CHANNEL_KIND::GiveThumbsUp, true, true, false) == AssistChannelAction::ThumbsUpCooldown,
                "thumbs-up cooldown message") && ok;
    ok = expect(assistchannelhelpers::SelectAction(GP_CLI_COMMAND_ASSIST_CHANNEL_KIND::GiveThumbsUp, true, true, true) == AssistChannelAction::Forward,
                "thumbs-up forwards") && ok;
    ok = expect(assistchannelhelpers::SelectAction(GP_CLI_COMMAND_ASSIST_CHANNEL_KIND::IssueWarning, true, false, true) == AssistChannelAction::Error,
                "non-mentor or muted warning sender errors") && ok;
    ok = expect(assistchannelhelpers::SelectAction(GP_CLI_COMMAND_ASSIST_CHANNEL_KIND::IssueWarning, true, true, false) == AssistChannelAction::WarningCooldown,
                "warning cooldown message") && ok;
    ok = expect(assistchannelhelpers::SelectAction(GP_CLI_COMMAND_ASSIST_CHANNEL_KIND::IssueWarning, true, true, true) == AssistChannelAction::Forward,
                "warning forwards") && ok;
    ok = expect(assistchannelhelpers::SelectAction(GP_CLI_COMMAND_ASSIST_CHANNEL_KIND::AddToMuteList, true, false, true) == AssistChannelAction::Error,
                "unauthorized mute errors") && ok;
    ok = expect(assistchannelhelpers::SelectAction(GP_CLI_COMMAND_ASSIST_CHANNEL_KIND::AddToMuteList, true, true, false) == AssistChannelAction::Forward,
                "mute ignores cooldown fact and forwards") && ok;
    ok = expect(assistchannelhelpers::SelectAction(GP_CLI_COMMAND_ASSIST_CHANNEL_KIND::RemoveFromMuteList, true, true, true) == AssistChannelAction::Forward,
                "unmute forwards") && ok;
    return ok;
}

} // namespace

auto runC2SAssistChannelRuntimeSelfTests() -> bool
{
    return testTargetAndDefaultBranches() && testKnownActionGatesAndRouting();
}
