#include "test_char_skill_save_host_7071.h"
#include "common/database.h"
#include "common/database/sqlite/sqlite_database.h"
#include "map/entities/char_entity.h"
#include "map/utils/charutils.h"
#include <iostream>

namespace
{

class ScopedDatabase final
{
public:
    explicit ScopedDatabase(db::Database& d)
    : p(&db::getDatabase())
    {
        db::setDatabase(&d);
    }

    ~ScopedDatabase()
    {
        db::setDatabase(p);
    }

private:
    db::Database* p;
};

} // namespace

// Direct SaveCharSkills characterization (slice 7071): valid skills upsert;
// IDs at MAX_SKILLTYPE and above leave existing rows unchanged.
auto runCharSkillSaveHost7071SelfTests() -> bool
{
    db::SQLiteDatabase d("file:char_skill_save_host_7071?mode=memory&cache=shared");
    ScopedDatabase     active(d);
    d.executeScript("CREATE TABLE char_skills (charid INTEGER, skillid INTEGER, value INTEGER, rank INTEGER, PRIMARY KEY(charid, skillid)); INSERT INTO char_skills VALUES (77, 1, 1, 1);");
    CCharEntity c;
    c.id                  = 77;
    c.RealSkills.skill[1] = 321;
    c.RealSkills.rank[1]  = 7;
    charutils::SaveCharSkills(&c, 1);
    charutils::SaveCharSkills(&c, MAX_SKILLTYPE);
    const auto row = db::preparedStmt("SELECT value, rank FROM char_skills WHERE charid = ? AND skillid = ?", 77, 1);
    const bool ok  = row && row->rowsCount() == 1 && row->next() && row->get<uint16>("value") == 321 && row->get<uint8>("rank") == 7;
    if (!ok)
        std::cerr << "character skill save host 7071 self-test failed\n";
    return ok;
}
