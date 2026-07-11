#include "test_login_character_create_1320.h"

#include "common/cbasetypes.h"
#include "login/character_create.h"

#include <cstdint>
#include <iostream>
#include <limits>
#include <string>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "login character create 1320 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runLoginCharacterCreate1320SelfTests() -> bool
{
    using gate = loginHelpers::create_char_field_gate;
    bool ok    = true;

    ok = expect(loginHelpers::CreateCharRaceOffset == 48, "race offset") && ok;
    ok = expect(loginHelpers::CreateCharJobOffset == 50, "job offset") && ok;
    ok = expect(loginHelpers::CreateCharNationOffset == 54, "nation offset") && ok;
    ok = expect(loginHelpers::CreateCharSizeOffset == 57, "size offset") && ok;
    ok = expect(loginHelpers::CreateCharFaceOffset == 60, "face offset") && ok;

    ok = expect(loginHelpers::ClassifyCreateCharRace(1) == gate::ALLOW, "race min") && ok;
    ok = expect(loginHelpers::ClassifyCreateCharRace(8) == gate::ALLOW, "race max") && ok;
    ok = expect(loginHelpers::ClassifyCreateCharRace(0) == gate::INVALID_RACE, "race 0") && ok;
    ok = expect(loginHelpers::ClassifyCreateCharRace(9) == gate::INVALID_RACE, "race 9") && ok;

    ok = expect(loginHelpers::ClassifyCreateCharSize(0) == gate::ALLOW, "size 0") && ok;
    ok = expect(loginHelpers::ClassifyCreateCharSize(2) == gate::ALLOW, "size 2") && ok;
    ok = expect(loginHelpers::ClassifyCreateCharSize(3) == gate::INVALID_SIZE, "size 3") && ok;

    ok = expect(loginHelpers::ClassifyCreateCharFace(0) == gate::ALLOW, "face 0") && ok;
    ok = expect(loginHelpers::ClassifyCreateCharFace(15) == gate::ALLOW, "face 15") && ok;
    ok = expect(loginHelpers::ClassifyCreateCharFace(16) == gate::INVALID_FACE, "face 16") && ok;

    ok = expect(loginHelpers::ClassifyCreateCharNation(0) == gate::ALLOW, "nation sandy") && ok;
    ok = expect(loginHelpers::ClassifyCreateCharNation(2) == gate::ALLOW, "nation windy") && ok;
    ok = expect(loginHelpers::ClassifyCreateCharNation(3) == gate::INVALID_NATION, "nation 3") && ok;

    ok = expect(loginHelpers::ClampStartingJob(1) == 1, "job min") && ok;
    ok = expect(loginHelpers::ClampStartingJob(6) == 6, "job max") && ok;
    ok = expect(loginHelpers::ClampStartingJob(0) == 1, "job clamp low") && ok;
    ok = expect(loginHelpers::ClampStartingJob(7) == 6, "job clamp high") && ok;
    ok = expect(loginHelpers::ClampStartingJob(255) == 6, "job clamp 255") && ok;

    ok = expect(loginHelpers::StartingZoneForNation(0, 0) == 0xE6, "sandy z0") && ok;
    ok = expect(loginHelpers::StartingZoneForNation(0, 1) == 0xE7, "sandy z1") && ok;
    ok = expect(loginHelpers::StartingZoneForNation(0, 2) == 0xE8, "sandy z2") && ok;
    ok = expect(loginHelpers::StartingZoneForNation(1, 0) == 0xEA, "bastok z0") && ok;
    ok = expect(loginHelpers::StartingZoneForNation(1, 2) == 0xEC, "bastok z2") && ok;
    ok = expect(loginHelpers::StartingZoneForNation(2, 0) == 0xEE, "windy z0") && ok;
    ok = expect(loginHelpers::StartingZoneForNation(2, 1) == 0xF0, "windy z1") && ok;
    ok = expect(loginHelpers::StartingZoneForNation(2, 2) == 0xF1, "windy z2") && ok;
    ok = expect(loginHelpers::StartingZoneForNation(0, 3) == 0xE6, "index mod 3") && ok;
    ok = expect(loginHelpers::StartingZoneForNation(0, 5) == 0xE8, "index 5 -> 2") && ok;

    ok = expect(loginHelpers::NextCharacterID(0) == 1, "empty table next id") && ok;
    ok = expect(loginHelpers::NextCharacterID(1000) == 1001, "mid next id") && ok;
    ok = expect(loginHelpers::NextCharacterID(std::numeric_limits<uint32>::max()) == 0, "wrap next id") && ok;

    const auto ids = loginHelpers::PackCreateCharInfoIDs(0x00012345);
    ok = expect(ids.ffxi_id == 0x00012345, "content id") && ok;
    ok = expect(ids.ffxi_id_world == 0x2345, "world low") && ok;
    ok = expect(ids.worldid == 0 && ids.status == 1 && ids.race_change == 0 && ids.renamef == 0, "flags") && ok;
    ok = expect(ids.ffxi_id_world_tbl == 0x01, "high byte") && ok;

    const auto idsMax = loginHelpers::PackCreateCharInfoIDs(0x00ABCDEF);
    ok = expect(idsMax.ffxi_id_world == 0xCDEF && idsMax.ffxi_id_world_tbl == 0xAB, "pack high") && ok;

    ok = expect(loginHelpers::FormatInvalidCreateRace("Alice", 9) ==
                    "Alice attempted to create character with invalid race 9",
                "race diag") &&
         ok;
    ok = expect(loginHelpers::FormatInvalidStartingJobSubstitution("Bob", 12, 6) ==
                    "Bob attempted to create invalid starting job 12 substituting 6",
                "job diag") &&
         ok;

    return ok;
}
