#include "test_dual_wield_1662.h"

#include "map/dual_wield_capacity.h"

#include <iostream>

namespace
{
using namespace dualwieldhelpers;

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "dual wield 1662 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runDualWield1662SelfTests() -> bool
{
    bool ok = true;

    // --- mob path: mod only (flag ignored) ---
    ok = expect(!IsDualWielding(true, false, 0), "mob mod 0 flag false") && ok;
    ok = expect(!IsDualWielding(true, true, 0), "mob mod 0 flag true ignored") && ok;
    ok = expect(IsDualWielding(true, false, 1), "mob mod 1") && ok;
    ok = expect(IsDualWielding(true, false, 1), "mob mod 1 flag false") && ok;
    ok = expect(IsDualWielding(true, true, 1), "mob mod 1 flag true") && ok;
    ok = expect(IsDualWielding(true, false, -1), "mob mod negative") && ok;
    ok = expect(IsDualWielding(true, false, 32767), "mob mod max int16") && ok;
    ok = expect(IsDualWielding(true, true, -32768), "mob mod min int16") && ok;

    // --- non-mob path: flag only (mod ignored) ---
    ok = expect(!IsDualWielding(false, false, 0), "non-mob flag false mod 0") && ok;
    ok = expect(!IsDualWielding(false, false, 1), "non-mob flag false mod nonzero ignored") && ok;
    ok = expect(!IsDualWielding(false, false, -5), "non-mob flag false mod negative ignored") && ok;
    ok = expect(IsDualWielding(false, true, 0), "non-mob flag true mod 0") && ok;
    ok = expect(IsDualWielding(false, true, 99), "non-mob flag true mod nonzero ignored") && ok;
    ok = expect(IsDualWielding(false, true, -1), "non-mob flag true mod negative ignored") && ok;

    // --- table: entity-kind scenarios ---
    ok = expect(!IsDualWielding(false, false, 0), "pc not dual") && ok;
    ok = expect(IsDualWielding(false, true, 0), "pc dual") && ok;
    ok = expect(IsDualWielding(false, true, 1), "pc dual with stray mod") && ok;
    ok = expect(!IsDualWielding(false, false, 1), "pc not dual with stray mod") && ok;
    ok = expect(!IsDualWielding(true, false, 0), "mob no mod") && ok;
    ok = expect(IsDualWielding(true, false, 1), "mob with mod") && ok;
    ok = expect(!IsDualWielding(true, true, 0), "mob flag alone not enough") && ok;
    ok = expect(IsDualWielding(true, true, 1), "mob both flag and mod") && ok;
    ok = expect(IsDualWielding(false, true, 0), "pet uses flag not mod") && ok;
    ok = expect(IsDualWielding(false, true, 5), "trust uses flag") && ok;

    return ok;
}
