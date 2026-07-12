#include "test_char_entity_update_1469.h"

#include "map/char_entity_update_capacity.h"

#include <iostream>
#include <string>

auto runCharEntityUpdate1469SelfTests() -> bool
{
    bool ok = true;
    for (const bool pending : { false, true })
    {
        for (const bool character : { false, true })
        {
            std::string selected{};
            charentityupdatehelpers::Apply(
                pending,
                character,
                [&]() { selected = "update-character"; },
                [&]() { selected = "update-entity"; },
                [&]() { selected = "new-character"; },
                [&]() { selected = "new-entity"; });
            const auto expected = pending ? (character ? "update-character" : "update-entity") :
                                            (character ? "new-character" : "new-entity");
            ok = selected == expected && ok;
        }
    }

    if (!ok)
    {
        std::cerr << "char entity update 1469 self-test failed\n";
    }
    return ok;
}
