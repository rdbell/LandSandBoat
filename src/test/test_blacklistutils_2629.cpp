#include "test_blacklistutils_2629.h"

#include <iostream>
#include <string>

#include "map/utils/blacklistutils.h"

auto runBlacklistUtils2629SelfTests() -> bool
{
    const auto [reset12, last12] = blacklistutils::detail::FullChunkFlags(12, 12);
    const auto [reset13, last13] = blacklistutils::detail::FullChunkFlags(13, 13);
    const auto [filteredReset, filteredLast] = blacklistutils::detail::FullChunkFlags(12, 13);
    const bool ok = blacklistutils::detail::IsNameCharactersOnly("Alice") &&
                    !blacklistutils::detail::IsNameCharactersOnly(std::string("a\0b", 3)) &&
                    !blacklistutils::detail::IsNameCharactersOnly(std::string("bob\0", 4)) &&
                    !blacklistutils::detail::IsNameCharactersOnly("a_b") &&
                    !blacklistutils::detail::IsNameCharactersOnly("a b") && reset12 && last12 &&
                    !reset13 && last13 && filteredReset && !filteredLast;
    if (!ok) std::cerr << "blacklist utils self-test failed\n";
    return ok;
}
