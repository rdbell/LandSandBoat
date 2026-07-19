#pragma once

namespace trustcontrollercasttargetsource
{
// ShouldUseSelf reports whether a trust spell's target mask is exactly self-only.
constexpr auto ShouldUseSelf(const bool selfOnly) -> bool
{
    return selfOnly;
}
} // namespace trustcontrollercasttargetsource
