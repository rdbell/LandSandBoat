#pragma once

#include <utility>

// Pure blacklistutils::SendBlacklist full-chunk flag policy.
// Host injects totalCount / rowCount / chunkSize; helpers never touch DB or packets.

namespace blacklistutilshelpers
{

// FullChunkFlags returns {reset, last} for a full SAVE_BLACK chunk emission.
// Production hard-codes chunkSize 12; default matches that constant.
//   Reset = totalCount <= chunkSize  (first full chunk only when chunkSize==12)
//   Last  = totalCount == rowCount   (rowCount is unfiltered DB result size)
inline auto FullChunkFlags(const int totalCount, const int rowCount, const int chunkSize = 12) -> std::pair<bool, bool>
{
    return { totalCount <= chunkSize, totalCount == rowCount };
}

} // namespace blacklistutilshelpers
