#pragma once

#include <cstdint>

// Pure GeneratePlaceholderEntries gate and score helpers.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 2645: GeneratePlaceholderEntries pure port / field cycles / loop quirk
//   - 2855: ShouldGeneratePlaceholderEntries + PlaceholderEntryScore residual
//           dual-wire expand (gate + score pure helpers; host dual-wire)
//   - 3380: ShouldGeneratePlaceholderEntries prior dedicated dual-wire
//           (PRESENTING && realEntries < maxEntries;
//            residual expand 2855 / pure 2645 — formula unchanged)
//   - 3523: ShouldGeneratePlaceholderEntries dedicated dual-wire
//           (PRESENTING && realEntries < maxEntries;
//            residual expand 2855 / prior dedicated 3380 / pure 2645 —
//            formula unchanged)
//
// Host injects presenting / measure / entry-count scalars; helpers never touch
// FakeContestEntries storage, settings, or process-global contest state.
// Production host: fishingcontest::GeneratePlaceholderEntries injects
// presenting = (status == PRESENTING) into ShouldGeneratePlaceholderEntries
// and measure==SMALLEST into PlaceholderEntryScore.

namespace fishingcontestplaceholderhelpers
{

// ShouldGeneratePlaceholderEntries mirrors GeneratePlaceholderEntries' early
// return: status must be PRESENTING and realEntries must be below maxEntries.
// presenting is true when status == FISHING_CONTEST_STATUS::PRESENTING.
//
// Formula (slice 3523 dedicated dual-wire; residual expand 2855 / prior
// dedicated 3380 / pure 2645 — formula unchanged):
//   presenting && realEntries < maxEntries
//
// Dual-wire C++ / Go: fishingcontestplaceholderhelpers::ShouldGeneratePlaceholderEntries
// / fishingcontest.ShouldGeneratePlaceholderEntries. Residual dual-wire suite:
// slice 2855. Prior dedicated dual-wire suite: slice 3380. Dedicated dual-wire
// suite: slice 3523.
// Sibling score ladder PlaceholderEntryScore remains residual-ok under 2855.
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
// Residual dual-wire: slice 2855 (left residual under dedicated gate expand
// 3523 / 3380).
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
