#pragma once

// Pure CDeathState virtual policy (slice 9282). These helpers keep the
// constant state restrictions shared by production and characterization tests.
namespace deathstatepolicy
{

constexpr auto canChangeState() -> bool { return false; }
constexpr auto canFollowPath() -> bool { return false; }
constexpr auto canInterrupt() -> bool { return false; }

} // namespace deathstatepolicy
