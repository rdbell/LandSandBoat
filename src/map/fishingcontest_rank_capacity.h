#pragma once

#include <cstddef>
#include <cstdint>

// Pure RankContestEntries sort-order and rank-share assignment helpers.
// Host injects score / submit-time / measure scalars; helpers never touch
// FishingContestEntry storage or process-global contest vectors.

namespace fishingcontestrankhelpers
{

// ShouldRankBefore mirrors RankContestEntries' std::sort comparator:
// equal scores order by earlier submitTime; otherwise greatest-first or
// smallest-first (production SMALLEST form is bScore > aScore).
// greatest is true when measure == FISHING_CONTEST_MEASURE::GREATEST.
inline auto ShouldRankBefore(
    const std::uint32_t aScore,
    const std::uint32_t aSubmitTime,
    const std::uint32_t bScore,
    const std::uint32_t bSubmitTime,
    const bool          greatest) -> bool
{
    if (aScore == bScore)
    {
        return aSubmitTime < bSubmitTime;
    }
    if (greatest)
    {
        return aScore > bScore;
    }
    return bScore > aScore;
}

// AssignedContestRank mirrors the per-entry contestRank selection:
// a new sequential rank when the score differs from the running tally,
// otherwise the shared previous rank (e.g. 1, 1, 3).
inline auto AssignedContestRank(
    const std::uint32_t entryScore,
    const std::uint32_t runningScore,
    const std::uint8_t  sequentialRank,
    const std::uint8_t  previousRank) -> std::uint8_t
{
    return entryScore != runningScore ? sequentialRank : previousRank;
}

// ShouldAdvanceRunningScore is true when the host must update the running
// score tally and previousRank after assigning contestRank.
inline auto ShouldAdvanceRunningScore(const std::uint32_t entryScore, const std::uint32_t runningScore) -> bool
{
    return entryScore != runningScore;
}

// ScoreMatchesShare is the pure count_if predicate for per-score share:
// entries whose score equals the current running tally contribute to share.
inline auto ScoreMatchesShare(const std::uint32_t candidateScore, const std::uint32_t shareScore) -> bool
{
    return candidateScore == shareScore;
}

// RankDatasetB mirrors dataset_b = share (duplicated packet field).
inline auto RankDatasetB(const std::uint8_t share) -> std::uint8_t
{
    return share;
}

// RankResultCount mirrors resultCount = static_cast<uint8>(entries.size()).
inline auto RankResultCount(const std::size_t entryCount) -> std::uint8_t
{
    return static_cast<std::uint8_t>(entryCount);
}

} // namespace fishingcontestrankhelpers
