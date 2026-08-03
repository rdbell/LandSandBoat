#include "test_battle_add_tp_1636.h"

#include "map/battle_add_tp_capacity.h"

#include <iostream>

auto runBattleAddTP1636SelfTests() -> bool
{
    using namespace battleaddtphelpers;

    bool ok = true;

    ok = ok && TypePC == 0x01 && TypeMob == 0x04 && TypePet == 0x08;
    ok = ok && TypeTrust == 0x20 && TypeFellow == 0x40;
    ok = ok && TPMax == 3000;

    ok = ok && ResolveTPMultiplier(TypePC, false, 1.5f, 1.0f, 1.0f, 1.0f, 1.0f) == 1.5f;
    ok = ok && ResolveTPMultiplier(TypePet, false, 1.0f, 2.0f, 1.0f, 1.0f, 1.0f) == 2.0f;
    ok = ok && ResolveTPMultiplier(TypeMob, true, 1.0f, 2.0f, 3.0f, 1.0f, 1.0f) == 2.0f; // charmed pet
    ok = ok && ResolveTPMultiplier(TypeMob, false, 1.0f, 2.0f, 3.0f, 1.0f, 1.0f) == 3.0f;
    ok = ok && ResolveTPMultiplier(TypeTrust, false, 1.0f, 1.0f, 1.0f, 1.25f, 1.0f) == 1.25f;
    ok = ok && ResolveTPMultiplier(TypeFellow, false, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f) == 0.5f;
    ok = ok && ResolveTPMultiplier(0x02, false, 9.0f, 9.0f, 9.0f, 9.0f, 9.0f) == 1.0f; // NPC default

    ok = ok && ApplyInhibitTP(100, 0) == 100;
    ok = ok && ApplyInhibitTP(100, 25) == 75;
    ok = ok && ApplyInhibitTP(-50, 50) == -50; // no inhibit on loss

    ok = ok && ScaleTPGain(100, 0, 1.0f) == 100;
    ok = ok && ScaleTPGain(100, 0, 2.0f) == 200;
    ok = ok && ScaleTPGain(100, 50, 2.0f) == 100; // 50 after inhibit, *2 = 100
    ok = ok && ScaleTPGain(-100, 50, 2.0f) == -100;

    {
        // 1000 + 500 = 1500, return abs(1000-1500)=500
        const auto r = ResolveAddTP(1000, 500);
        ok           = ok && r.newTP == 1500 && r.returnedAbsDelta == 500 && r.setUpdateHP;
    }
    {
        // cap at 3000: 2800 + 500 → 3000, abs(2800-3000)=200
        const auto r = ResolveAddTP(2800, 500);
        ok           = ok && r.newTP == 3000 && r.returnedAbsDelta == 200;
    }
    {
        // LSB narrows currentTP + scaledTP to int16 before clamping:
        // 3000 + 32767 → -29769 → 0, not 3000.
        const auto r = ResolveAddTP(3000, 32767);
        ok           = ok && r.newTP == 0 && r.returnedAbsDelta == 3000 && r.setUpdateHP;
    }
    {
        // floor at 0: 100 + (-200) → 0, abs(100-0)=100
        const auto r = ResolveAddTP(100, -200);
        ok           = ok && r.newTP == 0 && r.returnedAbsDelta == 100 && r.setUpdateHP;
    }
    {
        const auto r = ResolveAddTP(1000, 0);
        ok           = ok && r.newTP == 1000 && r.returnedAbsDelta == 0 && !r.setUpdateHP;
    }

    {
        // Full: PC, inhibit 0, multi 1.5, 1000 + 100*1.5 = 1150
        const auto r = ResolveFullAddTP(1000, 100, 0, TypePC, false, 1.5f, 1.0f, 1.0f, 1.0f, 1.0f);
        ok           = ok && r.newTP == 1150 && r.returnedAbsDelta == 150;
    }

    if (!ok)
    {
        std::cerr << "battle add TP 1636 self-test failed\n";
    }
    return ok;
}
