#include "test_jobpoints_exist_2815.h"

#include "map/job_points_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "jobpoints exist 2815 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectU16(const uint16 got, const uint16 want, const char* const label) -> bool
{
    if (got != want)
    {
        std::cerr << "jobpoints exist 2815 self-test failed: " << label
                  << " got=" << got << " want=" << want << '\n';
        return false;
    }
    return true;
}

auto expectU8(const uint8 got, const uint8 want, const char* const label) -> bool
{
    if (got != want)
    {
        std::cerr << "jobpoints exist 2815 self-test failed: " << label
                  << " got=" << static_cast<unsigned>(got)
                  << " want=" << static_cast<unsigned>(want) << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runJobPointsExist2815SelfTests() -> bool
{
    using namespace jobpointshelpers;

    bool ok = true;

    // --- Pure constants ---
    ok = expectU16(kCategoryStart, 0x020, "kCategoryStart") && ok;
    ok = expectU16(kCategoryCount, 22, "kCategoryCount") && ok;
    ok = expectU16(kJPTypePerCategory, 10, "kJPTypePerCategory") && ok;

    // --- CategoryIndexByType / TypeIndex pure forms ---
    // WAR first type JP_MIGHTY_STRIKES_EFFECT = 0x020
    ok = expectU16(CategoryIndexByType(0x020), 1, "cat WAR") && ok;
    // RUN first type = 0x2C0
    ok = expectU16(CategoryIndexByType(0x2C0), 22, "cat RUN") && ok;
    ok = expectU8(TypeIndex(0x020), 0, "type index 0") && ok;
    ok = expectU8(TypeIndex(0x021), 1, "type index 1") && ok;
    ok = expectU8(TypeIndex(0x022), 2, "type index 2") && ok;
    ok = expectU8(TypeIndex(0x020 + 9), 9, "type index 9") && ok;
    ok = expectU8(TypeIndex(0x020 + 10), 10, "type index 10") && ok;

    // --- Accept: valid WAR / RUN types ---
    ok = expect(IsJobPointExistPure(0x020), "WAR type 0 exists") && ok;
    ok = expect(IsJobPointExistPure(0x020 + 9), "WAR type 9 exists") && ok;
    ok = expect(IsJobPointExistPure(0x2C0 + 9), "RUN type 9 exists") && ok;

    // LSB quirk: type index == JPTypePerCategory (10) passes (> check)
    ok = expect(IsJobPointExistPure(0x020 + 10), "type index 10 accepted") && ok;

    // LSB quirk: category index 23 accepted ((23-1) > 22 is false)
    ok = expect(IsJobPointExistPure(static_cast<uint16>(23 << 5)), "cat 23 accepted") && ok;

    // --- Reject: below category start ---
    ok = expect(!IsJobPointExistPure(0x01F), "below CategoryStart") && ok;
    ok = expect(!IsJobPointExistPure(0), "zero type") && ok;

    // --- Reject: category index 24 ((24-1) > 22) ---
    ok = expect(!IsJobPointExistPure(static_cast<uint16>(24 << 5)), "cat 24 rejected") && ok;

    // --- Reject: type index 11 ---
    ok = expect(!IsJobPointExistPure(0x020 + 11), "type index 11 rejected") && ok;

    // Boundary: last accepted category/start type
    ok = expect(IsJobPointExistPure(kCategoryStart), "exactly CategoryStart") && ok;

    return ok;
}
