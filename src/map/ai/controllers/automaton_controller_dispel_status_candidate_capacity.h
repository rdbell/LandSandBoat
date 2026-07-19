#pragma once

namespace automatoncontrollerdispelstatuscandidate
{
inline auto CanSelectCandidate(bool alreadySelected, bool durationPositive, bool dispellable) -> bool
{
    return !alreadySelected && durationPositive && dispellable;
}
} // namespace automatoncontrollerdispelstatuscandidate
