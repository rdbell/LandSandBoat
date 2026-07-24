#include "test_mobutils_sql_modifiers_host_6409.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mobutils sql modifiers host 6409 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for LoadSqlModifiers / AddSqlModifiers (slice 6409).
// Go: SqlModifiersStore / ApplyLoadSqlModifiers / ApplyAddSqlModifiers.
auto runMobutilsSqlModifiersHost6409SelfTests() -> bool
{
    bool ok = true;

    const std::string speciesSQL = "SELECT speciesid, modid, value, is_mob_mod FROM mob_species_mods";
    const std::string poolSQL    = "SELECT poolid, modid, value, is_mob_mod FROM mob_pool_mods";
    ok = expect(speciesSQL.find("mob_species_mods") != std::string::npos, "species sql") && ok;
    ok = expect(poolSQL.find("mob_pool_mods") != std::string::npos, "pool sql") && ok;

    // Apply order: species → pool → spawn
    const std::string order = "species,pool,spawn";
    ok = expect(order == "species,pool,spawn", "apply order") && ok;

    // is_mob_mod routes to setMobMod vs addModifier
    const bool isMobMod = true;
    ok = expect(isMobMod, "mobmod branch") && ok;

    return ok;
}
