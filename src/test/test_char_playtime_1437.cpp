#include "test_char_playtime_1437.h"

#include "map/char_playtime_capacity.h"

#include <chrono>
#include <iostream>

auto runCharPlaytime1437SelfTests() -> bool
{
    using namespace std::chrono_literals;
    using TimePoint = std::chrono::steady_clock::time_point;

    auto     current = TimePoint{100s};
    auto     saved   = TimePoint{5s};
    auto     stored  = TimePoint::duration{999s};
    unsigned calls   = 0;
    const auto now   = [&]()
    {
        ++calls;
        return current;
    };

    charplaytimehelpers::Set(TimePoint::duration{125s}, stored, saved, now);
    bool ok = stored == 125s && saved == TimePoint{100s} && calls == 1;

    current = TimePoint{130s};
    ok      = charplaytimehelpers::Get(false, stored, saved, now) == 125s &&
         saved == TimePoint{100s} && calls == 1 && ok;
    ok = charplaytimehelpers::Get(true, stored, saved, now) == 155s &&
         saved == TimePoint{130s} && calls == 2 && ok;

    current = TimePoint{142s};
    ok      = charplaytimehelpers::Get(true, stored, saved, now) == 167s &&
         saved == TimePoint{142s} && calls == 3 && ok;
    ok = charplaytimehelpers::Get(false, stored, saved, now) == 167s && calls == 3 && ok;

    current = TimePoint{200s} + 7ns;
    charplaytimehelpers::Set(TimePoint::duration{-5s + 3ns}, stored, saved, now);
    current += 3s + 11ns;
    ok = charplaytimehelpers::Get(true, stored, saved, now) == -2s + 14ns && calls == 5 && ok;

    if (!ok)
    {
        std::cerr << "char playtime 1437 self-test failed\n";
    }
    return ok;
}
