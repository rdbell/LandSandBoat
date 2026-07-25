#include "test_char_var_entity_ops_6886.h"

#include "map/char_var_entity_ops.h"

#include <iostream>

auto runCharVarEntityOps6886SelfTests() -> bool
{
    using namespace charvarentityopshelpers;

    const bool ok = ReadActionFor(false) == ReadAction::ReturnZero &&
                    ReadActionFor(true) == ReadAction::ReadLocalCache &&
                    SetActionFor(false) == SetAction::Noop &&
                    SetActionFor(true) == SetAction::UpdateLocalCache &&
                    ClearPrefixActionFor(false) == ClearPrefixAction::ReturnZero &&
                    ClearPrefixActionFor(true) == ClearPrefixAction::ClearLocalCache;
    if (!ok)
    {
        std::cerr << "char var entity ops 6886 self-test failed\n";
    }
    return ok;
}
