#include "test_mod_snapshot_1683.h"

#include "map/mod_snapshot_capacity.h"

#include <iostream>
#include <unordered_map>

namespace
{
using namespace modsnapshothelpers;

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "mod_snapshot 1683 self-test failed: " << label << '\n';
    }
    return condition;
}

auto mapsEqual(const std::unordered_map<std::uint16_t, std::int16_t>& a,
               const std::unordered_map<std::uint16_t, std::int16_t>& b) -> bool
{
    if (a.size() != b.size())
    {
        return false;
    }
    for (const auto& [k, v] : a)
    {
        const auto it = b.find(k);
        if (it == b.end() || it->second != v)
        {
            return false;
        }
    }
    return true;
}
} // namespace

auto runModSnapshot1683SelfTests() -> bool
{
    bool ok = true;

    // --- Empty ---
    {
        const auto snap = SnapshotMods({});
        ok = expect(snap.empty(), "SnapshotMods empty") && ok;

        const auto restored = RestoreMods({});
        ok = expect(restored.empty(), "RestoreMods empty") && ok;
    }

    // --- SnapshotMods content + isolation ---
    {
        std::unordered_map<std::uint16_t, std::int16_t> src{
            { 1, 10 },  // Mod::DEF
            { 2, -5 },
            { 49, 100 },
        };
        auto snap = SnapshotMods(src);
        ok = expect(mapsEqual(snap, src), "SnapshotMods content") && ok;

        // Mutating snapshot must not mutate source.
        snap[1] = 999;
        snap[99] = 1;
        snap.erase(2);
        ok = expect(src.at(1) == 10, "src[1] intact after snap mutate") && ok;
        ok = expect(src.find(99) == src.end(), "src no key 99") && ok;
        ok = expect(src.at(2) == -5, "src[2] intact after snap erase") && ok;

        // Mutating source must not mutate prior snapshot.
        auto snap2 = SnapshotMods(src);
        src[1]     = 0;
        src[200]   = 7;
        src.erase(49);
        ok = expect(snap2.at(1) == 10, "snap2[1] intact after src mutate") && ok;
        ok = expect(snap2.find(200) == snap2.end(), "snap2 no key 200") && ok;
        ok = expect(snap2.at(49) == 100, "snap2[49] intact after src erase") && ok;
    }

    // --- RestoreMods content + isolation ---
    {
        std::unordered_map<std::uint16_t, std::int16_t> saved{
            { 1, 42 },
            { 3, -1 },
            { 50, 200 },
        };
        auto restored = RestoreMods(saved);
        ok = expect(mapsEqual(restored, saved), "RestoreMods content") && ok;

        // Mutating restore result must not mutate saved.
        restored[1]  = 0;
        restored[77] = 9;
        restored.erase(3);
        ok = expect(saved.at(1) == 42, "saved[1] intact after restore mutate") && ok;
        ok = expect(saved.find(77) == saved.end(), "saved no key 77") && ok;
        ok = expect(saved.at(3) == -1, "saved[3] intact after restore erase") && ok;
    }

    // --- Restore overwrites (wholesale replace of current) ---
    {
        // Host discards old current and assigns current = RestoreMods(saved).
        // Keys present only in old current must not appear in the result.
        const std::unordered_map<std::uint16_t, std::int16_t> saved{
            { 1, 10 },
            { 2, 20 },
        };
        // Old current (for documentation of overwrite semantics only):
        // { 1:99, 2:88, 3:77, 49:5 } — none of 3/49 survive restore.
        const auto restored = RestoreMods(saved);
        ok = expect(restored.size() == 2, "restore overwrite size 2") && ok;
        ok = expect(restored.at(1) == 10 && restored.at(2) == 20, "restore overwrite values") && ok;
        ok = expect(restored.find(3) == restored.end(), "restore drop old key 3") && ok;
        ok = expect(restored.find(49) == restored.end(), "restore drop old key 49") && ok;

        // Empty saved clears all mods.
        ok = expect(RestoreMods({}).empty(), "restore empty clears") && ok;
    }

    // --- Snapshot then restore round-trip ---
    {
        std::unordered_map<std::uint16_t, std::int16_t> original{
            { 1, 15 },
            { 100, -3 },
            { 389, 50 },
        };
        const auto saved = SnapshotMods(original);

        // Mutate live mods after save.
        original[1]   = 0;
        original[999] = 1;
        original.erase(100);

        const auto restored = RestoreMods(saved);
        const std::unordered_map<std::uint16_t, std::int16_t> want{
            { 1, 15 },
            { 100, -3 },
            { 389, 50 },
        };
        ok = expect(mapsEqual(restored, want), "round-trip restore") && ok;
        ok = expect(mapsEqual(saved, want), "saved intact after restore") && ok;
    }

    return ok;
}
