#include "test_char_start_synth_1467.h"

#include "map/char_start_synth_capacity.h"

#include <iostream>

auto runCharStartSynth1467SelfTests() -> bool
{
    int calls = 0;
    bool ok  = !charstartsynthhelpers::Apply(false, [&]()
    {
        ++calls;
        return true;
    });
    ok = calls == 0 && ok;

    ok = !charstartsynthhelpers::Apply(true, [&]()
    {
        ++calls;
        return false;
    }) && ok;
    ok = calls == 1 && ok;

    ok = charstartsynthhelpers::Apply(true, [&]()
    {
        ++calls;
        return true;
    }) && ok;
    ok = calls == 2 && ok;

    if (!ok)
    {
        std::cerr << "char start synth 1467 self-test failed\n";
    }
    return ok;
}
