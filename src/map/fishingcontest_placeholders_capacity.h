#pragma once

#include <cstdint>

// Pure GeneratePlaceholderEntries gate and score helpers (slice 2855).
// Host injects presenting / measure / entry-count scalars; helpers never touch
// FakeContestEntries storage, settings, or process-global contest state.

namespace fishingcontestplaceholderhelpers
{

// ShouldGeneratePlaceholderEntries mirrors GeneratePlaceholderEntries' early
// return: status must be PRESENTING and realEntries must be below maxEntries.
// presenting is true when status == FISHING_CONTEST_STATUS::PRESENTING.
inline auto ShouldGeneratePlaceholderEntries(
    const bool         presenting,
    const std::uint8_t maxEntries,
    const std::uint8_t realEntries) -> bool
{
    return presenting && realEntries < maxEntries;
}

// PlaceholderEntryScore mirrors the fake-entry score ladder for 1-based number:
//   smallest: 9999 - needed + number
//   greatest: (needed + 1) - number  (signed intermediate; may underflow to uint32)
// needed is maxEntries - realEntries. smallest is true when
// measure == FISHING_CONTEST_MEASURE::SMALLEST.
inline auto PlaceholderEntryScore(
    const std::uint8_t needed,
    const int          number,
    const bool         smallest) -> std::uint32_t
{
    if (smallest)
    {
        return static_cast<std::uint32_t>(9999 - needed + number);
    }
    // Preserve signed intermediate so greatest-ladder underflow matches production
    // assignment of a negative int to uint32 (e.g. needed=2, number=4 → 0xFFFFFFFF).
    return static_cast<std::uint32_t>((static_cast<int>(needed) + 1) - number);
}

} // namespace fishingcontestplaceholderhelpers
