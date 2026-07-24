#pragma once

namespace zoneplaytimesave
{

// ShouldSaveCharacter mirrors SavePlayTime's unconditional list traversal.
constexpr auto ShouldSaveCharacter() -> bool
{
    return true;
}

} // namespace zoneplaytimesave
