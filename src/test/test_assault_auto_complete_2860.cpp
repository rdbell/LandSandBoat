#include "test_assault_auto_complete_2860.h"

#include "map/assault_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "assault auto-complete 2860 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua onInstanceProgressUpdate formula for dual-wire checks:
//   requiredProgress and progress >= requiredProgress and not completed
// requiredProgress uses Lua truthy semantics (0 → false).
auto inlineShouldAutoComplete(const int32 requiredProgress, const int32 progress, const bool alreadyCompleted) -> bool
{
    return requiredProgress != 0 && progress >= requiredProgress && !alreadyCompleted;
}

} // namespace

// Pure dual-wire expansion for assaulthelpers::ShouldAutoComplete
// (Lua InstanceAssault:onInstanceProgressUpdate complete gate).
auto runAssaultAutoComplete2860SelfTests() -> bool
{
    using assaulthelpers::ProgressMeetsRequired;
    using assaulthelpers::ShouldAutoComplete;

    bool ok = true;

    // Boundary: unset requiredProgress never completes.
    ok = expect(!ShouldAutoComplete(0, 0, false), "unset required zero progress") && ok;
    ok = expect(!ShouldAutoComplete(0, 100, false), "unset required high progress") && ok;
    ok = expect(!ShouldAutoComplete(0, 100, true), "unset required already completed") && ok;

    // Leujaoam Cleansing sample (requiredProgress = 14).
    ok = expect(!ShouldAutoComplete(14, 13, false), "Leujaoam below") && ok;
    ok = expect(ShouldAutoComplete(14, 14, false), "Leujaoam meet") && ok;
    ok = expect(ShouldAutoComplete(14, 20, false), "Leujaoam exceed") && ok;
    ok = expect(!ShouldAutoComplete(14, 14, true), "Leujaoam meet already completed") && ok;
    ok = expect(!ShouldAutoComplete(14, 20, true), "Leujaoam exceed already completed") && ok;

    // ProgressMeetsRequired pure threshold (no truthy/completed gates).
    ok = expect(!ProgressMeetsRequired(13, 14), "threshold below") && ok;
    ok = expect(ProgressMeetsRequired(14, 14), "threshold equal") && ok;
    ok = expect(ProgressMeetsRequired(15, 14), "threshold above") && ok;
    ok = expect(ProgressMeetsRequired(0, 0), "threshold zero>=zero") && ok;

    // Dual-wire matches inline formula across a small table.
    const struct
    {
        int32       required;
        int32       progress;
        bool        completed;
        bool        want;
        const char* label;
    } cases[] = {
        { 0, 0, false, false, "table unset zero" },
        { 0, 100, false, false, "table unset high" },
        { 14, 13, false, false, "table below" },
        { 14, 14, false, true, "table meet" },
        { 14, 20, false, true, "table exceed" },
        { 14, 14, true, false, "table meet completed" },
        { 14, 20, true, false, "table exceed completed" },
        { 1, 0, false, false, "table req1 prog0" },
        { 1, 1, false, true, "table req1 prog1" },
        { -1, 0, false, true, "table negative required truthy" },
        { 100, 99, false, false, "table large below" },
        { 100, 100, false, true, "table large meet" },
    };

    for (const auto& c : cases)
    {
        const bool got       = ShouldAutoComplete(c.required, c.progress, c.completed);
        const bool inlineGot = inlineShouldAutoComplete(c.required, c.progress, c.completed);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineGot, "dual-wire free == inline Lua formula") && ok;

        // Compose dual-wire: free function uses ProgressMeetsRequired after gates.
        if (c.required != 0 && !c.completed)
        {
            const bool composed = ProgressMeetsRequired(c.progress, c.required);
            ok                  = expect(got == composed, "dual-wire free == ProgressMeetsRequired compose") && ok;
        }
    }

    // Composition pins: free function dual-wires ProgressMeetsRequired.
    ok = expect(ShouldAutoComplete(14, 14, false) == ProgressMeetsRequired(14, 14), "compose meet") && ok;
    ok = expect(ShouldAutoComplete(14, 13, false) == ProgressMeetsRequired(13, 14), "compose below") && ok;
    ok = expect(ShouldAutoComplete(14, 14, true) == false, "compose already completed") && ok;
    ok = expect(ShouldAutoComplete(0, 14, false) == false, "compose unset required") && ok;

    return ok;
}
