#include "test_gmcall_faq_type_plan_2791.h"

#include "map/gmcall_packet_handlers.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "gmcall faq type plan 2791 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runGMCallFAQTypePlan2791SelfTests() -> bool
{
    using gmcall::handler::FAQGMCallDisposition;
    using gmcall::handler::PlanFAQGMCallType;

    bool ok = true;

    // Production ignored kinds.
    ok = expect(PlanFAQGMCallType(GP_CLI_COMMAND_FAQ_GMCALL_TYPE::AddHistory) == FAQGMCallDisposition::Ignore, "AddHistory ignore") && ok;
    ok = expect(PlanFAQGMCallType(GP_CLI_COMMAND_FAQ_GMCALL_TYPE::GMNotice) == FAQGMCallDisposition::Ignore, "GMNotice ignore") && ok;

    // Production GMCall path.
    ok = expect(PlanFAQGMCallType(GP_CLI_COMMAND_FAQ_GMCALL_TYPE::GMCall) == FAQGMCallDisposition::ProcessGMCall, "GMCall process") && ok;

    // Defensive: non-enum bit patterns fall through as Ignore (matches original switch).
    ok = expect(PlanFAQGMCallType(static_cast<GP_CLI_COMMAND_FAQ_GMCALL_TYPE>(0)) == FAQGMCallDisposition::Ignore, "type 0 ignore") && ok;
    ok = expect(PlanFAQGMCallType(static_cast<GP_CLI_COMMAND_FAQ_GMCALL_TYPE>(4)) == FAQGMCallDisposition::Ignore, "type 4 ignore") && ok;
    ok = expect(PlanFAQGMCallType(static_cast<GP_CLI_COMMAND_FAQ_GMCALL_TYPE>(15)) == FAQGMCallDisposition::Ignore, "type 15 ignore") && ok;

    return ok;
}
