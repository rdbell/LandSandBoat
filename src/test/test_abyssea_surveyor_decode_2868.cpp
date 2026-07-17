#include "test_abyssea_surveyor_decode_2868.h"

#include "map/abyssea_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "abyssea surveyor decode 2868 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua surveyorOnEventFinish option-field formula for dual-wire checks:
//   optionSelected   = bit.band(option, 0xF)
//   additionalStones = bit.rshift(option, 16)
auto inlineDecodeSurveyorOption(const uint32 option) -> abysseahelpers::SurveyorOption
{
    return abysseahelpers::SurveyorOption{
        .choice          = static_cast<uint8>(option & 0x0FU),
        .requestedStones = static_cast<int32>(option >> 16),
    };
}

auto optionsEqual(const abysseahelpers::SurveyorOption& a, const abysseahelpers::SurveyorOption& b) -> bool
{
    return a.choice == b.choice && a.requestedStones == b.requestedStones;
}

} // namespace

// Pure dual-wire expansion for abysseahelpers::DecodeSurveyorOption
// (Lua surveyorOnEventFinish option & 0xF / >> 16 split).
auto runAbysseaSurveyorDecode2868SelfTests() -> bool
{
    using abysseahelpers::DecodeSurveyorOption;
    using abysseahelpers::SurveyorOption;

    bool ok = true;

    const struct
    {
        uint32      option;
        uint8       wantChoice;
        int32       wantStones;
        const char* label;
    } cases[] = {
        { 0, 0, 0, "zero" },
        { 1, 1, 0, "low-only choice 1" },
        { 2, 2, 0, "choice 2 no stones" },
        { 3, 3, 0, "choice 3 no stones" },
        { 0x0F, 0x0F, 0, "full low nibble" },
        { 0x12, 2, 0, "low byte noise above nibble" },
        { 0x00010000, 0, 1, "one stone high word" },
        { 0x00030012, 2, 3, "choice 2 / 3 stones residual pin" },
        { (5U << 16) | 2U, 2, 5, "5 stones choice 2" },
        { (2U << 16) | 3U, 3, 2, "2 stones choice 3" },
        { 0xFFFF0000, 0, 0xFFFF, "hi full lo zero" },
        { 0x0000FFFF, 0x0F, 0, "lo full hi zero (choice nibble only)" },
        { 0xFFFFFFFF, 0x0F, 0xFFFF, "all bits" },
        { 0x000A000F, 0x0F, 10, "10 stones full choice nibble" },
        { 0x00000010, 0, 0, "bit 4 ignored by low-nibble mask" },
    };

    for (const auto& c : cases)
    {
        const SurveyorOption got     = DecodeSurveyorOption(c.option);
        const SurveyorOption inlineF = inlineDecodeSurveyorOption(c.option);

        ok = expect(got.choice == c.wantChoice && got.requestedStones == c.wantStones, c.label) && ok;
        ok = expect(optionsEqual(got, inlineF), "dual-wire free == inline Lua formula") && ok;
    }

    // Explicit residual dual-wire pin used by Go TestDecodeSurveyorOption.
    const SurveyorOption residual = DecodeSurveyorOption(0x00030012);
    ok = expect(residual.choice == 2 && residual.requestedStones == 3, "residual 0x00030012 pin") && ok;

    // Held-stone clamp remains host-owned: pure decode does not min with held.
    const SurveyorOption five = DecodeSurveyorOption((5U << 16) | 2U);
    ok = expect(five.requestedStones == 5, "decode does not clamp held stones") && ok;

    return ok;
}
