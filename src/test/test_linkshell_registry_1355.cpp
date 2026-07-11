#include "test_linkshell_registry_1355.h"

#include "map/linkshell_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "linkshell registry 1355 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runLinkshellRegistry1355SelfTests() -> bool
{
    using load_gate     = linkshellhelpers::load_linkshell_gate;
    using register_gate = linkshellhelpers::register_linkshell_gate;
    bool ok             = true;

    ok = expect(linkshellhelpers::ClassifyLoadLinkshell(true, true) == load_gate::FOUND, "load found") && ok;
    ok = expect(linkshellhelpers::ClassifyLoadLinkshell(false, true) == load_gate::NOT_FOUND, "load query fail") && ok;
    ok = expect(linkshellhelpers::ClassifyLoadLinkshell(true, false) == load_gate::NOT_FOUND, "load no row") && ok;

    ok = expect(linkshellhelpers::ShouldUnloadLinkshell(true), "unload") && ok;
    ok = expect(!linkshellhelpers::ShouldUnloadLinkshell(false), "no unload") && ok;

    ok = expect(linkshellhelpers::ShouldRejectNullOnlineMember(true), "online null") && ok;
    ok = expect(linkshellhelpers::FormatOnlineMemberNullWarning() == "PChar is null.", "online null warn") && ok;
    ok = expect(linkshellhelpers::ShouldProcessLinkshellItem(true, true), "process item") && ok;
    ok = expect(!linkshellhelpers::ShouldProcessLinkshellItem(true, false), "not ls type") && ok;
    ok = expect(!linkshellhelpers::ShouldProcessLinkshellItem(false, true), "null item") && ok;
    ok = expect(linkshellhelpers::ShouldLoadLinkshellOnOnlineAdd(false), "cache miss load") && ok;
    ok = expect(!linkshellhelpers::ShouldLoadLinkshellOnOnlineAdd(true), "cache hit") && ok;
    ok = expect(linkshellhelpers::ShouldAddMemberAfterOnlineLookup(true), "add after load") && ok;
    ok = expect(!linkshellhelpers::OnlineMemberAlwaysReturnsFalse(), "always false") && ok;

    ok = expect(linkshellhelpers::ShouldEraseLinkshellAfterDelOnline(false), "erase empty") && ok;
    ok = expect(!linkshellhelpers::ShouldEraseLinkshellAfterDelOnline(true), "keep non-empty") && ok;

    ok = expect(linkshellhelpers::IsValidLinkshellNameFromQuery(false, 1), "name valid query fail") && ok;
    ok = expect(linkshellhelpers::IsValidLinkshellNameFromQuery(true, 0), "name free") && ok;
    ok = expect(!linkshellhelpers::IsValidLinkshellNameFromQuery(true, 1), "name taken") && ok;

    ok = expect(linkshellhelpers::RegisterNewLinkshellPostRights == LSTYPE_PEARLSACK, "insert postrights") && ok;
    ok = expect(linkshellhelpers::ShouldAttemptRegisterInsert(true), "attempt insert") && ok;

    ok = expect(linkshellhelpers::ClassifyRegisterNewLinkshell(false, false, false, false, false) ==
                    register_gate::REJECT_NAME,
                "reg name") &&
         ok;
    ok = expect(linkshellhelpers::ClassifyRegisterNewLinkshell(true, false, false, false, false) ==
                    register_gate::REJECT_INSERT,
                "reg insert") &&
         ok;
    ok = expect(linkshellhelpers::ClassifyRegisterNewLinkshell(true, true, false, false, false) ==
                    register_gate::REJECT_SELECT,
                "reg select") &&
         ok;
    ok = expect(linkshellhelpers::ClassifyRegisterNewLinkshell(true, true, true, true, false) ==
                    register_gate::REJECT_LOAD,
                "reg load") &&
         ok;
    ok = expect(linkshellhelpers::ClassifyRegisterNewLinkshell(true, true, true, true, true) ==
                    register_gate::SUCCESS,
                "reg ok") &&
         ok;

    ok = expect(linkshellhelpers::ShouldReturnCachedLinkshell(true), "get cached") && ok;
    ok = expect(!linkshellhelpers::ShouldReturnCachedLinkshell(false), "get miss") && ok;

    return ok;
}
